#include <aesblock.hpp>
#include <cstring>        // needed for memcpy()
#include <cstddef>        // std::size_t
#include <sbox.hpp>
#include <hexascii.hpp>
#include <stm32wle5_aes.hpp>

#ifndef generic
#include "main.h"
#else
#endif

void aesBlock::setFromByteArray(const uint8_t bytesIn[lengthInBytes]) {
    (void)memcpy(blockAsByteArray, bytesIn, lengthInBytes);
    syncWordsFromBytes();
}

void aesBlock::setFromWordArray(const uint32_t wordsIn[lengthInWords]) {
    for (uint32_t wordIndex = 0; wordIndex < 4; wordIndex++) {
        blockAsWordArray[wordIndex] = wordsIn[wordIndex];
    }
    syncBytesFromWords();
}

void aesBlock::setFromHexString(const char *string) {
    uint8_t tmpBytes[lengthInBytes];
    hexAscii::hexStringToByteArray(tmpBytes, string, 32U);
    (void)memcpy(blockAsByteArray, tmpBytes, lengthInBytes);
    syncWordsFromBytes();
}

void aesBlock::setFromOtherBlock(const aesBlock &block) {
    (void)memcpy(blockAsByteArray, block.blockAsByteArray, lengthInBytes);
    syncWordsFromBytes();
}

void aesBlock::setByte(const uint32_t byteIndex, uint8_t newValue) {
    blockAsByteArray[byteIndex] = newValue;
    syncWordsFromBytes();
}

void aesBlock::setWord(const uint32_t wordIndex, uint32_t newValue) {
    blockAsWordArray[wordIndex] = newValue;
    syncBytesFromWords();
}

uint32_t aesBlock::nmbrOfBlocksFromBytes(uint32_t nmbrOfBytes) {
    return (nmbrOfBytes + 15U) / 16U;
}
uint32_t aesBlock::incompleteLastBlockSizeFromBytes(uint32_t nmbrOfBytes) {
    return nmbrOfBytes % 16U;
}

bool aesBlock::hasIncompleteLastBlockFromBytes(uint32_t nmbrOfBytes) {
    return ((nmbrOfBytes % 16U) != 0U);
}

uint32_t aesBlock::calculateNmbrOfBytesToPad(uint32_t messageLength) {
    if (messageLength == 0) {
        return 16;        // exception case : for zero length message, we need to pad 16 bytes
    }
    if (hasIncompleteLastBlockFromBytes(messageLength)) {
        return (16 - incompleteLastBlockSizeFromBytes(messageLength));
    }
    return 0;
}

uint32_t aesBlock::swapLittleBigEndian(const uint32_t wordIn) {
    // ARM Cortex-M4 stores uin32_t in little endian format, but STM32WLE5 AES peripheral expects big endian format. This function swaps the bytes in a word.
    uint32_t wordOut;
    wordOut = (wordIn & 0xFF000000) >> 24 | (wordIn & 0x00FF0000) >> 8 | (wordIn & 0x0000FF00) << 8 | (wordIn & 0x000000FF) << 24;
    return wordOut;
}

// Maps a 16 byte vector to a 4x4 matrix in the way AES State expects it
void aesBlock::vectorToMatrix(uint8_t matrixOut[4][4], const uint8_t vectorIn[16]) {
    for (auto row = 0; row < 4; row++) {
        for (auto col = 0; col < 4; col++) {
            matrixOut[row][col] = vectorIn[(col * 4) + row];
        }
    }
}

// Maps a 4x4 matrix to a 16 byte vector in the way AES State expects it
void aesBlock::matrixToVector(uint8_t vectorOut[16], const uint8_t matrixIn[4][4]) {
    for (auto row = 0; row < 4; row++) {
        for (auto col = 0; col < 4; col++) {
            vectorOut[(col * 4) + row] = matrixIn[row][col];
        }
    }
}

void aesBlock::bytesToWords(uint32_t wordsOut[4], const uint8_t bytesIn[16]) {
    for (auto i = 0; i < 4; i++) {
        wordsOut[i] = bytesIn[i * 4] << 24 | bytesIn[i * 4 + 1] << 16 | bytesIn[i * 4 + 2] << 8 | bytesIn[i * 4 + 3];
    }
}

void aesBlock::wordsToBytes(uint8_t bytesOut[16], const uint32_t wordsIn[4]) {
    for (auto i = 0; i < 4; i++) {
        bytesOut[i * 4]     = (wordsIn[i] >> 24) & 0xFF;
        bytesOut[i * 4 + 1] = (wordsIn[i] >> 16) & 0xFF;
        bytesOut[i * 4 + 2] = (wordsIn[i] >> 8) & 0xFF;
        bytesOut[i * 4 + 3] = wordsIn[i] & 0xFF;
    }
}

