// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

class aesKey {
  public:
    static constexpr uint32_t lengthInBytes{16};
    static constexpr uint32_t lengthInWords{4};
    static constexpr uint32_t lengthAsHexAscii{32};

    void setFromByteArray(const uint8_t bytes[lengthInBytes]);
    void setFromHexString(const char* string);

    uint8_t getAsByte(uint32_t index) const { return keyAsBytes[index]; }
    uint32_t getAsWord(uint32_t index) const;
    const char* getAsHexString() const { return keyAsHexString; };

    static uint32_t swapLittleBigEndian(uint32_t wordIn);

#ifndef HARDWARE_AES
    uint8_t expandedKey[176];
#endif

#ifndef unitTesting

  private:
#endif
    // These are 2 representations of the same key data, in different formats for easy retrieval. They are synced when setting the key.
    // representation as hexAscii aligns with how The Things Network shows the AES keys, so copy paste from there is possible.
    // representation as bytes : first to hexAscii characters map to first byte of the array

    uint8_t keyAsBytes[lengthInBytes]{};
    char keyAsHexString[lengthAsHexAscii + 1]{};
    void syncHexStringFromBytes();

#ifndef HARDWARE_AES
    void expandKey();
    void calculateRoundKey(uint8_t round);
#endif
};
