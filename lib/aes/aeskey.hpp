// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
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

#ifndef unitTesting

  private:
#endif
    uint8_t expandedKey[176];

    union {
        uint8_t asByte[lengthInBytes]{};        // interprete the data as 16 bytes
        uint32_t asWord[lengthInWords];         // interprete the data as 4 32bit words
    } key;

    void expandKey();
    void calculateRoundKey(uint8_t round);
};
