// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#pragma once
#include <stdint.h>

uint32_t swapLittleBigEndian(uint32_t wordIn);
uint32_t bytesToBigEndianWord(uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3);
uint8_t bigEndianWordToByte0(uint32_t wordIn);
uint8_t bigEndianWordToByte1(uint32_t wordIn);
uint8_t bigEndianWordToByte2(uint32_t wordIn);
uint8_t bigEndianWordToByte3(uint32_t wordIn);