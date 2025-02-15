// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

class gpio {
  public:
    gpio() = delete;
    static void initialize();
    static bool isDebugProbePresent();
    enum class group : uint32_t {
        none = 0,
        rfControl,                       // PP output
        i2c,                             // bidirectional
        writeProtect,                    // PP output
        spiDisplay,                      // output + inputs
        debugPort,                       // output + inputs
        uart1,                           // output + inputs
        uart2,                           // output + inputs
        usbPresent,                      // input
        enableDisplayPower,              // output
        enableSensorsEepromPower,        // output
        other
    };

    static void enableGpio(group aGroup);
    static void disableGpio(group aGroup);
    static void disableAllGpio();

#ifndef unitTesting

  private:
#endif
    static void enableDisableGpio(group theGroup, bool enable);
};