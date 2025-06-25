// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

// ####################################################################################################################################
// Important note :
// After flashing a device via the debugPort (SWD), a powr-cycle is needed to deactivate the debugPort and achieve the lowest power.
// A simple reset (via reset button) is not sufficient.
// ###################################################################################################################################

#pragma once
#include <stdint.h>
#include <stdarg.h>
#ifndef generic
#include "main.h"
#else
#endif

class debugPort {
  public:
    debugPort() = delete;
    static bool isDebugProbePresent() {
#ifndef generic
        return ((CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) == 0x0001);
#else
        return false;
#endif
    }
    static void transmit(const uint8_t* buffer, const uint32_t dataLength) {
        for (uint32_t index = 0; index < dataLength; index++) {
#ifndef generic
            ITM_SendChar(static_cast<uint32_t>(buffer[index]));
#endif
        }
    }
};
