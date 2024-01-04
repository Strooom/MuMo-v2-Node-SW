#include <hexascii.hpp>
#include "string.h"

uint8_t hexAscii::toUpperCase(uint8_t aCharacter)
{
    if (aCharacter >= 'a' && aCharacter <= 'z')
    {
        aCharacter = aCharacter - 'a' + 'A';
    }
    return aCharacter;
}

bool hexAscii::isHexCharacter(uint8_t aCharacter)
{
    return ((aCharacter >= 'A' && aCharacter <= 'F') || (aCharacter >= 'a' && aCharacter <= 'f') || (aCharacter >= '0' && aCharacter <= '9'));
}

uint8_t hexAscii::valueFromHexCharacter(uint8_t aCharacter)
{
    if (aCharacter >= '0' && aCharacter <= '9')
    {
        return aCharacter - '0';
    }
    if (aCharacter >= 'A' && aCharacter <= 'F')
    {
        return aCharacter - 'A' + 10;
    }
    if (aCharacter >= 'a' && aCharacter <= 'f')
    {
        return aCharacter - 'a' + 10;
    }
    return 0; // in case the character is not a hex character, return 0 as value
}

uint8_t hexAscii::hexCharacterFromValue(uint8_t aValue)
{
    if (aValue <= 9)
    {
        return aValue + '0';
    }
    else if (aValue <= 15)
    {
        return aValue - 10 + 'A';
    }
    else
        return '?';
}

void hexAscii::hexStringToByteArray(const char *aHexString, uint8_t *aBinaryArray)
{
    uint32_t aHexStringLength = strlen(aHexString);
    uint32_t nmbrBytes = aHexStringLength / 2;

    for (uint32_t index = 0; index < nmbrBytes; index++)
    {
        uint8_t leftCharacter = aHexString[index * 2];
        uint8_t rightCharacter = aHexString[(index * 2) + 1];
        leftCharacter = toUpperCase(leftCharacter);
        rightCharacter = toUpperCase(rightCharacter);
        if ((isHexCharacter(leftCharacter)) && (isHexCharacter(rightCharacter)))
        {
            aBinaryArray[index] = (valueFromHexCharacter(leftCharacter) << 4) + valueFromHexCharacter(rightCharacter);
        }
    }
}

void hexAscii::byteArrayToHexString(const uint8_t *aBinaryArray, uint32_t aBinaryArrayLength, char *aHexString)
{
    for (uint32_t index = 0; index < aBinaryArrayLength; index++)
    {
        aHexString[index * 2] = hexCharacterFromValue((aBinaryArray[index] & 0xF0) >> 4);
        aHexString[(index * 2) + 1] = hexCharacterFromValue(aBinaryArray[index] & 0x0F);
    }
    aHexString[aBinaryArrayLength * 2] = 0x00; // terminate the string
}
