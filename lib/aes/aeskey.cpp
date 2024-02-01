#include <aeskey.hpp>
#include <cstring>        // needed for memcpy()
#include <sbox.hpp>
#include <rcon.hpp>
#include <aesblock.hpp>
#include <hexascii.hpp>

const uint8_t* aesKey::asBytes() {
    return key.asByte;
}

const uint32_t* aesKey::asWords() {
    return key.asWord;
}

void aesKey::setFromByteArray(const uint8_t bytes[lengthInBytes]) {
    memcpy(key.asByte, bytes, lengthInBytes);
    expandKey();
}

void aesKey::setFromHexString(const char* string) {
    uint8_t tmpBytes[lengthInBytes];
    hexAscii::hexStringToByteArray(string, tmpBytes);
    memcpy(key.asByte, tmpBytes, lengthInBytes);
    expandKey();
}

uint32_t aesKey::swapLittleBigEndian(uint32_t wordIn) {
    // ARM Cortex-M4 stores uin32_t in little endian format, but STM32WLE5 AES peripheral expects big endian format. This function swaps the bytes in a word.
    uint32_t wordOut;
    wordOut = (wordIn & 0xFF000000) >> 24 | (wordIn & 0x00FF0000) >> 8 | (wordIn & 0x0000FF00) << 8 | (wordIn & 0x000000FF) << 24;
    return wordOut;
}


void aesKey::expandKey() {
    memcpy(expandedKey, key.asByte, 16);
    for (auto round = 1; round <= 10; round++) {
        memcpy(expandedKey + (round * 16), expandedKey + ((round - 1) * 16), 16);
        calculateRoundKey(round);
    }
}

void aesKey::calculateRoundKey(uint8_t round) {
    unsigned char temp[4];
    uint8_t* expandedKeyPtr = expandedKey + (round * 16);

    temp[0] = sbox::data[expandedKeyPtr[12 + 1]];
    temp[1] = sbox::data[expandedKeyPtr[12 + 2]];
    temp[2] = sbox::data[expandedKeyPtr[12 + 3]];
    temp[3] = sbox::data[expandedKeyPtr[12 + 0]];

    temp[0] ^= rcon::data[round];

    for (int index = 0; index < 16; index++) {
        expandedKeyPtr[index] ^= temp[index % 4];
        temp[index % 4] = expandedKeyPtr[index];
    }
}
