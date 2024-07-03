// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

int32_t integerPart(const float value, const uint32_t decimals);
uint32_t fractionalPart(const float value, const uint32_t decimals);
float factorFloat(const uint32_t decimals);
uint32_t factorInt(const uint32_t decimals);
uint8_t* floatToBytes(const float value);
float bytesToFloat(const uint8_t bytes[4]);
