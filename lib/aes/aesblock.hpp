// #############################################################################
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
    aesBlock &operator=(const aesBlock &block);

    uint8_t &operator[](std::size_t index);        // accessing the individual bytes through the [] operator
    bool operator==(const aesBlock &block) const;

    static uint32_t nmbrOfBlocksFromBytes(uint32_t nmbrOfBytes);
    static uint32_t incompleteLastBlockSizeFromBytes(uint32_t nmbrOfBytes);
    static bool hasIncompleteLastBlockFromBytes(uint32_t nmbrOfBytes);
    static uint32_t calculateNmbrOfBytesToPad(uint32_t nmbrOfBytes);

    void encrypt(aesKey &withKey);

    uint8_t *asBytes();
    uint32_t *asWords();

    static void matrixToVector(uint8_t vectorOut[16], const uint8_t matrixIn[4][4]);
    static void vectorToMatrix(uint8_t matrixOut[4][4], const uint8_t vectorIn[16]);
    static void bytesToWords(uint32_t wordsOut[4], const uint8_t bytesIn[16]);
    static void wordsToBytes(uint8_t bytesOut[16], const uint32_t wordsIn[4]);

    static uint32_t swapLittleBigEndian(const uint32_t wordIn);
    void XOR(const uint8_t *withData);
    void shiftLeft();

#ifndef unitTesting

  private:
#endif
    void substituteBytes();
    void shiftRows();
    void mixColumns();

    union {
        uint8_t asByte[lengthInBytes]{};         // interprete the data as 16 bytes
        uint32_t asUint32[lengthInWords];        // interprete the data as 4 32bit words
    } state;                                     // fancy name for data in the block
};