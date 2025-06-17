#include <aeskey.hpp>
#include <cstring>        // needed for memcpy()
#include <sbox.hpp>
#include <rcon.hpp>
#include <hexascii.hpp>

uint8_t* aesKey::asBytes() {
    return key.asByte;
}

uint32_t* aesKey::asWords() {
    return key.asUint32;
}

void aesKey::setFromByteArray(const uint8_t bytes[lengthInBytes]) {
    // old
    (void)memcpy(key.asByte, bytes, lengthInBytes);

    // mid
    (void)memcpy(key.asUint32, key.asByte, lengthInBytes);

    // new
    (void)memcpy(keyAsBytes, bytes, lengthInBytes);
    syncWordsFromBytes();
    syncHexStringFromBytes();

#ifndef HARDWARE_AES
    expandKey();
#endif
}

void aesKey::setFromWordArray(const uint32_t wordsIn[lengthInWords]) {
    for (uint32_t wordIndex = 0; wordIndex < 4; wordIndex++) {
        key.asUint32[wordIndex] = wordsIn[wordIndex];
    }
}

void aesKey::setFromHexString(const char* string) {
    // old
    uint8_t tmpBytes[lengthInBytes];
    hexAscii::hexStringToByteArray(tmpBytes, string, 32U);
    (void)memcpy(key.asByte, tmpBytes, lengthInBytes);

    // mid
    (void)memcpy(key.asUint32, key.asByte, lengthInBytes);

    //new
    hexAscii::hexStringToByteArray(keyAsBytes, string, 32U);
    syncWordsFromBytes();
    syncHexStringFromBytes();

#ifndef HARDWARE_AES
    expandKey();
#endif
}

uint32_t aesKey::swapLittleBigEndian(uint32_t wordIn) {
    // ARM Cortex-M4 stores uin32_t in little endian format, but STM32WLE5 AES peripheral expects big endian format. This function swaps the bytes in a word.
    uint32_t wordOut;
    wordOut = (wordIn & 0xFF000000) >> 24 | (wordIn & 0x00FF0000) >> 8 | (wordIn & 0x0000FF00) << 8 | (wordIn & 0x000000FF) << 24;
    return wordOut;
}

#ifndef HARDWARE_AES
void aesKey::expandKey() {
    (void)memcpy(expandedKey, key.asByte, 16);
    for (uint8_t round = 1; round <= 10; round++) {
        (void)memcpy(expandedKey + (round * 16), expandedKey + ((round - 1) * 16), 16);
        calculateRoundKey(round);
    }
}
#endif

#ifndef HARDWARE_AES
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
#endif

void aesKey::syncWordsFromBytes() {
    (void)memcpy(keyAsWords, keyAsBytes, lengthInBytes);
}
void aesKey::syncHexStringFromBytes() {
    hexAscii::byteArrayToHexString(keyAsHexString, keyAsBytes, lengthInBytes);
}
