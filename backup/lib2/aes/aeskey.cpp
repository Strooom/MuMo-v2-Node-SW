#include "aeskey.h"
#include <cstring>        // needed for memcpy()
#include "sbox.h"
#include "rcon.h"

const uint8_t* aesKey::asBytes() {
    return key.asByte;
}

const uint32_t* aesKey::asWords() {
    return key.asWord;
}

void aesKey::set(const uint8_t bytes[lengthAsBytes]) {
    memcpy(key.asByte, bytes, lengthAsBytes);
    expandKey();
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