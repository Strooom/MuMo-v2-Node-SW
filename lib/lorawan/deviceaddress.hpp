// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

class deviceAddress {
  public:
    static constexpr uint32_t lengthInBytes{4};
    explicit deviceAddress();
    explicit deviceAddress(uint32_t theDeviceAddress);

    deviceAddress& operator=(const uint32_t theDeviceAddress);
    deviceAddress& operator=(const deviceAddress& theDeviceAddress);
    bool operator==(const deviceAddress& theDeviceAddress);
    bool operator!=(const deviceAddress& theDeviceAddress);

    union {
        uint32_t asUint32{0xFFFFFFFF};
        uint8_t asUint8[lengthInBytes];
    };
};
