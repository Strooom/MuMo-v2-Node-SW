// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

enum class payloadEncodingVersion : uint8_t {
    unknown   = 0x00,
    mumo_v1   = 0x01,        // for backward compatibility with version 1
    mumo_v2_0 = 0x02,        // sending a list of measurements, each one contains type, timestamp and value
    mumo_v2_1 = 0x03,        // sending a list of measurements, each one contains type, timestamp, value and measurementIndex

};