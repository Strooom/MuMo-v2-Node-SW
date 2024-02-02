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
    memcpy(state.asByte, bytesIn, lengthInBytes);
}

void aesBlock::setFromWordArray(const uint32_t wordsIn[lengthInWords]) {
    for (uint32_t wordIndex = 0; wordIndex < 4; wordIndex++) {
        state.asWord[wordIndex] = wordsIn[wordIndex];
    }
}

void aesBlock::setFromHexString(const char *string) {
    uint8_t tmpBytes[lengthInBytes];
    hexAscii::hexStringToByteArray(tmpBytes, string);
    memcpy(state.asByte, tmpBytes, lengthInBytes);
}

uint8_t &aesBlock::operator[](std::size_t index) {
    return state.asByte[index];
}

bool aesBlock::operator==(const aesBlock &block) {
    return (memcmp(state.asByte, block.state.asByte, lengthInBytes) == 0);
}

aesBlock &aesBlock::operator=(const aesBlock &block) {
    memcpy(state.asByte, block.state.asByte, lengthInBytes);
    return *this;
}

uint8_t *aesBlock::asBytes() {
    return state.asByte;
}

uint32_t *aesBlock::asWords() {
    return state.asWord;
}

uint32_t aesBlock::nmbrOfBlocks(uint32_t nmbrOfBytes) {
    return (nmbrOfBytes + 15U) / 16U;
}
uint32_t aesBlock::incompleteLastBlockSize(uint32_t nmbrOfBytes) {
    return nmbrOfBytes % 16U;
}

uint32_t aesBlock::swapLittleBigEndian(uint32_t wordIn) {
    // ARM Cortex-M4 stores uin32_t in little endian format, but STM32WLE5 AES peripheral expects big endian format. This function swaps the bytes in a word.
    uint32_t wordOut;
    wordOut = (wordIn & 0xFF000000) >> 24 | (wordIn & 0x00FF0000) >> 8 | (wordIn & 0x0000FF00) << 8 | (wordIn & 0x000000FF) << 24;
    return wordOut;
}

// Maps a 16 byte vector to a 4x4 matrix in the way AES State expects it
void aesBlock::vectorToMatrix(uint8_t matrixOut[4][4], uint8_t vectorIn[16]) {
    for (auto row = 0; row < 4; row++) {
        for (auto col = 0; col < 4; col++) {
            matrixOut[row][col] = vectorIn[(col * 4) + row];
        }
    }
}

// Maps a 4x4 matrix to a 16 byte vector in the way AES State expects it
void aesBlock::matrixToVector(uint8_t vectorOut[16], uint8_t matrixIn[4][4]) {
    for (auto row = 0; row < 4; row++) {
        for (auto col = 0; col < 4; col++) {
            vectorOut[(col * 4) + row] = matrixIn[row][col];
        }
    }
}

void aesBlock::bytesToWords(uint32_t wordsOut[4], uint8_t bytesIn[16]) {
    for (auto i = 0; i < 4; i++) {
        wordsOut[i] = bytesIn[i * 4] << 24 | bytesIn[i * 4 + 1] << 16 | bytesIn[i * 4 + 2] << 8 | bytesIn[i * 4 + 3];
    }
}

void aesBlock::wordsToBytes(uint8_t bytesOut[16], uint32_t wordsIn[4]) {
    for (auto i = 0; i < 4; i++) {
        bytesOut[i * 4]     = (wordsIn[i] >> 24) & 0xFF;
        bytesOut[i * 4 + 1] = (wordsIn[i] >> 16) & 0xFF;
        bytesOut[i * 4 + 2] = (wordsIn[i] >> 8) & 0xFF;
        bytesOut[i * 4 + 3] = wordsIn[i] & 0xFF;
    }
}



void aesBlock::encrypt(aesKey &key) {
#ifndef generic
    stm32wle5_aes::initialize(aesMode::EBC);
    stm32wle5_aes::setKey(key);
    stm32wle5_aes::enable();
    stm32wle5_aes::write(*this);
    while (!stm32wle5_aes::isComputationComplete()) {
    }
    stm32wle5_aes::clearComputationComplete();
    stm32wle5_aes::read(*this);

#else
    // software implementation
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
        state.asByte[index] = sbox::data[state.asByte[index]];
    }
}

void aesBlock::XOR(const uint8_t *data) {
    for (uint32_t index = 0; index < lengthInBytes; index++) {
        state.asByte[index] ^= data[index];
    }
}

void aesBlock::shiftRows() {
    uint8_t temp;

    temp             = state.asByte[1];
    state.asByte[1]  = state.asByte[5];
    state.asByte[5]  = state.asByte[9];
    state.asByte[9]  = state.asByte[13];
    state.asByte[13] = temp;

    temp             = state.asByte[2];
    state.asByte[2]  = state.asByte[10];
    state.asByte[10] = temp;

    temp             = state.asByte[6];
    state.asByte[6]  = state.asByte[14];
    state.asByte[14] = temp;

    temp             = state.asByte[15];
    state.asByte[15] = state.asByte[11];
    state.asByte[11] = state.asByte[7];
    state.asByte[7]  = state.asByte[3];
    state.asByte[3]  = temp;
}

void aesBlock::mixColumns() {
    uint8_t tempState[4][4];
    vectorToMatrix(tempState, state.asByte);
    uint8_t row, column;
    uint8_t a[4], b[4];
    for (column = 0; column < 4; column++) {
        for (row = 0; row < 4; row++) {
            a[row] = tempState[row][column];
            b[row] = (tempState[row][column] << 1);

            if ((tempState[row][column] & 0x80) == 0x80) {
                b[row] ^= 0x1B;
            }
        }
        tempState[0][column] = b[0] ^ a[1] ^ b[1] ^ a[2] ^ a[3];
        tempState[1][column] = a[0] ^ b[1] ^ a[2] ^ b[2] ^ a[3];
        tempState[2][column] = a[0] ^ a[1] ^ b[2] ^ a[3] ^ b[3];
        tempState[3][column] = a[0] ^ b[0] ^ a[1] ^ a[2] ^ b[3];
    }
    matrixToVector(state.asByte, tempState);
}


void aesBlock::shiftLeft() {
    // TODO : check if we can make this more efficient in 32bit operations
    unsigned char i;
    unsigned char Overflow = 0;

    uint8_t Data[16];
    memcpy(Data, state.asByte, 16);

    for (i = 0; i < 16; i++) {
        // Check for overflow on next byte except for the last byte
        if (i < 15) {
            // Check if upper bit is one
            if ((Data[i + 1] & 0x80) == 0x80) {
                Overflow = 1;
            } else {
                Overflow = 0;
            }
        } else {
            Overflow = 0;
        }

        // Shift one left
        Data[i] = (Data[i] << 1) + Overflow;
    }
    memcpy(state.asByte, Data, 16);
}
