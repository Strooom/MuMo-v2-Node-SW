// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

class hexAscii {
  public:
    static void hexStringToByteArray(uint8_t *byteArrayOut, const char *hexStringIn);
    static void byteArrayToHexString(char *hexStringOut, const uint8_t *byteArrayIn, const uint32_t binaryArrayInLength);

#ifndef unitTesting

  private:
#endif
    static uint8_t toUpperCase(const uint8_t characterToConvert);
    static bool isHexCharacter(const uint8_t characterToTest);
    static uint8_t valueFromHexCharacter(const uint8_t characterToConvert);
    static uint8_t hexCharacterFromValue(const uint8_t valueToConvert);
};
