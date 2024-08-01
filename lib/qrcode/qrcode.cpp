#include <qrcode.hpp>
#include <stdlib.h>
#include <cstring>

// This code is using the vocabulary of the QR Code standard ISO/IEC 18004:2006
// See qrcode.md for a short explanation

bool qrCode::isValid(uint32_t theVersion) {
    if (theVersion == 0) {
        return false;
    }
    if (theVersion > maxVersion) {
        return false;
    }
    return true;
}

 uint32_t qrCode::size(uint32_t theVersion) {return 17 + 4 * theVersion;}

bool qrCode::isNumeric(const char data) {
    return (data >= '0' && data <= '9');
}

bool qrCode::isNumeric(const char *data) {
    uint32_t length = strlen(data);
    return isNumeric(data, length);
}

bool qrCode::isNumeric(const char *data, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        if (!isNumeric(data[i])) {
            return false;
        }
    }
    return true;
}

bool qrCode::isAlphanumeric(const char data) {
    if ((data >= '0') && (data <= '9')) {
        return true;
    }
    if ((data >= 'A') && (data <= 'Z')) {
        return true;
    }
    if ((data == ' ') || (data == '$') || (data == '%') || (data == '*') || (data == '+') || (data == '-') || (data == '.') || (data == '/') || (data == ':')) {
        return true;
    }
    return false;
}

bool qrCode::isAlphanumeric(const char *data) {
    uint32_t length = strlen(data);
    return isAlphanumeric(data, length);
}

bool qrCode::isAlphanumeric(const char *data, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        if (!isAlphanumeric(data[i])) {
            return false;
        }
    }
    return true;
}

uint8_t qrCode::compressNumeric(char theCharacter) {
    if (isNumeric(theCharacter)) {
        return (theCharacter - '0');
    } else
        return 0;
}

uint8_t qrCode::compressAlphanumeric(char theCharacter) {
    if (isAlphanumeric(theCharacter)) {
        if (theCharacter >= '0' && theCharacter <= '9') {
            return (theCharacter - '0');
        }
        if (theCharacter >= 'A' && theCharacter <= 'Z') {
            return (theCharacter - 'A' + 10);
        }
        switch (theCharacter) {
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
            case ':':        // intentional fallthrough
            default:
                return 44;
        }
    } else
        return 0;
}

uint8_t qrCode::modeIndicator(encodingFormat theEncodingFormat) {
    switch (theEncodingFormat) {
        case encodingFormat::numeric:
            return 0b0001;

        case encodingFormat::alphanumeric:
            return 0b0010;

        case encodingFormat::byte:        // intentional fallthrough
        default:
            return 0b0100;
    }
}

uint32_t qrCode::characterCountIndicatorLength(uint32_t theVersion, encodingFormat theEncodingFormat) {
    if (theVersion < 10) {
        switch (theEncodingFormat) {
            case encodingFormat::numeric:
                return 10;
            case encodingFormat::alphanumeric:
                return 9;
            case encodingFormat::byte:        // intentional fallthrough
            default:
                return 8;
        }
    }
    if (theVersion < 27) {
        switch (theEncodingFormat) {
            case encodingFormat::numeric:
                return 12;
            case encodingFormat::alphanumeric:
                return 11;
            case encodingFormat::byte:        // intentional fallthrough
            default:
                return 16;
        }
    }
    switch (theEncodingFormat) {
        case encodingFormat::numeric:
            return 14;
        case encodingFormat::alphanumeric:
            return 13;
        case encodingFormat::byte:        // intentional fallthrough
        default:
            return 16;
    }
}

uint32_t qrCode::payloadLengthInBits(uint32_t length, uint32_t theVersion, encodingFormat theEncodingFormat) {
    uint32_t modeIndicatorLength{4U};
    switch (theEncodingFormat) {
        case encodingFormat::numeric:
            uint32_t remainderLength;
            switch (length % 3) {
                case 0:
                    remainderLength = 0;
                    break;
                case 1:
                    remainderLength = 4;
                    break;
                case 2:
                    remainderLength = 7;
                    break;
            }
            return modeIndicatorLength + characterCountIndicatorLength(theVersion, theEncodingFormat) + 10 * (length / 3) + remainderLength;
        case encodingFormat::alphanumeric:
            return modeIndicatorLength + characterCountIndicatorLength(theVersion, theEncodingFormat) + 11 * (length / 2) + 6 * (length % 2);
        case encodingFormat::byte:        // intentional fallthrough
        default:
            return modeIndicatorLength + characterCountIndicatorLength(theVersion, theEncodingFormat) + 8 * length;
    }
}

