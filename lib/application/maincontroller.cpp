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
#include <buildinfo.hpp>
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
    logging::enable(logging::source::lorawanMac);
    logging::enable(logging::source::lorawanData);
    logging::enable(logging::source::error);
    logging::enable(logging::source::criticalError);
    realTimeClock::initialize();

    logging::snprintf("\n\n\nhttps://github.com/Strooom\n");
    logging::snprintf("v%u.%u.%u - #%s\n", buildInfo::mainVersionDigit, buildInfo::minorVersionDigit, buildInfo::patchVersionDigit, buildInfo::lastCommitTag);
    logging::snprintf("%s %s build - %s\n", toString(buildInfo::theBuildEnvironment), toString(buildInfo::theBuildType), buildInfo::buildTimeStamp);
    logging::snprintf("Creative Commons 4.0 - BY-NC-SA\n");

    char tmpKeyAsHexAscii[17];
    hexAscii::uint64ToHexString(tmpKeyAsHexAscii, uniqueId::get());
    logging::snprintf("\n");
    logging::snprintf("UID      : %s\n", tmpKeyAsHexAscii);

    spi::wakeUp();
    display::detectPresence();
    logging::snprintf(logging::source::settings, "Display  : %s\n", display::isPresent() ? "present" : "not present");
    if (display::isPresent()) {
        screen::setType(screenType::logo);
        screen::update();
    }
    spi::goSleep();

    i2c::wakeUp();
    uint32_t nmbr64KBlocks = nonVolatileStorage::isPresent();
    if (nmbr64KBlocks > 0) {
        logging::snprintf(logging::source::settings, "EEPROM   : %d * 64K present\n", nmbr64KBlocks);
    } else {
        logging::snprintf(logging::source::criticalError, "no EEPROM\n");
        state = mainState::fatalError;
        return;
    }

    batteryType theBatteryType = static_cast<batteryType>(settingsCollection::read<uint8_t>(settingsCollection::settingIndex::batteryType));
    if (battery::isValidType(theBatteryType)) {
        battery::initialize(theBatteryType);
        logging::snprintf(logging::source::settings, "Battery  : %s (%d)\n", toString(theBatteryType), static_cast<uint8_t>(theBatteryType));
        sensorDeviceCollection::isPresent[static_cast<uint32_t>(sensorDeviceType::battery)] = true;

        sensorDeviceCollection::set(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage, 0, 20);
        sensorDeviceCollection::set(static_cast<uint32_t>(sensorDeviceType::battery), battery::stateOfCharge, 0, 20);
    } else {
        logging::snprintf(logging::source::criticalError, "invalid BatteryType %d\n", static_cast<uint8_t>(theBatteryType));
        // settingsCollection::save(static_cast<uint8_t>(batteryType::liFePO4_700mAh), settingsCollection::settingIndex::batteryType);        // comment out for setting the battery type
    }

    mcuType theMcuType = static_cast<mcuType>(settingsCollection::read<uint8_t>(settingsCollection::settingIndex::mcuType));
    if (sx126x::isValidType(theMcuType)) {
        sx126x::initialize(theMcuType);
        logging::snprintf(logging::source::settings, "Radio    : %s (%d)\n", toString(theMcuType), static_cast<uint8_t>(theMcuType));
        sensorDeviceCollection::isPresent[static_cast<uint32_t>(sensorDeviceType::mcu)] = true;
    } else {
        logging::snprintf(logging::source::criticalError, "invalid RadioType %d\n", static_cast<uint8_t>(theMcuType));
        // settingsCollection::save(static_cast<uint8_t>(mcuType::lowPower), settingsCollection::settingIndex::mcuType);        // comment out for setting the radio type
        goTo(mainState::fatalError);
        return;
    }

    sensorDeviceCollection::discover();

    if (bme680::isPresent()) {
        sensorDeviceCollection::set(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::temperature, 0, 20);
        sensorDeviceCollection::set(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::relativeHumidity, 0, 20);
    }
    logging::snprintf("BME680   : %s\n", bme680::isPresent() ? "present" : "not present");

    if (tsl2591::isPresent()) {
        sensorDeviceCollection::set(static_cast<uint32_t>(sensorDeviceType::tsl2591), tsl2591::visibleLight, 0, 20);
    }
    logging::snprintf("TSL2591  : %s\n", tsl2591::isPresent() ? "present" : "not present");

    if (sht40::isPresent()) {
        sensorDeviceCollection::set(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::temperature, 0, 20);
        sensorDeviceCollection::set(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::relativeHumidity, 0, 20);
    }
    logging::snprintf("SHT40    : %s\n", sht40::isPresent() ? "present" : "not present");

    // TODO : initialize measurementCollection

    gpio::enableGpio(gpio::group::rfControl);

    LoRaWAN::initialize();

    // uint32_t toBeDevAddr            = 0x260B1723;
    // const char toBeNetworkKey[]     = "687F33B28C045E0412383B50D8853984";
    // const char toBeApplicationKey[] = "862DDA53CFB9A19169EDF0F4CC31480D";

    // LoRaWAN::DevAddr.asUint32 = toBeDevAddr;
    // LoRaWAN::networkKey.setFromHexString(toBeNetworkKey);
    // LoRaWAN::applicationKey.setFromHexString(toBeApplicationKey);
    // LoRaWAN::saveConfig();
    // LoRaWAN::restoreConfig();

    // Uncomment to reset MacLayer state and/or channels in the device
    // Needs a matching MacLayer reset at the LoRaWAN Network Server
    // LoRaWAN::resetState();           // TEST
    // LoRaWAN::saveState();            // TEST
    // LoRaWAN::resetChannels();        // TEST
    // LoRaWAN::saveChannels();         // TEST
    // LoRaWAN::initialize();           // TEST

    static constexpr uint32_t tmpStringLength{64};
    char tmpString[tmpStringLength];
    logging::snprintf("\n");
    hexAscii::uint32ToHexString(tmpString, LoRaWAN::DevAddr.asUint32);
    logging::snprintf("DevAddr  : %s\n", tmpString);
    hexAscii::byteArrayToHexString(tmpString, LoRaWAN::applicationKey.asBytes(), 16);
    logging::snprintf("AppSKey  : %s\n", tmpString);
    hexAscii::byteArrayToHexString(tmpString, LoRaWAN::networkKey.asBytes(), 16);
    logging::snprintf("NwkSKey  : %s\n", tmpString);

    if (!LoRaWAN::isValidConfig()) {
        logging::snprintf(logging::source::criticalError, "invalid LoRaWAN config\n");
        goTo(mainState::fatalError);
        return;
    }

    logging::snprintf("\n");
    logging::snprintf("FrmCntUp : %u\n", LoRaWAN::uplinkFrameCount.toUint32());
    logging::snprintf("FrmCntDn : %u\n", LoRaWAN::downlinkFrameCount.toUint32());
    logging::snprintf("rx1Delay : %u\n", LoRaWAN::rx1DelayInSeconds);
    logging::snprintf("DataRate : %u\n", LoRaWAN::currentDataRateIndex);
    logging::snprintf("rx1DROff : %u\n", LoRaWAN::rx1DataRateOffset);
    logging::snprintf("rx2DRIdx : %u\n", LoRaWAN::rx2DataRateIndex);

    if (!LoRaWAN::isValidState()) {
        logging::snprintf(logging::source::criticalError, "invalid LoRaWAN state\n");
        goTo(mainState::fatalError);
        return;
    }

    logging::snprintf("\n");
    logging::snprintf("ActiveCh : %u\n", loRaTxChannelCollection::nmbrActiveChannels());
    for (uint32_t loRaTxChannelIndex = 0; loRaTxChannelIndex < loRaTxChannelCollection::maxNmbrChannels; loRaTxChannelIndex++) {
        logging::snprintf("Chan[%02u] : %u Hz, %u, %u\n", loRaTxChannelIndex, loRaTxChannelCollection::channel[loRaTxChannelIndex].frequencyInHz, loRaTxChannelCollection::channel[loRaTxChannelIndex].minimumDataRateIndex, loRaTxChannelCollection::channel[loRaTxChannelIndex].maximumDataRateIndex);
    }

    applicationEventBuffer.initialize();        // clears RTCticks which may already have been generated
    goTo(mainState::networkCheck);
    // i2c::goSleep(); // TODO / BUG : this is a problem, as the I2C now sleeps during the networkCheck state... and so new settings cannot be stored in NVS
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
            logging::snprintf(logging::source::lorawanMac, "answer : %u\n", static_cast<uint8_t>(answerCounter));
            // showLoRaWanStatus();
            if (answerCounter >= minNmbrAnswers) {
                goTo(mainState::idle);
            }
            break;

        case applicationEvent::realTimeClockTick:
            requestCounter++;
            logging::snprintf(logging::source::lorawanMac, "request : %u\n", static_cast<uint8_t>(requestCounter));
            miniAdr();
            // showLoRaWanStatus();
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
                i2c::wakeUp();
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
                measurementCollection::saveNewMeasurementsToEeprom();

                uint32_t payloadLength        = measurementCollection::nmbrOfBytesToTransmit();
                const uint8_t* payLoadDataPtr = measurementCollection::getTransmitBuffer();
                LoRaWAN::sendUplink(17, payLoadDataPtr, payloadLength);
                measurementCollection::setTransmitted(0, payloadLength);        // TODO : get correct frame counter *before* call to sendUplink
                goTo(mainState::networking);
            } else {
                i2c::goSleep();
                goTo(mainState::idle);
            }
            break;

        case mainState::networking:
            if (LoRaWAN::isIdle()) {
                // showMain();
                i2c::goSleep();
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

void mainController::prepareSleep() {
    switch (state) {
        case mainState::idle:
            logging::snprintf("goSleep...\n");
            gpio::disableGpio(gpio::group::usbPresent);
            i2c::goSleep();
            gpio::disableGpio(gpio::group::uart1);
            // gpio::disableGpio(gpio::group::spiDisplay);
            // gpio::disableGpio(gpio::group::uart2);
            // gpio::disableGpio(gpio::group::rfControl);
            break;

        default:
            break;
    }
}
void mainController::goSleep() {
    switch (state) {
        case mainState::idle:
            mcuStop2();
            break;

        default:
            break;
    }
}

void mainController::wakeUp() {
    switch (state) {
        case mainState::idle:
#ifndef generic
            MX_ADC_Init();
            MX_AES_Init();
            MX_RNG_Init();
            MX_USART1_UART_Init();
#endif
            // gpio::enableGpio(gpio::group::rfControl);
            gpio::enableGpio(gpio::group::uart1);
            gpio::enableGpio(gpio::group::usbPresent);
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
            snprintf(textSmall, 8, "%0*d  %s", static_cast<int>(decimals), static_cast<int>(fractionalPart(value, decimals)), sensorDeviceCollection::units(deviceIndex[lineIndex], channelIndex[lineIndex]));
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
