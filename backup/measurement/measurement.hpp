// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>

class measurement {
  public:
    uint8_t sensorDeviceId;         // defines from which sensordevice this measurement originates, eg bme680, tsl2561, etc..
    uint8_t channelAndFlags;        // channel is 4 bits of SensorChannel on the sensorDevice, flags is 4bits to indicate if the measurement has been transmitted to the cloud, and if it has been acknowledged by the cloud, etc..

    union {
        uint32_t asUint32;        // using UNIX time : seconds elapsed since 1970 Jan 01 00:00:00 UTC
        uint8_t asBytes[4];           //
    } timestamp;
    union {
        float asFloat;                // value can be of any data type, but I assume it always fits in 4 bytes..
        uint32_t asUint32;        //
        uint8_t asBytes[4];           //
    } value;

    static constexpr uint32_t length{10};                                 // total length of a measurement in bytes - NOTE : in RAM the compiler aligns things, so it occupies 16 bytes...
    static constexpr uint8_t transmittedToCloudMask{0b0000'0001};         // bit 0
    static constexpr uint8_t acknowledgedByCloudMask{0b0000'0010};        // bit 1
    static constexpr uint8_t priorityMask{0b0000'1100};                   // bit 2..3

#ifndef unitTesting

  private:
#endif
    static uint32_t getTimeStamp();
};
