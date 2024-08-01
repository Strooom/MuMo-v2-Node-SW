// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################
// based on the work of Richard Moore : https://github.com/ricmoo/QRCode
// based on the work of Nayuki : https://www.nayuki.io/page/qr-code-generator-library

#pragma once
#include <stdint.h>
#include <cstring>

class bitVector {
  public:
    static uint32_t neededLengthInBytes(uint32_t lengthInBits) { return ((lengthInBits + 7) / 8); };

    bitVector(uint8_t *externalDataStorage, uint32_t toBeLengthInBits) : data{externalDataStorage}, length{toBeLengthInBits}, lengthInBytes{neededLengthInBytes(toBeLengthInBits)} {}

    uint32_t lengthInBits() const { return length; };
    uint32_t levelInBits() const { return level; };
    
    // void clearAllBits() { memset(data, 0, lengthInBytes); }
    void setOrClearBit(uint32_t bitIndex, bool newBitState) {
        if (newBitState) {
            data[byteOffset(bitIndex)] |= bitMask(bitIndex);
        } else {
            data[byteOffset(bitIndex)] &= ~bitMask(bitIndex);
        }
    };
    void setBit(uint32_t bitIndex) { setOrClearBit(bitIndex, true); };
    void clearBit(uint32_t bitIndex) { setOrClearBit(bitIndex, false); };
    void invertBit(uint32_t bitIndex) { setOrClearBit(bitIndex, !getBit(bitIndex)); };
    bool getBit(uint32_t bitIndex) const { return (data[byteOffset(bitIndex)] & bitMask(bitIndex)) != 0; };

    void appendBits(uint32_t bits, uint8_t nmbrOfBits) {
        for (uint32_t index = 0; index < nmbrOfBits; index++) {
            if (((bits >> (nmbrOfBits - index - 1)) & 1) == 1) {
                setBit(level);
            } else {
                clearBit(level);
            }
            level++;
        }
    }

    uint32_t byteOffset(uint32_t bitIndex) const { return (bitIndex >> 3U); };
    uint8_t bitMask(uint32_t bitIndex) const { return static_cast<uint8_t>(1U << (7U - (bitIndex % 8U))); };

#ifndef unitTesting

  private:
#endif
    uint8_t *const data;
    uint32_t length{0};
    uint32_t lengthInBytes{0};
    uint32_t level{0};
};
