// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <cstring>

template <uint32_t maxWidthHeightInBits>
class bitMatrix {
  public:
    uint32_t getWidthHeightInBits() { return widthHeightInBits; };
    uint32_t getSizeInBits() { return widthHeightInBits * widthHeightInBits; };
    uint32_t getSizeInBytes() { return (getSizeInBits() + 7U) / 8U; };

    void setWidthHeightInBits(uint32_t newWidthHeightInBits) {
        if (newWidthHeightInBits <= maxWidthHeightInBits) {
            widthHeightInBits = newWidthHeightInBits;
            sizeInBits        = widthHeightInBits * widthHeightInBits;
            sizeInBytes       = (sizeInBits + 7U) / 8U;
        }
    }

    void clearAllBits() { memset(data, 0, sizeInBytes); }
    void setAllBits() { memset(data, 0xFF, sizeInBytes); }
    void setBit(uint32_t x, uint32_t y) { setOrClearBit(x, y, true); };
    void clearBit(uint32_t x, uint32_t y) { setOrClearBit(x, y, false); };
    void invertBit(uint32_t x, uint32_t y) { setOrClearBit(x, y, !getBit(x, y)); };
    bool getBit(uint32_t x, uint32_t y) { return (data[byteOffset(x, y)] & bitMask(x, y)) != 0; };
    void setOrClearBit(uint32_t x, uint32_t y, bool newBitState) {
        if (newBitState) {
            data[byteOffset(x, y)] |= bitMask(x, y);
        } else {
            data[byteOffset(x, y)] &= ~bitMask(x, y);
        }
    };

#ifndef unitTesting

  private:
#endif
    uint32_t widthHeightInBits{maxWidthHeightInBits};
    uint32_t sizeInBits{maxWidthHeightInBits * maxWidthHeightInBits};
    uint32_t sizeInBytes{((maxWidthHeightInBits * maxWidthHeightInBits) + 7U) / 8U};

    uint8_t data[((maxWidthHeightInBits * maxWidthHeightInBits) + 7U) / 8U]{0};
    uint32_t byteOffset(uint32_t x, uint32_t y) const { return ((y * widthHeightInBits + x) >> 3U); };
    uint8_t bitMask(uint32_t x, uint32_t y) const { return static_cast<uint8_t>(1U << (7U - ((y * widthHeightInBits + x) % 8U))); };
};
