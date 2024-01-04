// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include <cstddef>        // std::size_t
#include "aeskey.h"

class aesBlock {
  public:
    static constexpr uint32_t lengthAsBytes{16};         //
    static constexpr uint32_t lengthAsWords{4};          //
    void set(const uint8_t bytes[lengthAsBytes]);        // load the block with data
    uint8_t &operator[](std::size_t index); // accessing the individual bytes through the [] operator

    void encrypt(aesKey &key);        // encrypt the block
    const uint8_t *asBytes();         // return the encrypted block as bytes
    const uint32_t *asWords();        // return the encrypted block as words
    static void matrixToVector(uint8_t matrixIn[4][4], uint8_t vectorOut[16]);
    static void vectorToMatrix(uint8_t vectorIn[16], uint8_t matrixOut[4][4]);
    static void bytesToWords(uint8_t bytesIn[16], uint32_t wordsOut[4]);
    static void wordsToBytes(uint32_t wordsIn[4], uint8_t bytesOut[16]);

#ifndef unitTesting

  private:
#endif
    void substituteBytes();
    void shiftRows();
    void XOR(const uint8_t *data);
    void mixColumns();

    union {
        uint8_t asByte[lengthAsBytes]{};        // interprete the data as 16 bytes
        uint32_t asWord[lengthAsWords];         // interprete the data as 4 32bit words
    } state;                                    // fancy name for data in the block

    friend class aesKey;
};