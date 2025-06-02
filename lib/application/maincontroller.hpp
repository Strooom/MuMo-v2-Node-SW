// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once

#include <stdint.h>
#include <batterytype.hpp>
#include <radiotype.hpp>

#include <mainstate.hpp>
#include <screen.hpp>
#include <applicationevent.hpp>

// cliCommand cli::commands[nmbrOfCommands]{
//     {"?", "help", 0, showHelp},
//     {"ssw", "show-software", 0, nullptr},        // version, build date, build time
//     {"shw", "show-hardware", 0, nullptr},        // settings, battery-voltage
//     {"ss", "show-sensors", 0, nullptr},          // list sensors present and their channel settings
//     {"sl", "show-lorawan", 0, nullptr},
//     {"sda", "set-devaddr", 1, setDevAddr},
//     {"snk", "set-nwkkey", 1, nullptr},
//     {"sak", "set-appkey", 1, nullptr},
//     {"rml", "reset-mac-layer", 0, nullptr},
//     {"sn", "set-name", 0, nullptr},
//     {"sls", "set-logging-sources", 1, nullptr},
//     {"res", "restart", 0, nullptr}};        // will restart the software and jump to the bootloader for firmware update



static constexpr bool forceInitialization{false};
static constexpr batteryType defaultBatteryType{batteryType::alkaline_1200mAh};
static constexpr radioType defaultRadioType{radioType::lowPower};
static constexpr char toBeName[9] = "noName";

class mainController {
  public:
    mainController() = delete;
    static void initialize();
    static void handleEvents();
    static void handleEventsStateNetworkCheck(applicationEvent theEvent);
    static void handleEventsStateIdle(applicationEvent theEvent);
    static void handleEventsStateNetworking(applicationEvent theEvent);

    static void runUsbPowerDetection();
    static void runDisplayUpdate();
    static void runCli();
    static void runStateMachine();
    static void runSleep();
    static void prepareSleep();
    static void goSleep();
    static void wakeUp();

    static void mcuStop2();
    static void miniAdr();

    static constexpr uint32_t minNmbrAnswers{2};
    static constexpr uint32_t maxNmbrRequests{12};
    static const uint32_t deviceIndex[screen::nmbrOfMeasurementTextLines];
    static const uint32_t channelIndex[screen::nmbrOfMeasurementTextLines];

#ifndef unitTesting

    // private:
#endif
    static void goTo(mainState newState);
    static mainState state;

    static void showLoRaWanStatus();
    static void showMain();
    static void resetMacLayer();

    // static void setDevAddr(const char* newDevAddr);
    // static void setDevEui(const char* newDevEui);
    // static void setAppEui(const char* newAppEui);
    // static void setNwkKey(const char* newAppKey);
    // static void setAppKey(const char* newAppKey);
    // static void setBatteryType(batteryType newBatteryType);
    // static void setRadioType(radioType newRadioType);
    // static void setName(const char* newName);
    // static void setUidAsHex(const char* newUidAsHex);


    static uint32_t requestCounter;
    static uint32_t answerCounter;
};
