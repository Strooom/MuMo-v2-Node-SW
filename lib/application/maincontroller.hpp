// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once

#include <cstdint>
#include <mainstate.hpp>
#include <screen.hpp>

class mainController {
  public:
    mainController() = delete;
    static void initialize();
    static void handleEvents();
    static void run();
    static void sleep();
    static void mcuStop2();
    static void miniAdr();

    static constexpr uint32_t minNmbrAnswers{2};
    static constexpr uint32_t maxNmbrRequests{12};
    static uint32_t deviceIndex[screen::numberOfLines];
    static uint32_t channelIndex[screen::numberOfLines];


#ifndef unitTesting

    // private:
#endif
    static void goTo(mainState newState);
    static mainState state;

    static void showDeviceInfo();
    static void showLoRaWanConfig();
    static void showLoRaWanStatus();

    static uint32_t requestCounter;
    static uint32_t answerCounter;
};
