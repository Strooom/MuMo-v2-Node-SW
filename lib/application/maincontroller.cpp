// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

// #define noTransmit
#include <applicationevent.hpp>
#include <battery.hpp>
#include <bme680.hpp>
#include <buildinfo.hpp>
#include <circularbuffer.hpp>
#include <display.hpp>
#include <gpio.hpp>
#include <graphics.hpp>
#include <hexascii.hpp>
#include <logging.hpp>
#include <lorawan.hpp>
#include <maccommand.hpp>
#include <maincontroller.hpp>
#include <measurementcollection.hpp>
#include <power.hpp>
#include <realtimeclock.hpp>
#include <screen.hpp>
#include <sensordevicecollection.hpp>
#include <settingscollection.hpp>
#include <stdio.h>        // snprintf
#include <tsl2591.hpp>
#include <uniqueid.hpp>
#include <version.hpp>

#ifndef generic
#include "main.h"
extern I2C_HandleTypeDef hi2c2;
void MX_I2C2_Init(void);
#endif

mainState mainController::state{mainState::boot};
uint32_t mainController::requestCounter{0};
uint32_t mainController::answerCounter{0};

extern circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void mainController::initialize() {
    if (nonVolatileStorage::isPresent()) {
        if (!settingsCollection::isInitialized()) {
            settingsCollection::initializeOnce();
        }
    }

    sensorDeviceCollection::discover();
    sensorDeviceCollection::set(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage, 0, 4);
    sensorDeviceCollection::set(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::temperature, 0, 4);
    sensorDeviceCollection::set(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::relativeHumidity, 0, 4);
    sensorDeviceCollection::set(static_cast<uint32_t>(sensorDeviceType::tsl2591), tsl2591::visibleLight, 0, 4);

    gpio::enableGpio(gpio::group::rfControl);
    LoRaWAN::initialize();

    gpio::enableGpio(gpio::group::spiDisplay);
    showDeviceInfo();

    measurementCollection::initialize();
    measurementCollection::findMeasurementsInEeprom();
}

