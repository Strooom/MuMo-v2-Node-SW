// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <cstdint>

class gpio {
  public:
    gpio() = delete;
    enum class group : uint32_t {
        none = 0,
        rfControl,           // PP output
        i2c,                 // bidirectional
        writeProtect,        // PP output
        i2cSensors,
        i2cEeprom,
        spiDisplay,        // output + inputs
        debugPort,         // output + inputs
        uart1,             // output + inputs
        uart2,             // output + inputs
        usbPresent,        // input
        vddEnable,         // output
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