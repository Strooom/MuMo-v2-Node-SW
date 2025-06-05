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

    void setFromByteArray(const uint8_t bytes[lengthInBytes]);
    void setFromWordArray(const uint32_t wordsIn[lengthInWords]);
    void setFromHexString(const char* string);
    static uint32_t swapLittleBigEndian(uint32_t wordIn);

    uint8_t* asBytes();         // return the key as bytes
    uint32_t* asWords();        // return the key as words

    uint8_t expandedKey[176];        // TODO only needed when #ifndef HARDWARE_AES
#ifndef unitTesting

  private:
#endif

    union {
        uint8_t asByte[lengthInBytes]{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};        // interprete the data as 16 bytes
        uint32_t asUint32[lengthInWords];                                                                                                     // interprete the data as 4 32bit words
    } key;

    void expandKey();                             // TODO only needed when #ifndef HARDWARE_AES
    void calculateRoundKey(uint8_t round);        // TODO only needed when #ifndef HARDWARE_AES
};
