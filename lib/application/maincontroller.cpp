// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <applicationevent.hpp>
#include <battery.hpp>
#include <bme680.hpp>
#include <sht40.hpp>
#include <buildinfo.hpp>
#include <circularbuffer.hpp>
#include <cli.hpp>
#include <display.hpp>
#include <float.hpp>
#include <gpio.hpp>
#include <graphics.hpp>
#include <hexascii.hpp>
#include <i2c.hpp>
#include <logging.hpp>
#include <lorawan.hpp>
#include <lptim.hpp>
#include <maccommand.hpp>
#include <maincontroller.hpp>
#include <measurementcollection.hpp>
#include <power.hpp>
#include <realtimeclock.hpp>
#include <screen.hpp>
#include <sensordevicecollection.hpp>
#include <settingscollection.hpp>
#include <spi.hpp>
#include <stdio.h>
#include <tsl2591.hpp>
#include <uart.hpp>
#include <uniqueid.hpp>
#include <version.hpp>
#include <mcutype.hpp>
#include <sx126x.hpp>

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
const uint32_t mainController::deviceIndex[screen::nmbrOfMeasurementTextLines]{2, 2, 3};
const uint32_t mainController::channelIndex[screen::nmbrOfMeasurementTextLines]{0, 1, 0};

extern circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void mainController::initialize() {
    logging::enable(logging::destination::uart1);
    logging::enable(logging::source::applicationEvents);
    logging::enable(logging::source::settings);
    logging::enable(logging::source::error);
    logging::enable(logging::source::criticalError);
    realTimeClock::initialize();
    version::initialize();

    logging::snprintf("https://github.com/Strooom - %s\n", version::getIsVersionAsString());
    logging::snprintf("%s %s build - %s\n", toString(version::getBuildEnvironment()), toString(version::getBuildType()), buildInfo::buildTimeStamp);
    logging::snprintf("Creative Commons 4.0 - BY-NC-SA\n");
    char tmpKeyAsHexAscii[17];
    hexAscii::uint64ToHexString(tmpKeyAsHexAscii, uniqueId::get());
    logging::snprintf("UID     : %s\n", tmpKeyAsHexAscii);

    spi::wakeUp();
    display::detectPresence();
    logging::snprintf(logging::source::settings, "Display : %s\n", display::isPresent() ? "present" : "not present");
    spi::goSleep();

    i2c::wakeUp();
    if (nonVolatileStorage::isPresent()) {
        logging::snprintf(logging::source::settings, "EEPROM  : present\n");
    } else {
        logging::snprintf(logging::source::criticalError, "no EEPROM\n");
        state = mainState::fatalError;
        return;
    }

    batteryType theBatteryType = static_cast<batteryType>(settingsCollection::read<uint8_t>(settingsCollection::settingIndex::batteryType));
    if (battery::isValidType(theBatteryType)) {
        battery::initialize(theBatteryType);
        logging::snprintf(logging::source::settings, "Battery : %s (%d)\n", toString(theBatteryType), static_cast<uint8_t>(theBatteryType));
    } else {
        logging::snprintf(logging::source::criticalError, "invalid BatteryType %d\n", static_cast<uint8_t>(theBatteryType));
    }

    mcuType theMcuType = static_cast<mcuType>(settingsCollection::read<uint8_t>(settingsCollection::settingIndex::mcuType));
    if (sx126x::isValidType(theMcuType)) {
        sx126x::initialize(theMcuType);
        logging::snprintf(logging::source::settings, "Radio   : %s (%d)\n", toString(theMcuType), static_cast<uint8_t>(theMcuType));
    } else {
        logging::snprintf(logging::source::criticalError, "invalid RadioType %d\n", static_cast<uint8_t>(theMcuType));
        state = mainState::fatalError;
        return;
    }


    sensorDeviceCollection::discover();
    logging::snprintf("BME680  : %s\n", bme680::isPresent() ? "present" : "not present");
    logging::snprintf("SHT40   : %s\n", sht40::isPresent() ? "present" : "not present");
    logging::snprintf("TSL2591 : %s\n", tsl2591::isPresent() ? "present" : "not present");

    sensorDeviceCollection::set(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage, 3, 720);
    sensorDeviceCollection::set(static_cast<uint32_t>(sensorDeviceType::battery), battery::percentCharged, 3, 720);
    sensorDeviceCollection::set(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::temperature, 0, 30);
    sensorDeviceCollection::set(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::relativeHumidity, 0, 30);
    sensorDeviceCollection::set(static_cast<uint32_t>(sensorDeviceType::tsl2591), tsl2591::visibleLight, 2, 10);

    // TODO : initialize measurementCollection

    i2c::goSleep();

    gpio::enableGpio(gpio::group::rfControl);
    LoRaWAN::initialize();
    if (LoRaWAN::isValidConfig()) {
        char tmpString1[64];
        char tmpString2[64];
        hexAscii::uint32ToHexString(tmpString2, LoRaWAN::DevAddr.asUint32);
        snprintf(tmpString1, screen::maxConsoleTextLength, "DevAdrr : %s", tmpString2);
        logging::snprintf("%s\n", tmpString1);
        hexAscii::byteArrayToHexString(tmpString2, LoRaWAN::applicationKey.asBytes(), 16);
        snprintf(tmpString1, screen::maxConsoleTextLength, "AppSKey : %s", tmpString2);
        logging::snprintf("%s\n", tmpString1);
        hexAscii::byteArrayToHexString(tmpString2, LoRaWAN::networkKey.asBytes(), 16);
        snprintf(tmpString1, screen::maxConsoleTextLength, "NwkSKey : %s", tmpString2);
        logging::snprintf("%s\n", tmpString1);
        snprintf(tmpString1, screen::maxConsoleTextLength, "DataRate: %u", static_cast<uint8_t>(LoRaWAN::currentDataRateIndex));
        logging::snprintf("%s\n", tmpString1);
        snprintf(tmpString1, screen::maxConsoleTextLength, "rx1Delay: %u", static_cast<uint8_t>(LoRaWAN::rx1DelayInSeconds));
        logging::snprintf("%s\n", tmpString1);
    } else {
        logging::snprintf(logging::source::criticalError, "no valid LoRaWAN config\n");
        state = mainState::fatalError;
        return;
    }
    applicationEventBuffer.initialize();        // clear RTCticks which may already have been generated
    // goTo(mainState::networkCheck);
    goTo(mainState::idle);
}

