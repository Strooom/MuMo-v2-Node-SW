#include <aesblock.hpp>
#include <cstring>
#include <sbox.hpp>
#include <hexascii.hpp>
#include <stm32wle5_aes.hpp>

#ifndef generic
#include "main.h"
#else
#endif

void aesBlock::setFromByteArray(const uint8_t bytesIn[lengthInBytes]) {
    (void)memcpy(blockData, bytesIn, lengthInBytes);
}

void aesBlock::setFromHexString(const char *string) {
    uint8_t tmpBytes[lengthInBytes];
    hexAscii::hexStringToByteArray(tmpBytes, string, 32U);
    (void)memcpy(blockData, tmpBytes, lengthInBytes);
}

void aesBlock::setByte(const uint32_t byteIndex, uint8_t newValue) {
    blockData[byteIndex] = newValue;
}

uint32_t aesBlock::getAsWord(uint32_t index) const {
    uint32_t result;
    (void)memcpy(&result, &blockData[index * 4], 4);
    return result;
}

void aesBlock::setWord(const uint32_t wordIndex, uint32_t newValue) {
    (void)memcpy(&blockData[wordIndex * 4], &newValue, 4);
}

uint32_t aesBlock::nmbrOfBlocksFromBytes(uint32_t nmbrOfBytes) {
    return (nmbrOfBytes + 15U) / lengthInBytes;
}
uint32_t aesBlock::incompleteLastBlockSizeFromBytes(uint32_t nmbrOfBytes) {
    return nmbrOfBytes % lengthInBytes;
}

bool aesBlock::hasIncompleteLastBlockFromBytes(uint32_t nmbrOfBytes) {
    return ((nmbrOfBytes % lengthInBytes) != 0U);
}

uint32_t aesBlock::calculateNmbrOfBytesToPad(uint32_t messageLength) {
    if (messageLength == 0) {
        return lengthInBytes;        // exception case : for zero length message, we need to pad 16 bytes
    }
    if (hasIncompleteLastBlockFromBytes(messageLength)) {
        return (lengthInBytes - incompleteLastBlockSizeFromBytes(messageLength));
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

void aesBlock::encrypt(const aesKey &key) {
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
        blockData[index] = sbox::data[blockData[index]];
    }
}

void aesBlock::XOR(const uint8_t *data) {
    for (uint32_t index = 0; index < lengthInBytes; index++) {
        blockData[index] ^= data[index];
    }
}

void aesBlock::shiftRows() {
    uint8_t temp;

    temp                 = blockData[1];
    blockData[1]  = blockData[5];
    blockData[5]  = blockData[9];
    blockData[9]  = blockData[13];
    blockData[13] = temp;

    temp                 = blockData[2];
    blockData[2]  = blockData[10];
    blockData[10] = temp;

    temp                 = blockData[6];
    blockData[6]  = blockData[14];
    blockData[14] = temp;

    temp                 = blockData[15];
    blockData[15] = blockData[11];
    blockData[11] = blockData[7];
    blockData[7]  = blockData[3];
    blockData[3]  = temp;
}

void aesBlock::mixColumns() {
    uint8_t tempState[4][4];
    vectorToMatrix(tempState, blockData);
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
    matrixToVector(blockData, tempState);
}

void aesBlock::shiftLeft() {
    for (uint32_t byteIndex = 0; byteIndex < lengthInBytes; byteIndex++) {
        if (byteIndex < 15) {
            if ((blockData[byteIndex + 1] & 0x80) == 0x80) {
                blockData[byteIndex] = static_cast<uint8_t>((blockData[byteIndex] << 1U) + 1U);

            } else {
                blockData[byteIndex] = static_cast<uint8_t>(blockData[byteIndex] << 1U);
            }
        } else {
            blockData[byteIndex] = static_cast<uint8_t>(blockData[byteIndex] << 1U);
        }
    }

}