void mainController::handleEvents() {
    while (applicationEventBuffer.hasEvents()) {
        applicationEvent theEvent = applicationEventBuffer.pop();
        logging::snprintf(logging::source::applicationEvents, "%s[%u] in %s[%u]\n", toString(theEvent), static_cast<uint8_t>(theEvent), toString(state), static_cast<uint32_t>(state));

        switch (state) {
            case mainState::boot:
                switch (theEvent) {
                    case applicationEvent::realTimeClockTick:
                        showLoRaWanConfig();
                        requestCounter++;
                        LoRaWAN::appendMacCommand(macCommand::linkCheckRequest);
                        LoRaWAN::appendMacCommand(macCommand::deviceTimeRequest);
                        LoRaWAN::sendUplink(0, nullptr, 0);
                        goTo(mainState::networkCheck);
                        break;
                    default:
                        break;
                }
                break;

            case mainState::networkCheck:
                switch (theEvent) {
                    case applicationEvent::downlinkMacCommandReceived:
                        answerCounter++;
                        showLoRaWanStatus();
                        if (answerCounter >= minNmbrAnswers) {
                            goTo(mainState::idle);
                        }
                        break;

                    case applicationEvent::realTimeClockTick:
                        requestCounter++;
                        // TODO : if we don't get answers, decrease DataRate and try again
                        showLoRaWanStatus();
                        if (requestCounter >= maxNmbrRequests) {
                            goTo(mainState::networkError);
                        } else {
                            LoRaWAN::appendMacCommand(macCommand::linkCheckRequest);
                            LoRaWAN::appendMacCommand(macCommand::deviceTimeRequest);
                            LoRaWAN::sendUplink(0, nullptr, 0);
                        }
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
                        if (sensorDeviceCollection::needsSampling()) {
                            sensorDeviceCollection::startSampling();
                            goTo(mainState::measuring);
                        } else {
                            sensorDeviceCollection::updateCounters();
                        }
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
                sensorDeviceCollection::updateCounters();
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
#ifndef noTransmit
                LoRaWAN::sendUplink(17, payLoadDataPtr, payloadLength);
#endif
                measurementCollection::setTransmitted(0, payloadLength);        // TODO : get correct frame counter *before* call to sendUplink
                goTo(mainState::networking);
            } else {
                goTo(mainState::idle);
            }
            break;

        case mainState::networking:
            if (LoRaWAN::isIdle()) {
                if (display::isPresent()) {
                    screen::show(screenType::measurements);
                }
                goTo(mainState::idle);
            }
            break;

        case mainState::displaying:
            goTo(mainState::idle);
            break;

        case mainState::idle:
            if (false) {
                //            if (!power::hasUsbPower()) {
                logging::snprintf("goSleep...\n");
                gpio::disableGpio(gpio::group::spiDisplay);
                gpio::disableGpio(gpio::group::writeProtect);
                gpio::disableGpio(gpio::group::uart1);
#ifndef generic
                HAL_I2C_DeInit(&hi2c2);
#endif
                gpio::disableGpio(gpio::group::usbPresent);
                gpio::disableGpio(gpio::group::rfControl);

                sleep();

                gpio::enableGpio(gpio::group::rfControl);
                gpio::enableGpio(gpio::group::usbPresent);
#ifndef generic
                MX_I2C2_Init();
#endif
                gpio::enableGpio(gpio::group::uart1);
                gpio::enableGpio(gpio::group::writeProtect);
                gpio::enableGpio(gpio::group::spiDisplay);
                logging::snprintf("...wakeUp\n");
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
    // Prepare before going to sleep
    switch (state) {
        case mainState::idle:
            break;

        default:
            break;
    }
    // Sleep mode depending on state we are in
    switch (state) {
        case mainState::idle:
#ifndef generic
        {
            uint32_t currentPriMaskState = __get_PRIMASK();
            __disable_irq();
            HAL_SuspendTick();
            HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
            HAL_ResumeTick();
            __set_PRIMASK(currentPriMaskState);
        }
#endif
        break;

        default:
            break;
    }

    // Repair after waking up
    switch (state) {
        case mainState::idle:
            break;

        default:
            break;
    }
}

void mainController::showDeviceInfo() {
    char tmpString[screen::maxTextLength2 + 1];
    screen::clearAllTexts();
    snprintf(tmpString, screen::maxTextLength2, "MuMo %s", version::getIsVersionAsString());
    screen::setText(0, tmpString);
    screen::setText(1, "CC 4.0 BY-NC-SA");
    screen::setText(2, buildInfo::buildTimeStamp);
    hexAscii::uint64ToHexString(tmpString, uniqueId::get());
    screen::setText(3, tmpString);
    screen::setText(4, toString(battery::type));

    for (uint32_t sensorDeviceIndex = 2; sensorDeviceIndex < static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices); sensorDeviceIndex++) {
        if (sensorDeviceCollection::isValid(sensorDeviceIndex)) {
            screen::setText(3U + sensorDeviceIndex, sensorDeviceCollection::name(sensorDeviceIndex));
        }
    }
    screen::show(screenType::message);
}

void mainController::showLoRaWanConfig() {
    char tmpString[screen::maxTextLength2 + 1];
    screen::clearAllTexts();
    uint16_t devAddrEnd        = LoRaWAN::DevAddr.asUint8[0] + (LoRaWAN::DevAddr.asUint8[1] << 8);
    uint16_t applicationKeyEnd = LoRaWAN::applicationKey.asBytes()[15] + (LoRaWAN::applicationKey.asBytes()[14] << 8);
    uint16_t networkKeyEnd     = LoRaWAN::networkKey.asBytes()[15] + (LoRaWAN::networkKey.asBytes()[14] << 8);
    snprintf(tmpString, screen::maxTextLength2, "%04X %04X %04X", devAddrEnd, applicationKeyEnd, networkKeyEnd);
    screen::setText(0, tmpString);
    snprintf(tmpString, screen::maxTextLength2, "DataRate : %u", static_cast<uint8_t>(LoRaWAN::currentDataRateIndex));
    screen::setText(2, tmpString);
    snprintf(tmpString, screen::maxTextLength2, "rx1Delay : %u", static_cast<uint8_t>(LoRaWAN::rx1DelayInSeconds));
    screen::setText(3, tmpString);
    screen::show(screenType::message);
}

void mainController::showLoRaWanStatus() {
    char tmpString[screen::maxTextLength2 + 1];
    screen::clearAllTexts();
    snprintf(tmpString, screen::maxTextLength2, "requests %u", static_cast<uint8_t>(requestCounter));
    screen::setText(0, tmpString);
    snprintf(tmpString, screen::maxTextLength2, "answers %u", static_cast<uint8_t>(answerCounter));
    screen::setText(1, tmpString);
    snprintf(tmpString, screen::maxTextLength2, "DataRate : %u", static_cast<uint8_t>(LoRaWAN::currentDataRateIndex));
    screen::setText(2, tmpString);
    snprintf(tmpString, screen::maxTextLength2, "rx1Delay : %u", static_cast<uint8_t>(LoRaWAN::rx1DelayInSeconds));
    screen::setText(3, tmpString);
    snprintf(tmpString, screen::maxTextLength2, "Margin : %u", static_cast<uint8_t>(LoRaWAN::margin));
    screen::setText(4, tmpString);
    snprintf(tmpString, screen::maxTextLength2, "Gateways : %u", static_cast<uint8_t>(LoRaWAN::gatewayCount));
    screen::setText(5, tmpString);
    time_t localTime = realTimeClock::get();
    screen::setText(6, ctime(&localTime));
    screen::show(screenType::message);
}
