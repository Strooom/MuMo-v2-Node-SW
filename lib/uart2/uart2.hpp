/// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

class uart2x {
  public:
    uart2x() = delete;
    static void initialize();
    static void goSleep();
    // TODO : add an equivalent to HAL_UART_Transmit();
};
