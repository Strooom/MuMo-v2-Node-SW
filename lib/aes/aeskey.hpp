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
    void setFromWordArray(const uint32_t wordsIn[lengthInWords]);
    void setFromHexString(const char* string);

    uint8_t getAsByte(uint32_t index) const { return keyAsBytes[index]; }
    uint32_t getAsWord(uint32_t index) const { return keyAsWords[index]; }
    const char* getAsHexString() const { return keyAsHexString; }

    static uint32_t swapLittleBigEndian(uint32_t wordIn);

    uint8_t* asBytes();         // return the key as bytes
    uint32_t* asWords();        // return the key as words

#ifndef HARDWARE_AES
    uint8_t expandedKey[176];
#endif

#ifndef unitTesting

  private:
#endif

    void syncWordsFromBytes();
    void syncHexStringFromBytes();

    struct {
        uint8_t asByte[lengthInBytes]{};
        uint32_t asUint32[lengthInWords]{};
    } key;

    // These are 3 representations of the same key data, in different formats for easy retrieval. They are synced when setting the key.
    uint8_t keyAsBytes[lengthInBytes]{};
    uint32_t keyAsWords[lengthInWords]{};
    char keyAsHexString[lengthAsHexAscii + 1]{};

#ifndef HARDWARE_AES
    void expandKey();
    void calculateRoundKey(uint8_t round);
#endif
};
