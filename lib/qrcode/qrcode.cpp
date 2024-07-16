#include <qrcode.hpp>
#include <stdlib.h>
#include <cstring>


int qrCode::max(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

int8_t qrCode::getAlphanumeric(char c) {
    if (c >= '0' && c <= '9') {
        return (c - '0');
    }
    if (c >= 'A' && c <= 'Z') {
        return (c - 'A' + 10);
    }

    switch (c) {
        case ' ':
            return 36;
        case '$':
            return 37;
        case '%':
            return 38;
        case '*':
            return 39;
        case '+':
            return 40;
        case '-':
            return 41;
        case '.':
            return 42;
        case '/':
            return 43;
        case ':':
            return 44;
        default:
            return -1;
    }
}

bool qrCode::isAlphanumeric(const char *text, uint16_t length) {
    while (length != 0) {
        if (getAlphanumeric(text[--length]) == -1) {
            return false;
        }
    }
    return true;
}

bool qrCode::isNumeric(const char *text, uint16_t length) {
    while (length != 0) {
        char c = text[--length];
        if (c < '0' || c > '9') {
            return false;
        }
    }
    return true;
}


char qrCode::getModeBits(uint8_t version, encodingFormat theEncodingFormat) {
    // We store the following tightly packed (less 8) in modeInfo
    //               <=9  <=26  <= 40
    // NUMERIC      ( 10,   12,    14);
    // ALPHANUMERIC (  9,   11,    13);
    // BYTE         (  8,   16,    16);

    // Note: We use 15 instead of 16; since 15 doesn't exist and we cannot store 16 (8 + 8) in 3 bits
    // hex(int("".join(reversed([('00' + bin(x - 8)[2:])[-3:] for x in [10, 9, 8, 12, 11, 15, 14, 13, 15]])), 2))

    auto nmbrOfShifts     = 3U * static_cast<uint32_t>(theEncodingFormat);
    unsigned int modeInfo = 0x7bbb80a;

    if (version > 9) {
        modeInfo >>= 9;
    }
    if (version > 26) {
        modeInfo >>= 9;
    }
    char result = 8 + ((modeInfo >> nmbrOfShifts) & 0x07);
    if (result == 15) {
        result = 16;
    }

    return result;
}

//  void qrCode::bb_setBit(BitBucket *bitGrid, uint8_t x, uint8_t y, bool on) {
//     uint32_t offset = y * bitGrid->bitOffsetOrWidth + x;
//     uint8_t mask    = 1 << (7 - (offset & 0x07));
//     if (on) {
//         bitGrid->data[offset >> 3] |= mask;
//     } else {
//         bitGrid->data[offset >> 3] &= ~mask;
//     }
// }

// Replace invert with calling get and then setting the inverted bit

//  void qrCode::bb_invertBit(BitBucket *bitGrid, uint8_t x, uint8_t y, bool invert) {
//     uint32_t offset = y * bitGrid->bitOffsetOrWidth + x;
//     uint8_t mask    = 1 << (7 - (offset & 0x07));
//     bool on         = ((bitGrid->data[offset >> 3] & (1 << (7 - (offset & 0x07)))) != 0);
//     if (on ^ invert) {
//         bitGrid->data[offset >> 3] |= mask;
//     } else {
//         bitGrid->data[offset >> 3] &= ~mask;
//     }
// }

//  bool qrCode::bb_getBit(BitBucket *bitGrid, uint8_t x, uint8_t y) {
//     uint32_t offset = y * bitGrid->bitOffsetOrWidth + x;
//     return (bitGrid->data[offset >> 3] & (1 << (7 - (offset & 0x07)))) != 0;
// }
