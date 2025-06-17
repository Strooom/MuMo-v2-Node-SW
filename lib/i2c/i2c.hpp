// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

class i2c {
  public:
    static void wakeUp();
    static void goSleep();
    static bool isActive() { return initalized; }

    static constexpr bool isAwake{true};
    static constexpr bool wasSleeping{false};

#ifndef unitTesting

  private:
#endif
    static bool initalized;
};
