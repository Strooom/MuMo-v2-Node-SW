// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <swaplittleandbigendian.hpp>

uint32_t swapLittleBigEndian(uint32_t wordIn) {
    uint32_t wordOut;
    wordOut = (wordIn & 0xFF000000) >> 24 | (wordIn & 0x00FF0000) >> 8 | (wordIn & 0x0000FF00) << 8 | (wordIn & 0x000000FF) << 24;
    return wordOut;
}

uint32_t bytesToBigEndianWord(uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3) {
    uint32_t wordOut;
    wordOut = (static_cast<uint32_t>(byte0)) + (static_cast<uint32_t>(byte1) << 8) + (static_cast<uint32_t>(byte2) << 16) + (static_cast<uint32_t>(byte3) << 24);
    return wordOut;
}

uint8_t bigEndianWordToByte0(uint32_t wordIn) {
    return (wordIn & 0x000000FF);
}

uint8_t bigEndianWordToByte1(uint32_t wordIn) {
    return (wordIn & 0x0000FF00) >> 8;
}

uint8_t bigEndianWordToByte2(uint32_t wordIn) {
    return (wordIn & 0x00FF0000) >> 16;
}

uint8_t bigEndianWordToByte3(uint32_t wordIn) {
    return (wordIn & 0xFF000000) >> 24;
}
