// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include "sensortype.h"

class measurement {
  public:
    sensorChannelType type;                  // defines what data the measurement contains, eg temperature, humidity, pressure, etc
    union {
        uint32_t asDoubleWord;        // using UNIX time : seconds elapsed since 1970 Jan 01 00:00:00 UTC
        uint8_t asBytes[4];           //
    } timestamp;
    union {
        float asFloat;                                                // value can be of any data type, but I assume it always fits in 4 bytes..
        uint32_t asDoubleWord;                                        //
        uint8_t asBytes[4];                                           //
    } value;
    static constexpr uint32_t length{10};                             // total length of a measurement in bytes - NOTE : in RAM the compiler aligns things, so it occupies 16 bytes...
    static constexpr uint8_t transmittedToCloud{0b0000'0001};         // bit 0
    static constexpr uint8_t acknowledgedByCloud{0b0000'0010};        // bit 1
    static constexpr uint8_t priority{0b1100'0000};                   // bit 7..6
    uint8_t flags;                                                    // flags to indicate if the measurement has been transmitted to the cloud, and if it has been acknowledged by the cloud, etc..

#ifndef unitTesting

  private:
#endif
    static uint32_t getTimeStamp();
};
