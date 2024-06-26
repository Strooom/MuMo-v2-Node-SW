// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include <aeskey.hpp>
#include <aesblock.hpp>

class messageIntegrityCode {
  public:
    static constexpr uint32_t length{4};                                       // [bytes]
    uint32_t calculate(aesKey aKey);                                           // calculates the MIC over the buffer, using the key
    bool validate(const uint8_t* buffer, uint32_t length, aesKey aKey);        // validates the MIC over the buffer, using the key

    union {
        uint32_t asUint32{0};
        uint8_t asUint8[length];
    };
};
