// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <cstring>

template <uint32_t toBeWidthHeightInBits>
class bitMatrix {
  public:
    const uint32_t widthHeightInBits{toBeWidthHeightInBits};
    const uint32_t sizeInBits{toBeWidthHeightInBits * toBeWidthHeightInBits};
    const uint32_t sizeInBytes{((toBeWidthHeightInBits * toBeWidthHeightInBits) + 7) / 8};

    void clearAllBits() { memset(data, 0, sizeInBytes); }
    void setBit(uint32_t x, uint32_t y) { setOrClearBit(x, y, true); };
    void clearBit(uint32_t x, uint32_t y) { setOrClearBit(x, y, false); };
    void invertBit(uint32_t x, uint32_t y) { setOrClearBit(x, y, !getBit(x, y)); };
    bool getBit(uint32_t x, uint32_t y) { return (data[byteOffset(x, y)] & bitMask(x, y)) != 0; };

#ifndef unitTesting

  private:
#endif

    uint8_t data[((toBeWidthHeightInBits * toBeWidthHeightInBits) + 7U) / 8U]{0};
    uint32_t byteOffset(uint32_t x, uint32_t y) const { return ((y * widthHeightInBits + x) >> 3U); };
    uint8_t bitMask(uint32_t x, uint32_t y) const { return static_cast<uint8_t>(1U << (7U - ((y * widthHeightInBits + x) % 8U))); };
    void setOrClearBit(uint32_t x, uint32_t y, bool newBitState) {
        if (newBitState) {
            data[byteOffset(x, y)] |= bitMask(x, y);
        } else {
            data[byteOffset(x, y)] &= ~bitMask(x, y);
        }
    };
};