void qrCode::encodeData(bitVector &theBitVector, const char *text, uint32_t theVersion, encodingFormat theEncodingFormat) {
    uint32_t length = strlen(text);
    theBitVector.appendBits(modeIndicator(theEncodingFormat), 4);
    theBitVector.appendBits(length, characterCountIndicatorLength(theVersion, theEncodingFormat));

    switch (theEncodingFormat) {
        case encodingFormat::numeric: {
            uint32_t accumData  = 0;
            uint32_t accumCount = 0;
            for (uint32_t i = 0; i < length; i++) {
                accumData = accumData * 10 + compressNumeric(text[i]);
                accumCount++;
                if (accumCount == 3) {
                    theBitVector.appendBits(accumData, 10U);
                    accumData  = 0;
                    accumCount = 0;
                }
            }
            if (accumCount > 0) {
                theBitVector.appendBits(accumData, (accumCount * 3U) + 1U);
            }
        } break;

        case encodingFormat::alphanumeric: {
            uint32_t accumData  = 0;
            uint32_t accumCount = 0;
            for (uint32_t i = 0; i < length; i++) {
                accumData = accumData * 45 + compressAlphanumeric(text[i]);
                accumCount++;
                if (accumCount == 2) {
                    theBitVector.appendBits(accumData, 11U);
                    accumData  = 0;
                    accumCount = 0;
                }
            }
            if (accumCount > 0) {
                theBitVector.appendBits(accumData, 6U);
            }
        } break;

        case encodingFormat::byte:        // intentional fallthrough
        default: {
            for (uint32_t i = 0; i < length; i++) {
                theBitVector.appendBits(text[i], 8U);
            }
        } break;
    }
}

static bool isDataModule(uint32_t x, uint32_t y, uint32_t theVersion) {
    return false;
}

// uint32_t qrCode::versionNeeded(encodingFormat theEncodingFormat, uint32_t dataLength, errorCorrectionLevel theErrorCorrectionLevel) {
//     for (uint32_t result = 1; result <= maxVersion; result++) {
//         uint32_t nmbrOfRawDataModules = nmbrDataModulesAvailable(result, theErrorCorrectionLevel);
//         if (nmbrRawDataModules[result] < nmbrOfRawDataModules) {
//             return result;
//         }
//         return 0;
//     }
// }

// uint32_t qrCode::nmbrDataModulesAvailable(uint32_t theVersion, errorCorrectionLevel theErrorCorrectionLevel) {
//     if (!isValid(theVersion)) {
//         return 0;
//     }
//     uint32_t totalModulesAvailable     = nmbrRawDataModules[theVersion - 1];
//     uint32_t modulesForErrorCorrection = 8 * nmbrErrorCorrectionCodewords[static_cast<uint32_t>(theErrorCorrectionLevel)][theVersion - 1];
//     return (totalModulesAvailable - modulesForErrorCorrection);
// }

// int qrCode::max(int a, int b) {
//     if (a > b) {
//         return a;
//     }
//     return b;
// }

// char qrCode::getModeBits(uint8_t version, encodingFormat theEncodingFormat) {
//     // We store the following tightly packed (less 8) in modeInfo
//     //               <=9  <=26  <= 40
//     // NUMERIC      ( 10,   12,    14);
//     // ALPHANUMERIC (  9,   11,    13);
//     // BYTE         (  8,   16,    16);

//     // Note: We use 15 instead of 16; since 15 doesn't exist and we cannot store 16 (8 + 8) in 3 bits
//     // hex(int("".join(reversed([('00' + bin(x - 8)[2:])[-3:] for x in [10, 9, 8, 12, 11, 15, 14, 13, 15]])), 2))

//     auto nmbrOfShifts     = 3U * static_cast<uint32_t>(theEncodingFormat);
//     unsigned int modeInfo = 0x7bbb80a;

//     if (version > 9) {
//         modeInfo >>= 9;
//     }
//     if (version > 26) {
//         modeInfo >>= 9;
//     }
//     char result = 8 + ((modeInfo >> nmbrOfShifts) & 0x07);
//     if (result == 15) {
//         result = 16;
//     }

//     return result;
// }