void aesBlock::encrypt(aesKey &key) {
#ifdef HARDWARE_AES
    stm32wle5_aes::initialize(aesMode::EBC);
    stm32wle5_aes::setKey(key);
    stm32wle5_aes::enable();
    stm32wle5_aes::write(*this);
    while (!stm32wle5_aes::isComputationComplete()) {
        asm("NOP");
    }
    stm32wle5_aes::clearComputationComplete();
    stm32wle5_aes::read(*this);

#else
    XOR(key.expandedKey);
    for (auto round = 1; round <= 10; round++) {
        substituteBytes();
        shiftRows();
        if (round < 10) {
            mixColumns();
        }
        XOR(key.expandedKey + (round * 16));
    }
#endif
}

void aesBlock::substituteBytes() {
    for (uint32_t index = 0; index < lengthInBytes; index++) {
        blockAsByteArray[index] = sbox::data[blockAsByteArray[index]];
    }
    syncWordsFromBytes();
}

void aesBlock::XOR(const uint8_t *data) {
    for (uint32_t index = 0; index < lengthInBytes; index++) {
        blockAsByteArray[index] ^= data[index];
    }
    syncWordsFromBytes();
}

void aesBlock::shiftRows() {
    uint8_t temp;

    temp                       = blockAsByteArray[1];
    blockAsByteArray[1]  = blockAsByteArray[5];
    blockAsByteArray[5]  = blockAsByteArray[9];
    blockAsByteArray[9]  = blockAsByteArray[13];
    blockAsByteArray[13] = temp;

    temp                       = blockAsByteArray[2];
    blockAsByteArray[2]  = blockAsByteArray[10];
    blockAsByteArray[10] = temp;

    temp                       = blockAsByteArray[6];
    blockAsByteArray[6]  = blockAsByteArray[14];
    blockAsByteArray[14] = temp;

    temp                       = blockAsByteArray[15];
    blockAsByteArray[15] = blockAsByteArray[11];
    blockAsByteArray[11] = blockAsByteArray[7];
    blockAsByteArray[7]  = blockAsByteArray[3];
    blockAsByteArray[3]  = temp;

    syncWordsFromBytes();
}

void aesBlock::mixColumns() {
    uint8_t tempState[4][4];
    vectorToMatrix(tempState, blockAsByteArray);
    uint8_t a[4];
    uint8_t b[4];
    for (uint8_t column = 0; column < 4; column++) {
        for (uint8_t row = 0; row < 4; row++) {
            a[row] = tempState[row][column];
            b[row] = static_cast<uint8_t>(tempState[row][column] << 1U);

            if ((tempState[row][column] & 0x80) == 0x80) {
                b[row] ^= 0x1B;
            }
        }
        tempState[0][column] = b[0] ^ a[1] ^ b[1] ^ a[2] ^ a[3];
        tempState[1][column] = a[0] ^ b[1] ^ a[2] ^ b[2] ^ a[3];
        tempState[2][column] = a[0] ^ a[1] ^ b[2] ^ a[3] ^ b[3];
        tempState[3][column] = a[0] ^ b[0] ^ a[1] ^ a[2] ^ b[3];
    }
    matrixToVector(blockAsByteArray, tempState);
    syncWordsFromBytes();
}

void aesBlock::shiftLeft() {
    for (uint32_t byteIndex = 0; byteIndex < 16; byteIndex++) {
        if (byteIndex < 15) {
            if ((blockAsByteArray[byteIndex + 1] & 0x80) == 0x80) {
                blockAsByteArray[byteIndex] = static_cast<uint8_t>((blockAsByteArray[byteIndex] << 1U) + 1U);

            } else {
                blockAsByteArray[byteIndex] = static_cast<uint8_t>(blockAsByteArray[byteIndex] << 1U);
            }
        } else {
            blockAsByteArray[byteIndex] = static_cast<uint8_t>(blockAsByteArray[byteIndex] << 1U);
        }
    }
    syncWordsFromBytes();
}

void aesBlock::syncWordsFromBytes() {
    (void)memcpy(blockAsWordArray, blockAsByteArray, lengthInBytes);
}
void aesBlock::syncBytesFromWords() {
    (void)memcpy(blockAsByteArray, blockAsWordArray, lengthInBytes);
}

