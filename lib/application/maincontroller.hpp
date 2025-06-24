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
#include <clicommand.hpp>

static constexpr bool forceInitialization{false};
static constexpr batteryType defaultBatteryType{batteryType::alkaline_1200mAh};
static constexpr radioType defaultRadioType{radioType::lowPower};
static constexpr uint32_t maxNameLength{8U};        // maximum length of the node name, including the null terminator
static constexpr char toBeName[maxNameLength + 1] = "MuMo";

class mainController {
  public:
    mainController() = delete;
    static void initialize();
    static void runUsbPowerDetection();
    static void runStateMachine();
    static void handleEvents();
    static void runCli();
    static void runDisplayUpdate();
    static void runSleep();

    static constexpr uint32_t minNmbrAnswers{2};
    static constexpr uint32_t maxNmbrRequests{12};
    static const uint32_t deviceIndex[screen::nmbrOfMeasurementTextLines];
    static const uint32_t channelIndex[screen::nmbrOfMeasurementTextLines];

#ifndef unitTesting

    // private:
#endif
    static char name[maxNameLength + 1];

    static void goTo(mainState newState);
    static mainState state;

    static void handleEventsStateNetworkCheck(applicationEvent theEvent);
    static void handleEventsStateIdle(applicationEvent theEvent);
    static void handleEventsStateNetworking(applicationEvent theEvent);
    static void prepareSleep();
    static void goSleep();
    static void wakeUp();
    static void mcuStop2();
    static void miniAdr();

    static void showLoRaWanStatus();
    static void showMain();
    static void resetMacLayer();

    static uint32_t requestCounter;
    static uint32_t answerCounter;

    static void showPrompt();
    static void showHelp();
    static void showDeviceStatus();
    static void showMeasurementsStatus();
    static void showNetworkStatus();
    static void setDeviceAddress(cliCommand& aCommand);
    static void setNetworkKey(cliCommand& aCommand);
    static void setApplicationKey(cliCommand& aCommand);
    static void setName(cliCommand& aCommand);
    static void setBatteryType(cliCommand& aCommand);
    static void setRadioType(cliCommand& aCommand);
    static void setDisplay(cliCommand& aCommand);
    static void setSensor(cliCommand& aCommand);
};
