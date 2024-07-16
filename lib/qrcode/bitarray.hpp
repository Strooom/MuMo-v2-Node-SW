// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################
// based on the work of Richard Moore : https://github.com/ricmoo/QRCode
// based on the work of Nayuki : https://www.nayuki.io/page/qr-code-generator-library

#pragma once
#include <stdint.h>
#include <cstring>

template <uint32_t tobeWidthHeightInBits>
class bitArray {
  public:
    const uint32_t widthHeightInBits{tobeWidthHeightInBits};
    const uint32_t sizeInBits{tobeWidthHeightInBits * tobeWidthHeightInBits};
    const uint32_t sizeInBytes{((tobeWidthHeightInBits * tobeWidthHeightInBits) + 7) / 8};
    uint8_t data[((tobeWidthHeightInBits * tobeWidthHeightInBits) + 7) / 8]{0};

    void clearAllBits() { memset(data, 0, sizeInBytes); }
    void setOrClearBit(uint32_t x, uint32_t y, bool on) {
        if (on) {
            data[byteOffset(x, y)] |= bitMask(x, y);
        } else {
            data[byteOffset(x, y)] &= ~bitMask(x, y);
        }
    };
    void setBit(uint32_t x, uint32_t y) { setOrClearBit(x, y, true); };
    void clearBit(uint32_t x, uint32_t y) { setOrClearBit(x, y, false); };
    void invertBit(uint32_t x, uint32_t y) { setOrClearBit(x, y, !getBit(x, y)); };
    bool getBit(uint32_t x, uint32_t y) { return (data[byteOffset(x, y)] & bitMask(x, y)) != 0; };

    uint32_t byteOffset(uint32_t x, uint32_t y) const { return ((y * widthHeightInBits + x) >> 3U); };
    uint8_t bitMask(uint32_t x, uint32_t y) const { return (1U << (7U - ((y * widthHeightInBits + x) % 8U))); };
};
