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
    static constexpr uint32_t lengthAsBytes{16};        //
    static constexpr uint32_t lengthAsWords{4};         //

    void set(const uint8_t bytes[lengthAsBytes]);        // load the key with an array of bytes and expands it

    const uint8_t* asBytes();         // return the key as bytes
    const uint32_t* asWords();        // return the key as words

#ifndef unitTesting

  private:
#endif
    uint8_t expandedKey[176];

    union {
        uint8_t asByte[lengthAsBytes]{};        // interprete the data as 16 bytes
        uint32_t asWord[lengthAsWords];         // interprete the data as 4 32bit words
    } key;

    void expandKey();
    void calculateRoundKey(uint8_t round);

    friend class aesBlock;
};


