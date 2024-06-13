// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once

#include <stdint.h>
#include <mainstate.hpp>

class mainController {
  public:
    mainController() = delete;
    static void initialize();
    static void handleEvents();
    static void run();
    static void sleep();

    static constexpr uint32_t nmbrUplinksWaitingForNetwork{3};

#ifndef unitTesting

  private:
#endif
    static void goTo(mainState newState);
    static mainState state;

    static void showBootScreen1();
    static void showBootScreen2();
    static void showBootScreen3();

};
