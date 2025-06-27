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
#include <measurementgroupcollection.hpp>
#include <power.hpp>
#include <realtimeclock.hpp>
#include <screen.hpp>
#include <sensordevicecollection.hpp>
#include <settingscollection.hpp>
#include <spi.hpp>
#include <stdio.h>
#include <tsl2591.hpp>
#include <debugport.hpp>
#include <uart2.hpp>
#include <uniqueid.hpp>
#include <buildinfo.hpp>
#include <radiotype.hpp>
#include <sx126x.hpp>
#include <stdlib.h>        // atoi

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
const uint32_t mainController::displayDeviceIndex[screen::nmbrOfMeasurementTextLines]{2, 2, 3};
const uint32_t mainController::displayChannelIndex[screen::nmbrOfMeasurementTextLines]{0, 1, 0};
char mainController::name[maxNameLength + 1]{};

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
    logging::snprintf("Creative Commons 4.0 - BY-NC-SA\n\n");

    logging::snprintf("UID      : %s\n", uniqueId::asHexString());

    uint8_t tmpName[maxNameLength + 1]{};
    settingsCollection::readByteArray(tmpName, settingsCollection::settingIndex::name);
    for (uint32_t index = 0; index < maxNameLength; ++index) {
        name[index] = tmpName[index];
    }
    name[maxNameLength] = '\0';        // ensure null termination
    logging::snprintf("name     : %s\n", name);

    spi::wakeUp();
    display::detectPresence();
    logging::snprintf(logging::source::settings, "display  : %s\n", display::isPresent() ? "present" : "not present");
    if (display::isPresent()) {
        screen::setType(screenType::logo);
        screen::setName(name);
        screen::update();
    }
    spi::goSleep();

    i2c::wakeUp();
    uint32_t nmbr64KBlocks = nonVolatileStorage::detectNmbr64KBanks();
    if (nmbr64KBlocks > 0) {
        logging::snprintf(logging::source::settings, "EEPROM   : %d * 64K present\n", nmbr64KBlocks);
    } else {
        logging::snprintf(logging::source::criticalError, "no EEPROM\n");
        state = mainState::fatalError;
        return;
    }

    if (!settingsCollection::isValid()) {
        settingsCollection::save(settingsCollection::maxMapVersion, settingsCollection::settingIndex::mapVersion);
    }
    logging::snprintf(logging::source::settings, "settingsMapVersion : %d\n", settingsCollection::getMapVersion());

    batteryType theBatteryType = static_cast<batteryType>(settingsCollection::read<uint8_t>(settingsCollection::settingIndex::batteryType));
    if (!battery::isValidType(theBatteryType)) {
        settingsCollection::save(static_cast<uint8_t>(defaultBatteryType), settingsCollection::settingIndex::batteryType);
        theBatteryType = static_cast<batteryType>(settingsCollection::read<uint8_t>(settingsCollection::settingIndex::batteryType));
    }
    battery::initialize(theBatteryType);
    logging::snprintf(logging::source::settings, "batteryType : %s (%d)\n", toString(theBatteryType), static_cast<uint8_t>(theBatteryType));

    radioType theRadioType = static_cast<radioType>(settingsCollection::read<uint8_t>(settingsCollection::settingIndex::radioType));
    if (!sx126x::isValidType(theRadioType)) {
        settingsCollection::save(static_cast<uint8_t>(defaultRadioType), settingsCollection::settingIndex::radioType);
        theRadioType = static_cast<radioType>(settingsCollection::read<uint8_t>(settingsCollection::settingIndex::radioType));
    }
    sx126x::initialize(theRadioType);
    logging::snprintf(logging::source::settings, "radioType : %s (%d)\n", toString(theRadioType), static_cast<uint8_t>(theRadioType));

    sensorDeviceCollection::discover();
    static constexpr uint32_t defaultPrescaler{20U};
    sensorDeviceCollection::set(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage, 0, defaultPrescaler);
    sensorDeviceCollection::set(static_cast<uint32_t>(sensorDeviceType::battery), battery::stateOfCharge, 0, defaultPrescaler);
    if (bme680::isPresent()) {
        sensorDeviceCollection::set(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::temperature, 0, defaultPrescaler);
        sensorDeviceCollection::set(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::relativeHumidity, 0, defaultPrescaler);
    }
    logging::snprintf("BME680   : %s\n", bme680::isPresent() ? "present" : "not present");

    if (tsl2591::isPresent()) {
        sensorDeviceCollection::set(static_cast<uint32_t>(sensorDeviceType::tsl2591), tsl2591::visibleLight, 0, defaultPrescaler);
    }
    logging::snprintf("TSL2591  : %s\n", tsl2591::isPresent() ? "present" : "not present");

    if (sht40::isPresent()) {
        sensorDeviceCollection::set(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::temperature, 0, defaultPrescaler);
        sensorDeviceCollection::set(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::relativeHumidity, 0, defaultPrescaler);
    }
    logging::snprintf("SHT40    : %s\n", sht40::isPresent() ? "present" : "not present");

    measurementGroupCollection::initialize();

    gpio::enableGpio(gpio::group::rfControl);

    LoRaWAN::initialize();
    logging::snprintf("\n");
    if (!LoRaWAN::isValidConfig()) {
        logging::snprintf(logging::source::criticalError, "invalid LoRaWAN config\n");
        LoRaWAN::initializeConfig();
    } else if (forceInitialization) {
        LoRaWAN::initializeConfig();
        logging::snprintf(logging::source::criticalError, "forced initialisation LoRaWAN config\n");
    }
    logging::snprintf("DevAddr  : %s\n", LoRaWAN::DevAddr.getAsHexString());
    logging::snprintf("AppSKey  : %s\n", LoRaWAN::applicationKey.getAsHexString());
    logging::snprintf("NwkSKey  : %s\n", LoRaWAN::networkKey.getAsHexString());

    logging::snprintf("\n");
    if (!LoRaWAN::isValidState()) {
        logging::snprintf(logging::source::criticalError, "invalid LoRaWAN state\n");
        LoRaWAN::initializeState();
    } else if (forceInitialization) {
        LoRaWAN::initializeState();
        logging::snprintf(logging::source::criticalError, "forced initialisation LoRaWAN state\n");
    }
    logging::snprintf("FrmCntUp : %u\n", LoRaWAN::uplinkFrameCount.getAsWord());
    logging::snprintf("FrmCntDn : %u\n", LoRaWAN::downlinkFrameCount.getAsWord());
    logging::snprintf("rx1Delay : %u\n", LoRaWAN::rx1DelayInSeconds);
    logging::snprintf("DataRate : %u\n", LoRaWAN::currentDataRateIndex);
    logging::snprintf("rx1DROff : %u\n", LoRaWAN::rx1DataRateOffset);
    logging::snprintf("rx2DRIdx : %u\n", LoRaWAN::rx2DataRateIndex);
    logging::snprintf("\n");
    logging::snprintf("ActiveCh : %u\n", loRaTxChannelCollection::nmbrActiveChannels());
    for (uint32_t loRaTxChannelIndex = 0; loRaTxChannelIndex < loRaTxChannelCollection::maxNmbrChannels; loRaTxChannelIndex++) {
        logging::snprintf("Chan[%02u] : %u Hz, %u, %u\n", loRaTxChannelIndex, loRaTxChannelCollection::channel[loRaTxChannelIndex].frequencyInHz, loRaTxChannelCollection::channel[loRaTxChannelIndex].minimumDataRateIndex, loRaTxChannelCollection::channel[loRaTxChannelIndex].maximumDataRateIndex);
    }

    applicationEventBuffer.initialize();
    requestCounter = 0;
    answerCounter  = 0;
    goTo(mainState::networkCheck);
}

