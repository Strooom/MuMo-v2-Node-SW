// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################
// based on the work of Richard Moore : https://github.com/ricmoo/QRCode
// based on the work of Nayuki : https://www.nayuki.io/page/qr-code-generator-library

#pragma once
#include <stdint.h>
#include <cstring>

class bitMatrix {
  public:
    static uint32_t neededLengthInBytes(uint32_t widthHeightInBits) { return (((widthHeightInBits * widthHeightInBits) + 7) / 8); };
    bitMatrix(uint8_t *externalDataStorage, uint32_t toBewidthHeightInBits) : data{externalDataStorage}, widthHeight{toBewidthHeightInBits}, lengthInBytes{neededLengthInBytes(toBewidthHeightInBits)} { clearAllBits(); };
    uint32_t widthHeightInBits() const { return widthHeight; };

    void clearAllBits() { memset(data, 0, lengthInBytes); }
    void setOrClearBit(uint32_t x, uint32_t y, bool newBitState) {
        if (newBitState) {
            data[byteOffset(x, y)] |= bitMask(x, y);
        } else {
            data[byteOffset(x, y)] &= ~bitMask(x, y);
        }
    };
    void setBit(uint32_t x, uint32_t y) { setOrClearBit(x, y, true); };
    void clearBit(uint32_t x, uint32_t y) { setOrClearBit(x, y, false); };
    void invertBit(uint32_t x, uint32_t y) { setOrClearBit(x, y, !getBit(x, y)); };
    bool getBit(uint32_t x, uint32_t y) const { return (data[byteOffset(x, y)] & bitMask(x, y)) != 0; };

    uint32_t byteOffset(uint32_t x, uint32_t y) const { return ((y * widthHeight + x) >> 3U); };
    uint8_t bitMask(uint32_t x, uint32_t y) const { return static_cast<uint8_t>(1U << (7U - ((y * widthHeight + x) % 8U))); };

#ifndef unitTesting

  private:
#endif
    uint8_t *const data;
    uint32_t widthHeight{0};
    uint32_t lengthInBytes{0};
};

// static bool mask(uint32_t x, uint32_t y, uint32_t mask) {
//     switch (mask) {
//         case 0:
//             return ((x + y) % 2 == 0);
//         case 1:
//             return (y % 2 == 0);
//         case 2:
//             return (x % 3 == 0);
//         case 3:
//             return ((x + y) % 3 == 0);
//         case 4:
//             return ((x / 3 + y / 2) % 2 == 0);
//         case 5:
//             return (x * y % 2 + x * y % 3 == 0);
//         case 6:
//             return ((x * y % 2 + x * y % 3) % 2 == 0);
//         case 7:
//             return (((x + y) % 2 + x * y % 3) % 2 == 0);
//         default:
//             return false;
//     }
// }

// void applyMask(bitArray<tobeWidthHeightInBits> &isFunction, uint32_t maskId) {
//     for (uint32_t y = 0; y < widthHeightInBits; y++) {
//         for (uint32_t x = 0; x < widthHeightInBits; x++) {
//             if (!isFunction.getBit(x, y) && mask(x, y, maskId)) {
//                 invertBit(x, y);
//             }
//         }
//     }
// }
// }
// ;
