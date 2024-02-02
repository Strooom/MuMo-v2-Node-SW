// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include <cstddef>        // std::size_t
#include <aeskey.hpp>

class aesBlock {
  public:
    static constexpr uint32_t lengthInBytes{16};
    static constexpr uint32_t lengthInWords{4};
    
    void setFromByteArray(const uint8_t bytesIn[lengthInBytes]);
    void setFromWordArray(const uint32_t wordsIn[lengthInWords]);
    void setFromHexString(const char *string);
    aesBlock& operator= (const aesBlock& block);
    
    uint8_t &operator[](std::size_t index);        // accessing the individual bytes through the [] operator
    bool operator == (const aesBlock &block);

    static uint32_t nmbrOfBlocks(uint32_t nmbrOfBytes);
    static uint32_t incompleteLastBlockSize(uint32_t nmbrOfBytes);

    void encrypt(aesKey &withKey);

    uint8_t *asBytes();
    uint32_t *asWords();
    static void matrixToVector(uint8_t matrixIn[4][4], uint8_t vectorOut[16]);
    static void vectorToMatrix(uint8_t vectorIn[16], uint8_t matrixOut[4][4]);
    static void bytesToWords(uint8_t bytesIn[16], uint32_t wordsOut[4]);
    static void wordsToBytes(uint32_t wordsIn[4], uint8_t bytesOut[16]);
    static uint32_t swapLittleBigEndian(uint32_t wordIn);
    void XOR(const uint8_t *withData);
    void shiftLeft();

#ifndef unitTesting

  private:
#endif
    void substituteBytes();
    void shiftRows();
    void mixColumns();

    union {
        uint8_t asByte[lengthInBytes]{};        // interprete the data as 16 bytes
        uint32_t asWord[lengthInWords];         // interprete the data as 4 32bit words
    } state;                                    // fancy name for data in the block
};