void mainController::handleEvents() {
    while (applicationEventBuffer.hasEvents()) {
        applicationEvent theEvent = applicationEventBuffer.pop();
        logging::snprintf(logging::source::applicationEvents, "%s[%u] in %s[%u]\n", toString(theEvent), static_cast<uint8_t>(theEvent), toString(state), static_cast<uint32_t>(state));

        switch (state) {
            case mainState::networkCheck:
                handleEventsStateNetworkCheck(theEvent);
                break;

            case mainState::idle:
                handleEventsStateIdle(theEvent);
                break;

            case mainState::networking:
                handleEventsStateNetworking(theEvent);
                break;

            default:
                break;
        }
    }
}

void mainController::handleEventsStateNetworkCheck(applicationEvent theEvent) {
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
                goTo(mainState::fatalError);
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
}

void mainController::handleEventsStateIdle(applicationEvent theEvent) {
    switch (theEvent) {
        case applicationEvent::realTimeClockTick:
            if (realTimeClock::needsSync()) {
                LoRaWAN::appendMacCommand(macCommand::deviceTimeRequest);
            }
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
}

void mainController::handleEventsStateNetworking(applicationEvent theEvent) {
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
}

void mainController::runStateMachine() {
    switch (state) {
        case mainState::boot:
        case mainState::idle:
        case mainState::fatalError:
        case mainState::networkCheck:
            break;

        case mainState::measuring:
            sensorDeviceCollection::run();
            if (sensorDeviceCollection::isSamplingReady()) {
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
                // measurementCollection::saveNewMeasurementsToEeprom();

                uint32_t payloadLength        = measurementCollection::nmbrOfBytesToTransmit();
                const uint8_t* payLoadDataPtr = measurementCollection::getTransmitBuffer();
                // LoRaWAN::sendUplink(17, payLoadDataPtr, payloadLength); temporarily disabled TODO : enable again
                measurementCollection::setTransmitted(0, payloadLength);        // TODO : get correct frame counter *before* call to sendUplink
                goTo(mainState::networking);
            } else {
                goTo(mainState::idle);
            }
            break;

        case mainState::networking:
            if (LoRaWAN::isIdle()) {
                // showMain();
                goTo(mainState::idle);
            }
            break;

        default:
            break;
    }
}

void mainController::goTo(mainState newState) {
    logging::snprintf(logging::source::applicationEvents, "mainController stateChange : %s[%u] -> %s[%u]\n", toString(state), static_cast<uint32_t>(state), toString(newState), static_cast<uint32_t>(newState));
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
            MX_I2C2_Init();
            MX_ADC_Init();
            MX_AES_Init();
            MX_RNG_Init();
#endif
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

void mainController::showLoRaWanStatus() {
    char tmpString[screen::maxConsoleTextLength + 1];
    screen::setType(screenType::loraStatus);
    screen::clearConsole();
    snprintf(tmpString, screen::maxConsoleTextLength, "Requests : %u", static_cast<uint8_t>(requestCounter));
    screen::setText(0, tmpString);
    snprintf(tmpString, screen::maxConsoleTextLength, "Answers : %u", static_cast<uint8_t>(answerCounter));
    screen::setText(1, tmpString);
    snprintf(tmpString, screen::maxConsoleTextLength, "DataRate : %u", static_cast<uint8_t>(LoRaWAN::currentDataRateIndex));
    screen::setText(2, tmpString);
    snprintf(tmpString, screen::maxConsoleTextLength, "rx1Delay : %u", static_cast<uint8_t>(LoRaWAN::rx1DelayInSeconds));
    screen::setText(3, tmpString);
    snprintf(tmpString, screen::maxConsoleTextLength, "Margin : %u", static_cast<uint8_t>(LoRaWAN::margin));
    screen::setText(4, tmpString);
    snprintf(tmpString, screen::maxConsoleTextLength, "Gateways : %u", static_cast<uint8_t>(LoRaWAN::gatewayCount));
    screen::setText(5, tmpString);
    time_t rtcTime            = realTimeClock::get();
    const struct tm* rtcTime2 = localtime(&rtcTime);
    strftime(tmpString, screen::maxConsoleTextLength, "Date : %Y-%b-%d", rtcTime2);
    screen::setText(6, tmpString);
    strftime(tmpString, screen::maxConsoleTextLength, "Time : %H:%M:%S", rtcTime2);
    screen::setText(7, tmpString);
}

void mainController::showMain() {
    for (uint32_t lineIndex = 0; lineIndex < screen::nmbrOfMeasurementTextLines; lineIndex++) {
        float value       = sensorDeviceCollection::value(deviceIndex[lineIndex], channelIndex[lineIndex]);
        uint32_t decimals = sensorDeviceCollection::decimals(deviceIndex[lineIndex], channelIndex[lineIndex]);
        char textBig[8];
        char textSmall[8];
        snprintf(textBig, 8, "%d", static_cast<int>(integerPart(value, decimals)));
        if (decimals > 0) {
            snprintf(textSmall, 8, "%d  %s", static_cast<int>(fractionalPart(value, decimals)), sensorDeviceCollection::units(deviceIndex[lineIndex], channelIndex[lineIndex]));
        } else {
            snprintf(textSmall, 8, "%s", sensorDeviceCollection::units(deviceIndex[lineIndex], channelIndex[lineIndex]));
        }
        screen::setText(lineIndex, textBig, textSmall);
    }
}

void mainController::mcuStop2() {
#ifndef generic
    uint32_t currentPriMaskState = __get_PRIMASK();
    __disable_irq();
    HAL_SuspendTick();
    HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
    HAL_ResumeTick();
    __set_PRIMASK(currentPriMaskState);
#endif
}

void mainController::miniAdr() {
    if (answerCounter == 0) {
        int32_t newDataRateIndex = 5 - (requestCounter / 2);
        if (newDataRateIndex < 0) {
            newDataRateIndex = 0;
        }
        LoRaWAN::currentDataRateIndex = newDataRateIndex;
    }
}

void mainController::runDisplayUpdate() {
    switch (state) {
        case mainState::networking:
            break;

        default:
            if (display::isPresent()) {
                if (screen::isModified() || ((realTimeClock::tickCounter % display::refreshPeriodInTicks) == (display::refreshPeriodInTicks - 1))) {
                    screen::update();
                }
            }
            break;
    }
}

void mainController::runCli() {
    switch (state) {
        case mainState::networking:
            break;

        default:
            if (power::hasUsbPower()) {
                cli::run();
            }
            break;
    }
}

void mainController::runUsbPowerDetection() {
    if (power::isUsbConnected()) {
        applicationEventBuffer.push(applicationEvent::usbConnected);
        // uart2::initialize();
        // gpio::enableGpio(gpio::group::uart2);
        // screen::setUsbStatus(true);
    }
    if (power::isUsbRemoved()) {
        applicationEventBuffer.push(applicationEvent::usbRemoved);
        // gpio::disableGpio(gpio::group::uart2);
        // screen::setUsbStatus(false);
    }
}
