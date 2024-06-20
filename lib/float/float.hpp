// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <cstdint>

int32_t integerPart(float value, uint32_t decimals);
uint32_t fractionalPart(float value, uint32_t decimals);
float factorFloat(uint32_t decimals);
uint32_t factorInt(uint32_t decimals);
uint8_t* floatToBytes(float value);
float bytesToFloat(uint8_t bytes[4]);
