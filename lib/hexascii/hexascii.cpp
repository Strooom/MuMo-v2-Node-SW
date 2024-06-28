#include <hexascii.hpp>
#include "cstring"

uint8_t hexAscii::toUpperCase(const uint8_t characterToConvert) {
    if (characterToConvert >= 'a' && characterToConvert <= 'z') {
        return characterToConvert - 'a' + 'A';
    } else {
        return characterToConvert;
    }
}

bool hexAscii::isHexCharacter(const uint8_t characterToTest) {
    return ((characterToTest >= 'A' && characterToTest <= 'F') || (characterToTest >= 'a' && characterToTest <= 'f') || (characterToTest >= '0' && characterToTest <= '9'));
}

uint8_t hexAscii::valueFromHexCharacter(const uint8_t characterToConvert) {
    if (characterToConvert >= '0' && characterToConvert <= '9') {
        return characterToConvert - '0';
    }
    if (characterToConvert >= 'A' && characterToConvert <= 'F') {
        return characterToConvert - 'A' + 10;
    }
    if (characterToConvert >= 'a' && characterToConvert <= 'f') {
        return characterToConvert - 'a' + 10;
    }
    return 0;        // in case the character is not a hex character, return 0 as value
}

uint8_t hexAscii::hexCharacterFromValue(const uint8_t valueToConvert) {
    if (valueToConvert <= 9) {
        return valueToConvert + '0';
    } else if (valueToConvert <= 15) {
        return valueToConvert - 10 + 'A';
    } else
        return '?';
}

void hexAscii::hexStringToByteArray(uint8_t *byteArrayOut, const char *hexStringIn) {
    uint32_t hexStringInLength = strlen(hexStringIn);
    uint32_t nmbrBytes         = hexStringInLength / 2;

    for (uint32_t index = 0; index < nmbrBytes; index++) {
        uint8_t leftCharacter  = hexStringIn[index * 2];
        uint8_t rightCharacter = hexStringIn[(index * 2) + 1];
        leftCharacter          = toUpperCase(leftCharacter);
        rightCharacter         = toUpperCase(rightCharacter);
        if ((isHexCharacter(leftCharacter)) && (isHexCharacter(rightCharacter))) {
            byteArrayOut[index] = (valueFromHexCharacter(leftCharacter) << 4) + valueFromHexCharacter(rightCharacter);
        }
    }
}

void hexAscii::byteArrayToHexString(char *hexStringOut, const uint8_t *byteArrayIn, const uint32_t binaryArrayInLength) {
    for (uint32_t index = 0; index < binaryArrayInLength; index++) {
        hexStringOut[index * 2]       = hexCharacterFromValue((byteArrayIn[index] & 0xF0) >> 4);
        hexStringOut[(index * 2) + 1] = hexCharacterFromValue(byteArrayIn[index] & 0x0F);
    }
    hexStringOut[binaryArrayInLength * 2] = 0x00;
}

void hexAscii::uint64ToHexString(char *hexStringOut, const uint64_t dataIn) {
    for (uint32_t index = 0; index < 16; index++) {        // 64 bits = 8 bytes = 16 nibbles = 16 hex characters
        hexStringOut[index] = hexCharacterFromValue((dataIn >> (60 - (index * 4))) & 0x0F);
    }
    hexStringOut[16] = 0x00;
}
