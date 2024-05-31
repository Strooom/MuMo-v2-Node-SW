// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <ctime>

class measurement {
  public:
    uint8_t id;        // bit7 : has timestamp, bit6..2 : sensorDeviceId, bit1..0 : channel
    static constexpr uint8_t hasTimestampMask = 0x80;
    static constexpr uint8_t sensorDeviceIdMask = 0x7C;
    static constexpr uint8_t channelMask = 0x03;

    union {
        uint32_t asUint32;        // using UNIX time : seconds elapsed since 1970 Jan 01 00:00:00 UTC
        uint8_t asBytes[4];
    } timestamp;

    union {
        float asFloat;                // value can be of any data type, but I assume it always fits in 4 bytes..
        uint32_t asUint32;
        uint8_t asBytes[4];
    } value;

#ifndef unitTesting

  private:
#endif
};
