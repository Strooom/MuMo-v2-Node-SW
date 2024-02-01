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
    static void hexStringToByteArray(const char *aHexString, uint8_t *aBinaryArray);                                     // convert HEX-ASCII string to binary array
    static void byteArrayToHexString(const uint8_t *aBinaryArray, uint32_t aBinaryArrayLength, char *aHexString);        // convert binary array to HEX-ASCII string

    static void hexStringToByteArray2(uint8_t *byteArrayOut, const char *hexStringIn);
    static void byteArrayToHexString2(char *hexStringOut, const uint8_t *byteArrayIn, const uint32_t binaryArrayInLength);

#ifndef unitTesting

  private:
#endif
    static uint8_t toUpperCase(const uint8_t characterToConvert);
    static bool isHexCharacter(const uint8_t characterToTest);
    static uint8_t valueFromHexCharacter(const uint8_t characterToConvert);
    static uint8_t hexCharacterFromValue(const uint8_t valueToConvert);
};
