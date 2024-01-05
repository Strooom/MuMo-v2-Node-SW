#include "aesblock.h"
#include <cstring>        // needed for memcpy()
#include <cstddef>        // std::size_t
#include "aeskey.h"
#include "sbox.h"

#ifndef generic
#include "main.h"
extern CRYP_HandleTypeDef hcryp;
#else
#endif

void aesBlock::set(const uint8_t bytes[lengthAsBytes]) {
    memcpy(state.asByte, bytes, lengthAsBytes);
}

uint8_t &aesBlock::operator[](std::size_t index) {
    return state.asByte[index];
}

const uint8_t *aesBlock::asBytes() {
    return state.asByte;
}

const uint32_t *aesBlock::asWords() {
    return state.asWord;
}

// Maps a 16 byte vector to a 4x4 matrix in the way AES State expects it
void aesBlock::vectorToMatrix(uint8_t vectorIn[16], uint8_t matrixOut[4][4]) {
    for (auto row = 0; row < 4; row++) {
        for (auto col = 0; col < 4; col++) {
            matrixOut[row][col] = vectorIn[(col * 4) + row];
        }
    }
}

// Maps a 4x4 matrix to a 16 byte vector in the way AES State expects it
void aesBlock::matrixToVector(uint8_t matrixIn[4][4], uint8_t vectorOut[16]) {
    for (auto row = 0; row < 4; row++) {
        for (auto col = 0; col < 4; col++) {
            vectorOut[(col * 4) + row] = matrixIn[row][col];
        }
    }
}

void aesBlock::bytesToWords(uint8_t bytesIn[16], uint32_t wordsOut[4]) {
    for (auto i = 0; i < 4; i++) {
        wordsOut[i] = bytesIn[i * 4] << 24 | bytesIn[i * 4 + 1] << 16 | bytesIn[i * 4 + 2] << 8 | bytesIn[i * 4 + 3];
    }
}

void aesBlock::wordsToBytes(uint32_t wordsIn[4], uint8_t bytesOut[16]) {
    for (auto i = 0; i < 4; i++) {
        bytesOut[i * 4]     = (wordsIn[i] >> 24) & 0xFF;
        bytesOut[i * 4 + 1] = (wordsIn[i] >> 16) & 0xFF;
        bytesOut[i * 4 + 2] = (wordsIn[i] >> 8) & 0xFF;
        bytesOut[i * 4 + 3] = wordsIn[i] & 0xFF;
    }
}

void aesBlock::encrypt(aesKey &key) {
#ifndef generic
    uint32_t plainTextAsWords[4];
    uint32_t cypherTextAsWords[4];
    bytesToWords(state.asByte, plainTextAsWords);        // convert the 16 bytes to 4 words the way the hardware expects it
    if (HAL_CRYP_Encrypt(&hcryp, plainTextAsWords, 4, cypherTextAsWords, 50) != HAL_OK) {
        // Error_Handler();
    }
    wordsToBytes(cypherTextAsWords, state.asByte);        // convert the 4 words back to 16 bytes

// TODO : provide a hardware implementation
// Initialize AES :
// * clear EN bit in AES_CR
// * configure EAS mode : encryption - mode = 0b00
// * chaning mode : ECB - mode = 0b00
// * datatype : 32 bit - datatype = 0b00
// * key size : 128 bit - keysize = 0b00
// * write key in AES_KEYRx registers

// Encryption - polling mode
// * write 4 words into AES_DINR register -
// * wait until CCF in AES_SR is set
// * read 4 words from AES_DOUTR register

// Disable AES peripheral by clearing EN bit in AES_CR
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
    for (uint32_t index = 0; index < lengthAsBytes; index++) {
        state.asByte[index] = sbox::data[state.asByte[index]];
    }
}

void aesBlock::XOR(const uint8_t *data) {
    for (uint32_t index = 0; index < lengthAsBytes; index++) {
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
    vectorToMatrix(state.asByte, tempState);
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
    matrixToVector(tempState, state.asByte);
}
