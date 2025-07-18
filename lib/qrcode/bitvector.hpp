// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <cstring>

template <uint32_t toBeLength>
class bitVector {
  public:
    const uint32_t length{toBeLength};
    const uint32_t lengthInBytes{(toBeLength + 7U) / 8U};
    void reset() {
        level = 0;
        clearAllBits();
    };
    void clearAllBits() { memset(data, 0, lengthInBytes); }
    void setBit(uint32_t bitIndex) { setOrClearBit(bitIndex, true); };
    void clearBit(uint32_t bitIndex) { setOrClearBit(bitIndex, false); };
    void invertBit(uint32_t bitIndex) { setOrClearBit(bitIndex, !getBit(bitIndex)); };
    bool getBit(uint32_t bitIndex) const { return (data[byteOffset(bitIndex)] & bitMask(bitIndex)) != 0; };
    void appendBits(uint32_t bits, uint32_t nmbrOfBits) {
        for (uint32_t index = 0; index < nmbrOfBits; index++) {
            if (((bits >> (nmbrOfBits - index - 1)) & 1) == 1) {
                setBit(level);
            } else {
                clearBit(level);
            }
            level++;
        }
    }
    void appendByte(uint8_t byteToAppend) {        // this only works when level is a multiple of 8, ie. data is aligned with bytes
        data[levelInBytes()] = byteToAppend;
        level += 8;
    }
    uint8_t getByte(uint32_t byteIndex) const { return data[byteIndex]; };
    uint32_t levelInBits() const { return level; };
    uint32_t levelInBytes() const { return (level + 7U) / 8U; };

#ifndef unitTesting

  private:
#endif
    uint8_t data[(toBeLength + 7U) / 8U]{0};
    uint32_t level{0};
    uint32_t byteOffset(uint32_t bitIndex) const { return (bitIndex >> 3U); };
    uint8_t bitMask(uint32_t bitIndex) const { return static_cast<uint8_t>(1U << (7U - (bitIndex % 8U))); };
    void setOrClearBit(uint32_t bitIndex, bool newBitState) {
        if (newBitState) {
            data[byteOffset(bitIndex)] |= bitMask(bitIndex);
        } else {
            data[byteOffset(bitIndex)] &= ~bitMask(bitIndex);
        }
    };
};
