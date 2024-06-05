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
#include <screen.hpp>

// #include <linearbuffer.hpp>
// #include <version.hpp>
// #include <buildinfo.hpp>
#include <settingscollection.hpp>
#include <measurementcollection.hpp>
// #include <aeskey.hpp>
// #include <datarate.hpp>
// #include <maccommand.hpp>

#include <logging.hpp>

#ifndef generic
#include "main.h"
extern I2C_HandleTypeDef hi2c2;
void MX_I2C2_Init(void);
#endif

mainState mainController::state{mainState::boot};
extern circularBuffer<applicationEvent, 16U> applicationEventBuffer;

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
    // goTo(mainState::waitingForNetwork);
    goTo(mainState::idle);
}

void mainController::handleEvents() {
    while (applicationEventBuffer.hasEvents()) {
        applicationEvent theEvent = applicationEventBuffer.pop();
        logging::snprintf(logging::source::applicationEvents, "Application event : %s[%u]\n", toString(theEvent), static_cast<uint8_t>(theEvent));
        switch (theEvent) {
            case applicationEvent::usbConnected:
                // MX_USART2_UART_Init();
                break;

            case applicationEvent::usbRemoved:
                // MX_USART2_UART_DeInit();
                break;

            case applicationEvent::downlinkApplicationPayloadReceived: {
                // byteBuffer receivedData;
                // LoRaWAN::getReceivedDownlinkMessage(receivedData);
            } break;

            case applicationEvent::realTimeClockTick: {
                {
                    switch (state) {
                        case mainState::waitingForNetwork: {
                            // LoRaWAN::macOut.append(static_cast<uint8_t>(macCommand::linkCheckRequest));
                            // LoRaWAN::sendUplink(1, nullptr, 0);
                        } break;

                        case mainState::waitingForTime: {
                            // LoRaWAN::macOut.append(static_cast<uint8_t>(macCommand::deviceTimeRequest));
                            // LoRaWAN::sendUplink(1, nullptr, 0);
                        } break;

                        case mainState::idle:
                            sensorDeviceCollection::tick();
                            goTo(mainState::measuring);
                            break;

                        default:
                            logging::snprintf(logging::source::error, "RealTimeClockTick event received in state %s[%d] - Ignored\n", toString(state), static_cast<uint32_t>(state));
                            break;
                    }
                }
            } break;

            case applicationEvent::lowPowerTimerExpired:
            case applicationEvent::sx126xTxComplete:
            case applicationEvent::sx126xRxComplete:
            case applicationEvent::sx126xTimeout:
                LoRaWAN::handleEvents(theEvent);
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
        case mainState::waitingForNetwork:
            break;

        case mainState::waitingForTime:
            break;

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
                uint32_t payloadLength = measurementCollection::nmbrOfBytesToTransmit();
                const uint8_t* payLoadDataPtr = measurementCollection::getTransmitBuffer();
                logging::snprintf("--> %d bytes to transmit\n", payloadLength);
                LoRaWAN::sendUplink(17, payLoadDataPtr, payloadLength);
                measurementCollection::setTransmitted(0, payloadLength); // TODO : get correct frame counter *before* call to sendUplink
                goTo(mainState::networking);
            } else {
                goTo(mainState::idle);
            }
            break;

        case mainState::networking:
            if (LoRaWAN::isIdle()) {
                if (display::isPresent()) {
                    goTo(mainState::displaying);
                    screen::showMeasurements();
                } else {
                    goTo(mainState::idle);
                }
            }
            break;

        case mainState::displaying:
            goTo(mainState::idle);
            break;

        case mainState::idle:
            if (false) {
                //             if (!power::hasUsbPower()) {
                gpio::disableGpio(gpio::group::spiDisplay);
                gpio::disableGpio(gpio::group::writeProtect);
                gpio::disableGpio(gpio::group::uart1);
#ifndef generic
                HAL_I2C_DeInit(&hi2c2);
#endif
                gpio::disableGpio(gpio::group::usbPresent);
                gpio::disableGpio(gpio::group::rfControl);
                goTo(mainState::sleeping);
#ifndef generic
                uint32_t currentPriMaskState = __get_PRIMASK();
                __disable_irq();
                HAL_SuspendTick();
                HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
                HAL_ResumeTick();
                __set_PRIMASK(currentPriMaskState);
#endif
                gpio::disableGpio(gpio::group::rfControl);
                gpio::enableGpio(gpio::group::usbPresent);
#ifndef generic
                MX_I2C2_Init();
#endif
                gpio::enableGpio(gpio::group::uart1);
                gpio::enableGpio(gpio::group::writeProtect);
                gpio::enableGpio(gpio::group::spiDisplay);

                goTo(mainState::idle);        // ??
            }
            break;

        case mainState::test:
            break;

        default:
            // Error : we are in an unknown state
            break;
    }
}

void mainController::goTo(mainState newState) {
    logging::snprintf(logging::source::applicationEvents, "MainController stateChange : %s[%u] -> %s[%u]\n", toString(state), static_cast<uint32_t>(state), toString(newState), static_cast<uint32_t>(newState));
    state = newState;
}