void mainController::handleEvents() {
    while (!applicationEventBuffer.isEmpty()) {
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
            showLoRaWanStatus();
            if (answerCounter >= minNmbrAnswers) {
                goTo(mainState::idle);
            }
            break;

        case applicationEvent::realTimeClockTick:
            requestCounter++;
            logging::snprintf(logging::source::lorawanMac, "request : %u\n", static_cast<uint8_t>(requestCounter));
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
            if (sensorDeviceCollection::needsSampling()) {
                i2c::wakeUp();
                sensorDeviceCollection::startSampling();
                goTo(mainState::sampling);
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

        case mainState::sampling:
            sensorDeviceCollection::run();
            if (sensorDeviceCollection::isSamplingReady()) {
                if (sensorDeviceCollection::hasNewMeasurements()) {
                    sensorDeviceCollection::collectNewMeasurements();
                    measurementGroupCollection::addNew(sensorDeviceCollection::newMeasurements);
                    LoRaWAN::sendUplink(sensorDeviceCollection::newMeasurements);
                    goTo(mainState::networking);
                } else {
                    i2c::goSleep();
                    goTo(mainState::idle);
                }
                sensorDeviceCollection::updateCounters();
            }
            break;

        case mainState::networking:
            if (LoRaWAN::isIdle()) {
                showMain();
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

void mainController::runSleep() {
    if (!power::hasUsbPower() && !debugPort::isDebugProbePresent()) {
        prepareSleep();
        goSleep();
        wakeUp();
    }
}

void mainController::prepareSleep() {
    switch (state) {
        case mainState::idle:
            logging::snprintf("goSleep...\n");
            i2c::goSleep();
            spi::goSleep();
            // Powering down these pins has no effect on power consumption, so we keep them enabled  : gpio::disableGpio(gpio::group::usbPresent);
            // Powering down these pins has no effect on power consumption, so we keep them enabled  : gpio::disableGpio(gpio::group::rfControl);
            gpio::disableGpio(gpio::group::uart1);
            gpio::disableGpio(gpio::group::uart2);
            gpio::disableGpio(gpio::group::test0);
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
            // Powering down these pins has no effect on power consumption, so we keep them enabled  : gpio::enableGpio(gpio::group::usbPresent);
            // Powering down these pins has no effect on power consumption, so we keep them enabled  : gpio::enableGpio(gpio::group::rfControl);
            gpio::enableGpio(gpio::group::uart1);
            gpio::enableGpio(gpio::group::uart2);
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
        float value       = sensorDeviceCollection::value(displayDeviceIndex[lineIndex], displayChannelIndex[lineIndex]);
        uint32_t decimals = sensorDeviceCollection::decimals(displayDeviceIndex[lineIndex], displayChannelIndex[lineIndex]);
        char textBig[8];
        char textSmall[8];
        snprintf(textBig, 8, "%d", static_cast<int>(integerPart(value, decimals)));
        if (decimals > 0) {
            snprintf(textSmall, 8, "%0*d  %s", static_cast<int>(decimals), static_cast<int>(fractionalPart(value, decimals)), sensorDeviceCollection::units(displayDeviceIndex[lineIndex], displayChannelIndex[lineIndex]));
        } else {
            snprintf(textSmall, 8, "%s", sensorDeviceCollection::units(displayDeviceIndex[lineIndex], displayChannelIndex[lineIndex]));
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
            if (display::isPresent() && screen::isModified()) {
                spi::wakeUp();
                screen::update();
                spi::goSleep();
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
                if (cli::hasCommand()) {
                    cliCommand theCommand;
                    cli::getCommand(theCommand);
                    switch (theCommand.commandHash) {
                        case cliCommand::prompt:
                            showPrompt();
                            break;

                        case cliCommand::help:
                            showHelp();
                            break;

                        case cliCommand::enableLogging:
                            cli::sendResponse("logging enabled\n");
                            logging::enable(logging::destination::uart2);
                            break;

                        case cliCommand::disableLogging:
                            cli::sendResponse("logging disabled\n");
                            logging::disable(logging::destination::uart2);
                            break;

                        case cliCommand::enableRadio:
                            LoRaWAN::setEnableRadio(true);
                            cli::sendResponse("radio enabled\n");
                            break;

                        case cliCommand::disableRadio:
                            LoRaWAN::setEnableRadio(false);
                            cli::sendResponse("radio disabled\n");
                            break;

                        case cliCommand::getDeviceStatus:
                            showDeviceStatus();
                            break;

                        case cliCommand::getMeasurementsStatus:
                            showMeasurementsStatus();
                            break;

                        case cliCommand::getMeasurements:
                            showMeasurements();
                            break;

                        case cliCommand::getLoRaWANStatus:
                            showNetworkStatus();
                            break;

                        case cliCommand::resetMacLayer:
                            LoRaWAN::resetMacLayer();
                            cli::sendResponse("mac layer reset\n");
                            break;

                        case cliCommand::setDeviceAddress:
                            setDeviceAddress(theCommand);
                            break;

                        case cliCommand::setNetworkKey:
                            setNetworkKey(theCommand);
                            break;

                        case cliCommand::setApplicationKey:
                            setApplicationKey(theCommand);
                            break;

                        case cliCommand::setName:
                            setName(theCommand);
                            break;

                        case cliCommand::setBattery:
                            setBatteryType(theCommand);
                            break;

                        case cliCommand::setRadio:
                            setRadioType(theCommand);
                            break;

                        case cliCommand::setSensor:
                            setSensor(theCommand);
                            break;

                        case cliCommand::setDisplay:
                            cli::sendResponse("not yet implemented\n");
                            break;

                        case cliCommand::softwareReset:
                            initialize();
                            break;

                        default:
                            cli::sendResponse("unknown command : ");
                            cli::sendResponse(theCommand.commandAsString);
                            cli::sendResponse("\n");
                            break;
                    }
                }
            }
            break;
    }
}

void mainController::runUsbPowerDetection() {
    if (power::isUsbConnected()) {
        applicationEventBuffer.push(applicationEvent::usbConnected);
        uart2::initialize();
        screen::setUsbStatus(true);
    }
    if (power::isUsbRemoved()) {
        applicationEventBuffer.push(applicationEvent::usbRemoved);
        screen::setUsbStatus(false);
    }
}

void mainController::showPrompt() {
    cli::sendResponse("\n\nhttps://github.com/Strooom\n");
    cli::sendResponse("v%u.%u.%u - #%s\n", buildInfo::mainVersionDigit, buildInfo::minorVersionDigit, buildInfo::patchVersionDigit, buildInfo::lastCommitTag);
    cli::sendResponse("%s %s build - %s\n", toString(buildInfo::theBuildEnvironment), toString(buildInfo::theBuildType), buildInfo::buildTimeStamp);
    cli::sendResponse("Creative Commons 4.0 - BY-NC-SA\n");
    cli::sendResponse("Type '?' for help\n\n");
}

void mainController::showHelp() {
    cli::sendResponse("<enter> :show build info and license\n");
    cli::sendResponse("? : show help\n");
    cli::sendResponse("gds : show device status\n");
    cli::sendResponse("gms : show recorded measurements status\n");
    cli::sendResponse("gls : show LoRaWAN status\n");
    cli::sendResponse("el : enable logging\n");
    cli::sendResponse("dl : disable logging\n");
    cli::sendResponse("er : enable radio\n");
    cli::sendResponse("dr : disable radio\n");
    cli::sendResponse("rml : reset mac layer\n");
    cli::sendResponse("swr : restart device - soft reset\n");
    cli::sendResponse("sda <address> : set device address\n");
    cli::sendResponse("snk <key> : set network key\n");
    cli::sendResponse("sak <key> : set application key\n");
    cli::sendResponse("sn <newname> : set name\n");
    cli::sendResponse("sb <batteryType> : set battery type\n");
    cli::sendResponse("sr <radioType> : set radio type\n");
    cli::sendResponse("sd <line#> <deviceIndex> <channelIndex> : configure display\n");
    cli::sendResponse("ss <deviceIndex> <channelIndex> <oversampling> <prescaler> : configure sensor\n");
}

void mainController::showDeviceStatus() {
    cli::sendResponse("UID      : %s\n", uniqueId::asHexString());
    cli::sendResponse("name     : %s\n", name);
    cli::sendResponse("display  : %s\n", display::isPresent() ? "present" : "not present");
    cli::sendResponse("EEPROM   : %d * 64K present\n", nonVolatileStorage::detectNmbr64KBanks());
    cli::sendResponse("battery  : %s (%d)\n", toString(battery::getType()), static_cast<uint8_t>(battery::getType()));
    cli::sendResponse("radioType: %s (%d)\n", toString(sx126x::getType()), static_cast<uint8_t>(sx126x::getType()));

    for (uint32_t sensorDeviceIndex = 0; sensorDeviceIndex < static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices); sensorDeviceIndex++) {
        if (sensorDeviceCollection::isPresent(sensorDeviceIndex)) {
            cli::sendResponse("[%d] %s\n", sensorDeviceIndex, sensorDeviceCollection::name(sensorDeviceIndex));
            for (uint32_t channelIndex = 0; channelIndex < sensorDeviceCollection::nmbrOfChannels(sensorDeviceIndex); channelIndex++) {
                cli::sendResponse("  [%d] %s [%s]", channelIndex, sensorDeviceCollection::name(sensorDeviceIndex, channelIndex), sensorDeviceCollection::units(sensorDeviceIndex, channelIndex));
                if (sensorDeviceCollection::channel(sensorDeviceIndex, channelIndex).getPrescaler() > 0) {
                    cli::sendResponse(" oversampling = %d, prescaler = %d\n", sensorDeviceCollection::channel(sensorDeviceIndex, channelIndex).getOversampling(), sensorDeviceCollection::channel(sensorDeviceIndex, channelIndex).getPrescaler());
                } else {
                    cli::sendResponse(" disabled\n");
                }
            }
        } else {
            cli::sendResponse("[%d] %s not found\n", sensorDeviceIndex, sensorDeviceCollection::name(sensorDeviceIndex));
        }
    }
}

void mainController::showNetworkStatus() {
    cli::sendResponse("radio %s\n", LoRaWAN::isRadioEnabled() ? "enabled" : "disabled");
    cli::sendResponse("DevAddr  : %s\n", LoRaWAN::DevAddr.getAsHexString());
    char tmpKey[aesKey::lengthAsHexAscii + 1];
    memcpy(tmpKey, LoRaWAN::networkKey.getAsHexString(), aesKey::lengthAsHexAscii);
    memset(tmpKey + 2, '*', (aesKey::lengthAsHexAscii - 4));
    tmpKey[aesKey::lengthAsHexAscii] = 0;
    cli::sendResponse("NetSKey  : %s\n", tmpKey);
    memcpy(tmpKey, LoRaWAN::applicationKey.getAsHexString(), aesKey::lengthAsHexAscii);
    memset(tmpKey + 2, '*', (aesKey::lengthAsHexAscii - 4));
    tmpKey[aesKey::lengthAsHexAscii] = 0;
    cli::sendResponse("AppSKey  : %s\n", tmpKey);
    cli::sendResponse("FrmCntUp : %u\n", LoRaWAN::uplinkFrameCount.getAsWord());
    cli::sendResponse("FrmCntDn : %u\n", LoRaWAN::downlinkFrameCount.getAsWord());
    cli::sendResponse("rx1Delay : %u\n", LoRaWAN::rx1DelayInSeconds);
    cli::sendResponse("DataRate : %u\n", LoRaWAN::currentDataRateIndex);
    cli::sendResponse("rx1DROff : %u\n", LoRaWAN::rx1DataRateOffset);
    cli::sendResponse("rx2DRIdx : %u\n\n", LoRaWAN::rx2DataRateIndex);
    cli::sendResponse("ActiveCh : %u\n", loRaTxChannelCollection::nmbrActiveChannels());
    for (uint32_t loRaTxChannelIndex = 0; loRaTxChannelIndex < loRaTxChannelCollection::maxNmbrChannels; loRaTxChannelIndex++) {
        cli::sendResponse("Chan[%02u] : %u Hz, %u, %u\n", loRaTxChannelIndex, loRaTxChannelCollection::channel[loRaTxChannelIndex].frequencyInHz, loRaTxChannelCollection::channel[loRaTxChannelIndex].minimumDataRateIndex, loRaTxChannelCollection::channel[loRaTxChannelIndex].maximumDataRateIndex);
    }
}

void mainController::setDeviceAddress(const cliCommand& theCommand) {
    if (theCommand.nmbrOfArguments == 1) {
        char newDevAddrAsHex[deviceAddress::lengthAsHexAscii + 1]{0};
        size_t copyLen = strnlen(theCommand.arguments[0], deviceAddress::lengthAsHexAscii);
        if (copyLen > deviceAddress::lengthAsHexAscii) {
            copyLen = deviceAddress::lengthAsHexAscii;
        }
        memcpy(newDevAddrAsHex, theCommand.arguments[0], copyLen);
        LoRaWAN::DevAddr.setFromHexString(newDevAddrAsHex);
        LoRaWAN::saveConfig();

        cli::sendResponse("device address set : %s\n", LoRaWAN::DevAddr.getAsHexString());
    } else {
        cli::sendResponse("invalid arguments\n");
    }
}
void mainController::setNetworkKey(const cliCommand& theCommand) {
    if ((theCommand.nmbrOfArguments == 1) && (strnlen(theCommand.arguments[0], aesKey::lengthAsHexAscii) == aesKey::lengthAsHexAscii)) {
        char newKeyAsHex[aesKey::lengthAsHexAscii + 1]{0};
        memcpy(newKeyAsHex, theCommand.arguments[0], aesKey::lengthAsHexAscii);
        newKeyAsHex[aesKey::lengthAsHexAscii] = 0;
        LoRaWAN::networkKey.setFromHexString(newKeyAsHex);
        LoRaWAN::saveConfig();
        cli::sendResponse("network key set : %s\n", LoRaWAN::networkKey.getAsHexString());
    } else {
        cli::sendResponse("invalid arguments\n");
    }
}
void mainController::setApplicationKey(const cliCommand& theCommand) {
    if ((theCommand.nmbrOfArguments == 1) && (strnlen(theCommand.arguments[0], aesKey::lengthAsHexAscii) == aesKey::lengthAsHexAscii)) {
        char newKeyAsHex[aesKey::lengthAsHexAscii + 1]{0};
        memcpy(newKeyAsHex, theCommand.arguments[0], aesKey::lengthAsHexAscii);
        newKeyAsHex[aesKey::lengthAsHexAscii] = 0;
        LoRaWAN::applicationKey.setFromHexString(newKeyAsHex);
        LoRaWAN::saveConfig();
        cli::sendResponse("application key set : %s\n", LoRaWAN::applicationKey.getAsHexString());
    } else {
        cli::sendResponse("invalid arguments\n");
    }
}

void mainController::setName(const cliCommand& theCommand) {
    if (theCommand.nmbrOfArguments == 1) {
        uint8_t newName[maxNameLength + 1]{0};
        size_t copyLen = strnlen(theCommand.arguments[0], maxNameLength);
        if (copyLen > maxNameLength) {
            copyLen = maxNameLength;
        }
        memcpy(newName, theCommand.arguments[0], copyLen);
        settingsCollection::saveByteArray(newName, settingsCollection::settingIndex::name);
        settingsCollection::readByteArray(newName, settingsCollection::settingIndex::name);
        for (uint32_t index = 0; index < maxNameLength; ++index) {
            name[index] = newName[index];
        }
        name[maxNameLength] = '\0';
        screen::setName(name);
        cli::sendResponse("name set : %s\n", name);
    } else {
        cli::sendResponse("invalid arguments\n");
    }
}

void mainController::setBatteryType(const cliCommand& theCommand) {
    if (theCommand.nmbrOfArguments == 1) {
        uint32_t tmpBatteryTypeIndex = theCommand.argumentAsUint32(0);
        settingsCollection::save(static_cast<uint8_t>(tmpBatteryTypeIndex), settingsCollection::settingIndex::batteryType);
        battery::setType(static_cast<uint8_t>(tmpBatteryTypeIndex));
        cli::sendResponse("Battery set : %s (%d)\n", toString(battery::getType()), static_cast<uint8_t>(battery::getType()));

    } else {
        cli::sendResponse("invalid arguments\n");
    }
}

void mainController::setRadioType(const cliCommand& theCommand) {
    if (theCommand.nmbrOfArguments == 1) {
        uint32_t tmpRadioTypeIndex = theCommand.argumentAsUint32(0);
        settingsCollection::save(static_cast<uint8_t>(tmpRadioTypeIndex), settingsCollection::settingIndex::radioType);
        radioType theRadioType = static_cast<radioType>(settingsCollection::read<uint8_t>(settingsCollection::settingIndex::radioType));
        sx126x::initialize(theRadioType);
        cli::sendResponse("RadioType set : %s (%d)\n", toString(sx126x::getType()), static_cast<uint8_t>(sx126x::getType()));
    } else {
        cli::sendResponse("invalid arguments\n");
    }
}

void mainController::setDisplay(const cliCommand& theCommand) {
    cli::sendResponse("command not yet implemented\n");
}

void mainController::setSensor(const cliCommand& theCommand) {
    if (theCommand.nmbrOfArguments == 4) {
        uint32_t tmpDeviceIndex  = theCommand.argumentAsUint32(0);
        uint32_t tmpChannelIndex = theCommand.argumentAsUint32(1);
        uint32_t tmpOversampling = theCommand.argumentAsUint32(2);
        uint32_t tmpPrescaler    = theCommand.argumentAsUint32(3);
        sensorDeviceCollection::channel(tmpDeviceIndex, tmpChannelIndex).set(tmpOversampling, tmpPrescaler);
        cli::sendResponse("%s - %s set to oversampling = %d, prescaler = %d\n", sensorDeviceCollection::name(tmpDeviceIndex), sensorDeviceCollection::name(tmpDeviceIndex, tmpChannelIndex), sensorDeviceCollection::channel(tmpDeviceIndex, tmpChannelIndex).getOversampling(), sensorDeviceCollection::channel(tmpDeviceIndex, tmpChannelIndex).getPrescaler());
    } else {
        cli::sendResponse("invalid arguments\n");
    }
}

void mainController::showMeasurementsStatus() {
    measurementGroup tmpGroup;
    uint32_t startOffset = measurementGroupCollection::getOldestMeasurementOffset();
    uint32_t endOffset   = measurementGroupCollection::getNewMeasurementsOffset();
    if (endOffset < startOffset) {
        endOffset += nonVolatileStorage::getMeasurementsAreaSize();
    }
    uint32_t offset{startOffset};
    uint32_t nmbrOfGroups{0};
    uint32_t nmbrOfMeasurements{0};
    time_t oldestMeasurementTime;
    time_t newestMeasurementTime;

    measurementGroupCollection::get(tmpGroup, offset);
    oldestMeasurementTime = tmpGroup.getTimeStamp();

    while (offset < endOffset) {
        measurementGroupCollection::get(tmpGroup, offset);
        nmbrOfGroups++;
        nmbrOfMeasurements += tmpGroup.getNumberOfMeasurements();
        newestMeasurementTime = tmpGroup.getTimeStamp();
        offset += measurementGroup::lengthInBytes(tmpGroup.getNumberOfMeasurements());
    }
    cli::sendResponse("%u measurements in %u groups\n", nmbrOfMeasurements, nmbrOfGroups);
    cli::sendResponse("oldoffset %u %s", measurementGroupCollection::getOldestMeasurementOffset(), ctime(&oldestMeasurementTime));
    cli::sendResponse("newoffset %u %s", measurementGroupCollection::getNewMeasurementsOffset(), ctime(&newestMeasurementTime));
    cli::sendResponse("%u bytes available\n", measurementGroupCollection::getFreeSpace());
}

void mainController::showMeasurements() {
    measurementGroup tmpGroup;
    uint32_t startOffset = measurementGroupCollection::getOldestMeasurementOffset();
    uint32_t endOffset   = measurementGroupCollection::getNewMeasurementsOffset();
    if (endOffset < startOffset) {
        endOffset += nonVolatileStorage::getMeasurementsAreaSize();
    }
    uint32_t offset{startOffset};
    uint32_t nmbrOfGroups{0};
    uint32_t nmbrOfMeasurements{0};
    time_t oldestMeasurementTime;

    measurementGroupCollection::get(tmpGroup, offset);
    oldestMeasurementTime = tmpGroup.getTimeStamp();

    while (offset < endOffset) {
        measurementGroupCollection::get(tmpGroup, offset);
        cli::sendResponse("%d %s", nmbrOfGroups, ctime(&oldestMeasurementTime));        // 1 line per measurementGroup
        nmbrOfMeasurements = tmpGroup.getNumberOfMeasurements();
        for (uint32_t measurementIndex = 0; measurementIndex < nmbrOfMeasurements; measurementIndex++) {        // then per measurement part of this group
            uint32_t tmpDeviceIndex  = tmpGroup.getDeviceIndex(measurementIndex);
            uint32_t tmpChannelIndex = tmpGroup.getChannelIndex(measurementIndex);
            float tmpValue           = tmpGroup.getValue(measurementIndex);

            uint32_t decimals = sensorDeviceCollection::decimals(tmpDeviceIndex, tmpChannelIndex);
            uint32_t intPart  = integerPart(tmpValue, decimals);

            cli::sendResponse("  %s", sensorDeviceCollection::name(tmpDeviceIndex));
            cli::sendResponse(" %s", sensorDeviceCollection::name(tmpDeviceIndex, tmpChannelIndex));

            if (decimals > 0) {
                uint32_t fracPart;
                fracPart = fractionalPart(tmpValue, decimals);
                cli::sendResponse(" %d.%d", intPart, fracPart);
            } else {
                cli::sendResponse(" %d", intPart);
            }

            cli::sendResponse(" %s\n", sensorDeviceCollection::units(tmpDeviceIndex, tmpChannelIndex));
        }
        nmbrOfGroups++;
        offset += measurementGroup::lengthInBytes(tmpGroup.getNumberOfMeasurements());
    }
}