// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <maincontroller.hpp>
#include <applicationevent.hpp>
#include <circularbuffer.hpp>
#include <power.hpp>
#include <gpio.hpp>
#include <sensordevicecollection.hpp>
#include <lorawan.hpp>
#include <display.hpp>
#include <graphics.hpp>
#include <screen.hpp>

// #include <linearbuffer.hpp>
#include <version.hpp>
// #include <buildinfo.hpp>

#include <settingscollection.hpp>
#include <measurementcollection.hpp>
// #include <aeskey.hpp>
// #include <datarate.hpp>
#include <maccommand.hpp>
#include <logging.hpp>

#ifndef generic
#include "main.h"
extern I2C_HandleTypeDef hi2c2;
void MX_I2C2_Init(void);
#endif

mainState mainController::state{mainState::boot};
extern circularBuffer<applicationEvent, 16U> applicationEventBuffer;
extern font roboto36bold;
extern font tahoma24bold;
extern font lucidaConsole12;

void mainController::initialize() {
    if (nonVolatileStorage::isPresent()) {
        if (!settingsCollection::isInitialized()) {
            settingsCollection::initializeOnce();
        }
    }

    sensorDeviceCollection::discover();
    measurementCollection::initialize();
    measurementCollection::findMeasurementsInEeprom();

    gpio::enableGpio(gpio::group::spiDisplay);
    gpio::enableGpio(gpio::group::uart1);
    gpio::enableGpio(gpio::group::rfControl);

    LoRaWAN::initialize();

    display::initialize();
    display::clearAllPixels();
    graphics::drawText(4, 160, tahoma24bold, version::getIsVersionAsString());
    display::startUpdate();

    goTo(mainState::waitForBootScreen);
}

void mainController::handleEvents() {
    while (applicationEventBuffer.hasEvents()) {
        applicationEvent theEvent = applicationEventBuffer.pop();
        logging::snprintf(logging::source::applicationEvents, "%s[%u] in %s[%u]\n", toString(theEvent), static_cast<uint8_t>(theEvent), toString(state), static_cast<uint32_t>(state));

        switch (state) {
            case mainState::waitForBootScreen:
                switch (theEvent) {
                    case applicationEvent::realTimeClockTick:
                        goTo(mainState::waitForNetworkResponse);
                        break;

                    default:
                        break;
                }
                break;

            case mainState::waitForNetworkRequest:
                switch (theEvent) {
                    case applicationEvent::realTimeClockTick:
                        LoRaWAN::appendMacCommand(macCommand::linkCheckRequest);
                        LoRaWAN::appendMacCommand(macCommand::deviceTimeRequest);
                        LoRaWAN::sendUplink(0, nullptr, 0);
                        goTo(mainState::waitForNetworkResponse);
                        break;
                }
                break;

            case mainState::waitForNetworkResponse:
                switch (theEvent) {
                    case applicationEvent::downlinkMacCommandReceived:
                        display::initialize();
                        display::clearAllPixels();
                        graphics::drawText(4, 160, tahoma24bold, "network OK");
                        display::startUpdate();
                        goTo(mainState::idle);
                        break;

                    case applicationEvent::realTimeClockTick:
                        // decrease DataRate and try again
                        LoRaWAN::appendMacCommand(macCommand::linkCheckRequest);
                        LoRaWAN::appendMacCommand(macCommand::deviceTimeRequest);
                        LoRaWAN::sendUplink(0, nullptr, 0);
                        break;

                    case applicationEvent::lowPowerTimerExpired:
                    case applicationEvent::sx126xTxComplete:
                    case applicationEvent::sx126xRxComplete:
                    case applicationEvent::sx126xTimeout:
                        LoRaWAN::handleEvents(theEvent);
                        break;

                    default:
                        break;
                }
                break;

            case mainState::idle:
                switch (theEvent) {
                    case applicationEvent::realTimeClockTick:
                        sensorDeviceCollection::tick();
                        goTo(mainState::measuring);
                        break;

                    default:
                        break;
                }
                break;

            case mainState::networking:
                switch (theEvent) {
                    case applicationEvent::lowPowerTimerExpired:
                    case applicationEvent::sx126xTxComplete:
                    case applicationEvent::sx126xRxComplete:
                    case applicationEvent::sx126xTimeout:
                        LoRaWAN::handleEvents(theEvent);
                        break;

                    default:
                        break;
                }

            default:
                break;
        }

        switch (theEvent) {
            case applicationEvent::usbConnected:
                // MX_USART2_UART_Init();
                break;

            case applicationEvent::usbRemoved:
                // MX_USART2_UART_DeInit();
                break;

            default:
                break;
        }
    }
}

