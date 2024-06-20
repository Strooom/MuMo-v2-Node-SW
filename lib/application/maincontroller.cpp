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
#include <lptim.hpp>

#ifndef generic
#include "main.h"
extern I2C_HandleTypeDef hi2c2;
extern SPI_HandleTypeDef hspi2;
extern ADC_HandleTypeDef hadc;
extern RNG_HandleTypeDef hrng;
extern CRYP_HandleTypeDef hcryp;

void MX_I2C2_Init(void);
void MX_SPI2_Init(void);
void MX_ADC_Init(void);
void MX_AES_Init(void);
void MX_RNG_Init(void);
void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
#endif

mainState mainController::state{mainState::boot};
uint32_t mainController::requestCounter{0};
uint32_t mainController::answerCounter{0};
uint32_t mainController::deviceIndex[screen::numberOfLines]{1, 3, 2};
uint32_t mainController::channelIndex[screen::numberOfLines]{0, 0, 1};

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
    // measurementCollection::findMeasurementsInEeprom();
}

void mainController::handleEvents() {
    while (applicationEventBuffer.hasEvents()) {
        applicationEvent theEvent = applicationEventBuffer.pop();
        logging::snprintf(logging::source::applicationEvents, "%s[%u] in %s[%u]\n", toString(theEvent), static_cast<uint8_t>(theEvent), toString(state), static_cast<uint32_t>(state));

        switch (state) {
            case mainState::boot:
                switch (theEvent) {
                    case applicationEvent::lowPowerTimerExpired:
                        lptim::stop();
                        break;

                    case applicationEvent::realTimeClockTick:
                        if (!lptim::isRunning()) {
                            showLoRaWanConfig();
                            requestCounter++;
                            LoRaWAN::appendMacCommand(macCommand::linkCheckRequest);
                            LoRaWAN::appendMacCommand(macCommand::deviceTimeRequest);
                            LoRaWAN::sendUplink(0, nullptr, 0);
                            goTo(mainState::networkCheck);
                        }
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
                        miniAdr();
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
        case mainState::boot:
            mcuStop2();
            break;

        case mainState::idle:
        case mainState::networkError:
            if (!power::hasUsbPower()) {
                sleep();
            }
            break;

        case mainState::networkCheck:
            if (LoRaWAN::isIdle()) {
                if (!power::hasUsbPower()) {
                    mcuStop2();
                }
            }
            break;

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
                     // Here we need to update values to be shown on the screen...
                     // Then showing them can happen in idle later-on
                    screen::show(screenType::measurements);
                }
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
    // Prepare before going to sleep
    switch (state) {
        case mainState::idle:
            logging::snprintf("goSleep...\n");
            gpio::disableAllGpio();
            break;

        default:
            break;
    }
    // Sleep mode depending on state we are in
    switch (state) {
        case mainState::idle:
            mcuStop2();
            break;

        default:
            break;
    }

    // Repair after waking up
    switch (state) {
        case mainState::idle:
#ifndef generic
            // HAL_Delay(1000);
            MX_I2C2_Init();
            MX_ADC_Init();
            MX_AES_Init();
            MX_RNG_Init();
            // MX_USART1_UART_Init();
#endif
            // HAL_Delay(1000);
            gpio::enableGpio(gpio::group::rfControl);
            gpio::enableGpio(gpio::group::usbPresent);
            gpio::enableGpio(gpio::group::i2c);
            gpio::enableGpio(gpio::group::writeProtect);
            logging::snprintf("...wakeUp\n");
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

void mainController::mcuStop2() {
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
}

void mainController::miniAdr() {
    if (answerCounter == 0) {
        int32_t newDataRateIndex = 5 - (requestCounter / 2);
        if (newDataRateIndex < 0) {
            newDataRateIndex = 0;
        }
        if (newDataRateIndex > 5) {
            newDataRateIndex = 5;
        }
        LoRaWAN::currentDataRateIndex = newDataRateIndex;
    }
}