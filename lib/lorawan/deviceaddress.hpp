// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

class deviceAddress {
  public:
    static constexpr uint32_t lengthInBytes{4};
    static constexpr uint32_t lengthAsHexString{8};
    explicit deviceAddress();
    explicit deviceAddress(uint32_t theDeviceAddress);

    deviceAddress& operator=(const uint32_t theDeviceAddress);
    deviceAddress& operator=(const deviceAddress& theDeviceAddress);
    bool operator==(const deviceAddress& theDeviceAddress) const;
    bool operator!=(const deviceAddress& theDeviceAddress) const;

    // TODO need to get rid of this punning via union
    union {
        uint32_t asUint32{0xFFFFFFFF};
        uint8_t asUint8[lengthInBytes];
    };
    const char* asHexString();
    // uint8_t asUint8(uint32_t index);

  private:
    char asHexString_[lengthAsHexString + 1];
    uint8_t asUint8_[lengthInBytes];
};