void mainController::run() {
    if (power::isUsbConnected()) {
        applicationEventBuffer.push(applicationEvent::usbConnected);
    }
    if (power::isUsbRemoved()) {
        applicationEventBuffer.push(applicationEvent::usbRemoved);
    }

    switch (state) {
        case mainState::measuring:
            sensorDeviceCollection::run();
            if (sensorDeviceCollection::isSleeping()) {
                goTo(mainState::logging);
            }
            break;

        case mainState::logging:
            if (sensorDeviceCollection::hasNewMeasurements()) {
                if (logging::isActive(logging::source::sensorData)) {
                    sensorDeviceCollection::log();
                }
                sensorDeviceCollection::collectNewMeasurements();
                measurementCollection::saveNewMeasurementsToEeprom();
                uint32_t payloadLength        = measurementCollection::nmbrOfBytesToTransmit();
                const uint8_t* payLoadDataPtr = measurementCollection::getTransmitBuffer();
                logging::snprintf("--> %d bytes to transmit\n", payloadLength);
                LoRaWAN::sendUplink(17, payLoadDataPtr, payloadLength);
                measurementCollection::setTransmitted(0, payloadLength);        // TODO : get correct frame counter *before* call to sendUplink
                goTo(mainState::networking);
            } else {
                goTo(mainState::idle);
            }
            break;

        case mainState::networking:
            if (LoRaWAN::isIdle()) {
                if (display::isPresent()) {
                    // goTo(mainState::displaying);
                    screen::showMeasurements();
                    goTo(mainState::idle);
                } else {
                    goTo(mainState::idle);
                }
            }
            break;

        case mainState::displaying:
            if (display::isReady()) {
                goTo(mainState::idle);
            }
            break;

        case mainState::idle:

            if (!power::hasUsbPower()) {
                gpio::disableGpio(gpio::group::spiDisplay);
                gpio::disableGpio(gpio::group::writeProtect);
                gpio::disableGpio(gpio::group::uart1);
#ifndef generic
                HAL_I2C_DeInit(&hi2c2);
#endif
                gpio::disableGpio(gpio::group::usbPresent);
                gpio::disableGpio(gpio::group::rfControl);
                goTo(mainState::sleeping);

                sleep();

                gpio::disableGpio(gpio::group::rfControl);
                gpio::enableGpio(gpio::group::usbPresent);
#ifndef generic
                MX_I2C2_Init();
#endif
                gpio::enableGpio(gpio::group::uart1);
                gpio::enableGpio(gpio::group::writeProtect);
                gpio::enableGpio(gpio::group::spiDisplay);

                goTo(mainState::idle);
            }
            break;

        default:
            break;
    }
}

void mainController::goTo(mainState newState) {
    logging::snprintf(logging::source::applicationEvents, "MainController stateChange : %s[%u] -> %s[%u]\n", toString(state), static_cast<uint32_t>(state), toString(newState), static_cast<uint32_t>(newState));
    state = newState;
}

void mainController::sleep() {
#ifndef generic
    uint32_t currentPriMaskState = __get_PRIMASK();
    __disable_irq();
    HAL_SuspendTick();
    HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
    HAL_ResumeTick();
    __set_PRIMASK(currentPriMaskState);
#endif
}
