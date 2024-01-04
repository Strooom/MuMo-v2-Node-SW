// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

class hexAscii
{
public:
    static void hexStringToByteArray(const char *aHexString, uint8_t *aBinaryArray);                              // convert HEX-ASCII string to binary array
    static void byteArrayToHexString(const uint8_t *aBinaryArray, uint32_t aBinaryArrayLength, char *aHexString); // convert binary array to HEX-ASCII string

#ifndef unitTesting

private:
#endif
    static uint8_t toUpperCase(uint8_t aCharacter);
    static bool isHexCharacter(uint8_t aCharacter);
    static uint8_t valueFromHexCharacter(uint8_t aCharacter);
    static uint8_t hexCharacterFromValue(uint8_t aValue);
};
