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

static constexpr uint32_t maxNameLength{8U};        // maximum length of the node name, including the null terminator
static constexpr char toBeName[maxNameLength + 1] = "MuMo";

class mainController {
  public:
    mainController() = delete;
    static void initialize();
    static void initializeLogging();
    static void initializeNonVolatileStorage();
    static void initializeName();
    static void initializeBattery();
    static void initializeDisplay();
    static void initializeRadio();
    static void initializeSensors();
    static void runUsbPowerDetection();
    static void runStateMachine();
    static void handleEvents();
    static void runCli();
    static void runDisplayUpdate();
    static void runSleep();

    static constexpr uint32_t minNmbrAnswers{2};
    static constexpr uint32_t maxNmbrRequests{12};

#ifndef unitTesting

// private:
#endif
    static char name[maxNameLength + 1];
    static uint32_t displayDeviceIndex[screen::nmbrOfMeasurementTextLines];
    static uint32_t displayChannelIndex[screen::nmbrOfMeasurementTextLines];

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
    static void showMeasurements();
    static void showMeasurementsCsv();
    static void showNetworkStatus();
    static void setDeviceAddress(const cliCommand& aCommand);
    static void setNetworkKey(const cliCommand& aCommand);
    static void setApplicationKey(const cliCommand& aCommand);
    static void setName(const cliCommand& aCommand);
    static void setBatteryType(const cliCommand& aCommand);
    static void setRadioType(const cliCommand& aCommand);
    static void setDisplay(const cliCommand& aCommand);
    static void setSensor(const cliCommand& aCommand);

    static void setSensor(const uint8_t* payload, const uint32_t payloadLength);
    static void setDisplay(const uint8_t* payload, const uint32_t payloadLength);
    static void handleDownLink(const uint8_t port, const uint8_t* payload, const uint32_t payloadLength);
};