// void qrCode::drawTimingPattern() {
//     // modules
//     for (uint32_t index = 0; index < widthHeightInModules; index += 2U) {
//         bitArray::setBit(6, index);
//         bitArray::setBit(index, 6);
//     }
//     // isFunction
//     for (uint32_t index = 0; index < widthHeightInModules; index++) {
//         bitArray::setBit(6, index);
//         bitArray::setBit(index, 6);
//     }
// }

// // Draws a 9*9 finder pattern including the border separator, with the center module at (x, y).
// // Needs to draw real pattern in qrCode bitArray and all ones on the isFunction pattern
// // TODO : this can be simplified by using a const bitArray<9> and simply copy those bits
// void qrCode::drawFinderPattern(uint32_t centerX, uint32_t centerY) {
//     uint8_t size = modules->bitOffsetOrWidth;

//     for (int8_t i = -4; i <= 4; i++) {
//         for (int8_t j = -4; j <= 4; j++) {
//             uint8_t dist = max(abs(i), abs(j));        // Chebyshev/infinity norm
//             int16_t xx = centerX + j, yy = centerY + i;
//             if (0 <= xx && xx < size && 0 <= yy && yy < size) {
//                 setFunctionModule(modules, isFunction, xx, yy, dist != 2 && dist != 4);
//             }
//         }
//     }
// }

// // Draws a 5*5 alignment pattern, with the center module at (x, y).
// // TODO : this can be simplified by using a const bitArray<5> and simply copy those bits
// void qrCode::drawAlignmentPattern(uint32_t centerX, uint32_t centerY) {
//     for (int8_t i = -2; i <= 2; i++) {
//         for (int8_t j = -2; j <= 2; j++) {
//             setFunctionModule(modules, isFunction, x + j, y + i, max(abs(i), abs(j)) != 1);
//         }
//     }
// }

// // Draws two copies of the format bits (with its own error correction code)
// // based on the given mask and this object's error correction level field.
// void qrCode::drawFormatBits(uint32_t ecc, uint32_t mask) {
//     uint8_t size = modules->bitOffsetOrWidth;

//     // Calculate error correction code and pack bits
//     uint32_t data = ecc << 3 | mask;        // errCorrLvl is uint2, mask is uint3
//     uint32_t rem  = data;
//     for (int i = 0; i < 10; i++) {
//         rem = (rem << 1) ^ ((rem >> 9) * 0x537);
//     }

//     data = data << 10 | rem;
//     data ^= 0x5412;        // uint15

//     // Draw first copy
//     for (uint8_t i = 0; i <= 5; i++) {
//         setFunctionModule(modules, isFunction, 8, i, ((data >> i) & 1) != 0);
//     }

//     setFunctionModule(modules, isFunction, 8, 7, ((data >> 6) & 1) != 0);
//     setFunctionModule(modules, isFunction, 8, 8, ((data >> 7) & 1) != 0);
//     setFunctionModule(modules, isFunction, 7, 8, ((data >> 8) & 1) != 0);

//     for (int8_t i = 9; i < 15; i++) {
//         setFunctionModule(modules, isFunction, 14 - i, 8, ((data >> i) & 1) != 0);
//     }

//     // Draw second copy
//     for (int8_t i = 0; i <= 7; i++) {
//         setFunctionModule(modules, isFunction, size - 1 - i, 8, ((data >> i) & 1) != 0);
//     }

//     for (int8_t i = 8; i < 15; i++) {
//         setFunctionModule(modules, isFunction, 8, size - 15 + i, ((data >> i) & 1) != 0);
//     }

//     setFunctionModule(modules, isFunction, 8, size - 8, true);
// }

// // Draws two copies of the version bits (with its own error correction code),
// // based on this object's version field (which only has an effect for 7 <= version <= 40).
// void qrCode::drawVersion(uint32_t version) {
//     if (version < 7) {
//         return;
//     }

//     // Calculate error correction code and pack bits
//     uint32_t rem = version;        // version is uint6, in the range [7, 40]
//     for (uint8_t i = 0; i < 12; i++) {
//         rem = (rem << 1) ^ ((rem >> 11) * 0x1F25);
//     }

//     uint32_t data = version << 12 | rem;        // uint18

//     // Draw two copies
//     for (uint8_t i = 0; i < 18; i++) {
//         bool bit  = ((data >> i) & 1) != 0;
//         uint8_t a = size - 11 + i % 3, b = i / 3;
//         setFunctionModule(modules, isFunction, a, b, bit);
//         setFunctionModule(modules, isFunction, b, a, bit);
//     }
// }
