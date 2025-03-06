// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once

#include <stdint.h>
#include <mainstate.hpp>
#include <screen.hpp>
#include <applicationevent.hpp>

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

    static uint32_t requestCounter;
    static uint32_t answerCounter;
};
