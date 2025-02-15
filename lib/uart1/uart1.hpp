/// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

class uart1 {
  public:
    uart1() = delete;
    static void initialize();
    static void goSleep();
    static void transmit(const char *data, uint32_t dataLength);
};
