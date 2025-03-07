#include <qrcode.hpp>
#include <stdlib.h>
#include <cstring>

// This code is using the vocabulary of the QR Code standard ISO/IEC 18004:2006
// See qrcode.md for a short explanation

bool qrCode::isValidVersion(uint32_t theVersion) {
    if (theVersion == 0) {
        return false;
    }
    if (theVersion > maxVersion) {
        return false;
    }
    return true;
}

uint32_t qrCode::size(uint32_t theVersion) { return 17 + 4 * theVersion; }

bool qrCode::isNumeric(const char data) {
    return (data >= '0' && data <= '9');
}

bool qrCode::isNumeric(const char *data) {
    uint32_t length = strnlen(data, maxNumericLength);
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
    uint32_t length = strnlen(data, maxAlphanumericLength);
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

uint32_t qrCode::payloadLengthInBits(uint32_t dataLengthInBytes, uint32_t theVersion, encodingFormat theEncodingFormat) {
    uint32_t modeIndicatorLength{4U};
    switch (theEncodingFormat) {
        case encodingFormat::numeric:
            uint32_t remainderLength;
            switch (dataLengthInBytes % 3) {
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
            return modeIndicatorLength + characterCountIndicatorLength(theVersion, theEncodingFormat) + (10 * (dataLengthInBytes / 3)) + remainderLength;
        case encodingFormat::alphanumeric:
            return modeIndicatorLength + characterCountIndicatorLength(theVersion, theEncodingFormat) + (11 * (dataLengthInBytes / 2)) + (6 * (dataLengthInBytes % 2));
        case encodingFormat::byte:        // intentional fallthrough
        default:
            return modeIndicatorLength + characterCountIndicatorLength(theVersion, theEncodingFormat) + (8 * dataLengthInBytes);
    }
}

void qrCode::encodeData(bitVector &theBitVector, const char *text, uint32_t theVersion, encodingFormat theEncodingFormat) {
    uint32_t maxLength;
    switch (theEncodingFormat) {
        case encodingFormat::numeric:
            maxLength = maxNumericLength;
            break;

        case encodingFormat::alphanumeric:
            maxLength = maxAlphanumericLength;
            break;
        case encodingFormat::byte:        // intentional fallthrough
        default:
            maxLength = maxByteLength;
            break;
    }
    uint32_t length = strnlen(text, maxLength);
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

bool qrCode::isDataModule(uint32_t x, uint32_t y, uint32_t theVersion) {
    if (isFinderPatternOrSeparator(x, y, theVersion)) {
        return false;
    }
    if (isTimingPattern(x, y, theVersion)) {
        return false;
    }
    if (isAlignmentPattern(x, y, theVersion)) {
        return false;
    }
    if (isFormatInformation(x, y, theVersion)) {
        return false;
    }
    if (isVersionInformation(x, y, theVersion)) {
        return false;
    }
    return true;
}

bool qrCode::isFinderPatternOrSeparator(uint32_t x, uint32_t y, uint32_t theVersion) {
    if (x <= 7 && y <= 7) {
        return true;
    }
    if (x <= 7 && y >= size(theVersion) - 8) {
        return true;
    }
    if (x >= size(theVersion) - 8 && y <= 7) {
        return true;
    }
    return false;
}

bool qrCode::isTimingPattern(uint32_t x, uint32_t y, uint32_t theVersion) {
    if (x == 6 || y == 6) {
        return true;
    }
    return false;
}

bool qrCode::isDarkModule(uint32_t x, uint32_t y, uint32_t theVersion) {
    if ((x == 4 * theVersion + 9) && (y == 8)) {
        return true;
    }
    return false;
}

bool qrCode::isAlignmentPattern(uint32_t x, uint32_t y, uint32_t theVersion) {
    return false;
}

bool qrCode::isFormatInformation(uint32_t x, uint32_t y, uint32_t theVersion) {
    // TODO : remove some overlap with other functions, eg timing pattern and dark module
    if ((y == 8) && (x <= 8)) {
        return true;
    }
    if ((y == 8) && (x > (size(theVersion) - 8))) {
        return true;
    }
    if ((x == 8) && (y <= 8)) {
        return true;
    }
    if ((x == 8) && (x > (size(theVersion) - 8))) {
        return true;
    }
    return false;
}

bool qrCode::isVersionInformation(uint32_t x, uint32_t y, uint32_t theVersion) {
    if (theVersion < 7) {
        return false;
    }
    if ((x < 6) && (y >= size(theVersion) - 11) && (y <= size(theVersion) - 9)) {
        return true;
    }
    if ((y < 6) && (x >= size(theVersion) - 11) && (x <= size(theVersion) - 9)) {
        return true;
    }
    return false;
}

uint32_t qrCode::versionNeeded(encodingFormat theEncodingFormat, uint32_t dataLengthInBytes, errorCorrectionLevel theErrorCorrectionLevel) {
    for (uint32_t proposedVersion = 1; proposedVersion <= maxVersion; proposedVersion++) {
        uint32_t totalAvailableBits;
        uint32_t bitsNeededForErrorCorrection;
        uint32_t bitsNeededForPayload;

        bitsNeededForPayload         = payloadLengthInBits(dataLengthInBytes, proposedVersion, theEncodingFormat);
        bitsNeededForErrorCorrection = versionProperties[proposedVersion - 1].nmbrErrorCorrectionCodewords[static_cast<uint32_t>(theErrorCorrectionLevel)] * versionProperties[proposedVersion - 1].nmbrErrorCorrectionBlocks[static_cast<uint32_t>(theErrorCorrectionLevel)] * 8;
        totalAvailableBits           = nmbrOfDataModules(proposedVersion);
        if (bitsNeededForPayload + bitsNeededForErrorCorrection <= totalAvailableBits) {
            return proposedVersion;
        }
    }
    return 0;
}

uint32_t qrCode::nmbrOfTotalModules(uint32_t theVersion) {
    return size(theVersion) * size(theVersion);
}

uint32_t qrCode::nmbrOfFunctionModules(uint32_t theVersion) {
    uint32_t count{0};
    count += 3 * 64;                                          // Finder patterns including separators
    count += 2 * ((4 * theVersion) + 1);                      // Timing patterns
    count += 1;                                               // Dark module
    count += nmbrOfAlignmentPatterns(theVersion) * 25;        // Alignment patterns
    if (nmbrOfAlignmentPatternRowsOrCols(theVersion) > 2) {
        count -= 2 * ((nmbrOfAlignmentPatternRowsOrCols(theVersion) - 2) * 5);        // overlap of timing and alignment
    }
    count += (6U + 6U + 3U + 8U + 7U);        // Format information
    if (theVersion >= 7) {
        count += (2U * 6U * 3U);        // version information
    }
    return count;
}

uint32_t qrCode::nmbrOfDataModules(uint32_t theVersion) {
    return nmbrOfTotalModules(theVersion) - nmbrOfFunctionModules(theVersion);
}

uint32_t qrCode::nmbrOfErrorCorrectionModules(uint32_t theVersion, errorCorrectionLevel theErrorCorrectionLevel) {
    return versionProperties[theVersion - 1].nmbrErrorCorrectionCodewords[static_cast<uint32_t>(theErrorCorrectionLevel)] * versionProperties[theVersion - 1].nmbrErrorCorrectionBlocks[static_cast<uint32_t>(theErrorCorrectionLevel)] * 8;
}

uint32_t qrCode::nmbrOfAlignmentPatterns(uint32_t theVersion) {
    if (theVersion == 1) {
        return 0;
    } else {
        return (nmbrOfAlignmentPatternRowsOrCols(theVersion) * nmbrOfAlignmentPatternRowsOrCols(theVersion)) - 3;
    }
}

uint32_t qrCode::nmbrOfAlignmentPatternRowsOrCols(uint32_t theVersion) {
    if (theVersion == 1) {
        return 0;
    }
    return ((theVersion / 7U) + 2);
}

uint32_t qrCode::alignmentPatternSpacing(uint32_t theVersion) {
    if (theVersion != 32U) {
        return (theVersion * 4U + (nmbrOfAlignmentPatternRowsOrCols(theVersion) * 2U) + 1U) / ((2U * nmbrOfAlignmentPatternRowsOrCols(theVersion)) - 2U) * 2U;
    } else {
        return 26;
    }
}

uint32_t qrCode::alignmentPatternCoordinate(uint32_t index, uint32_t theVersion) {
    uint32_t maxIndex = nmbrOfAlignmentPatternRowsOrCols(theVersion) - 1;
    if (index > maxIndex) {
        return 0;
    }
    if (index == 0) {
        return 6U;
    } else {
        return size(theVersion) - 7 - (alignmentPatternSpacing(theVersion) * (maxIndex - index));
    }
}

void qrCode::drawFinderPattern(uint32_t centerX, uint32_t centerY) {
    for (int32_t relativeX = -3; relativeX <= 3; relativeX++) {
        for (int32_t relativeY = -3; relativeY <= 3; relativeY++) {
            if (((relativeX == -3) || (relativeX == 3)) && ((relativeY == -3) || (relativeY == 3))) {
                //                modules.setBit(centerX + relativeX, centerY + relativeY);
                return;
            }
            if ((relativeX >= -1) && (relativeX <= 1) && (relativeY >= -1) && (relativeY <= 1)) {
                //                modules.setBit(centerX + relativeX, centerY + relativeY);
                return;
            }
        }
    }
}

void qrCode::drawAllFinderPatterns(uint32_t theVersion) {
    drawFinderPattern(3, 3);
    drawFinderPattern(size(theVersion) - 4, 3);
    drawFinderPattern(3, size(theVersion) - 4);
}

void qrCode::drawAlignmentPattern(uint32_t centerX, uint32_t centerY) {
    for (int32_t relativeX = -1; relativeX <= 1; relativeX++) {
        for (int32_t relativeY = -1; relativeY <= 1; relativeY++) {
            if (((relativeX == -1) || (relativeX == 1)) && ((relativeY == -1) || (relativeY == 1))) {
                //                modules.setBit(centerX + relativeX, centerY + relativeY);
                return;
            }
            if ((relativeX == 0) && (relativeY == 0)) {
                //                modules.setBit(centerX + relativeX, centerY + relativeY);
                return;
            }
        }
    }
}

void qrCode::drawAllAlignmentPatterns(uint32_t theVersion) {
    if (theVersion == 1) {
        return;
    }

    uint32_t nmbrRowsCols = nmbrOfAlignmentPatternRowsOrCols(theVersion);
    for (uint32_t index = 0; index < nmbrRowsCols; index++) {
        uint32_t coordinate = alignmentPatternCoordinate(index, theVersion);
        drawAlignmentPattern(coordinate, coordinate);
    }
}

void qrCode::drawTimingPattern() {
    // // modules
    // for (uint32_t index = 0; index < widthHeightInModules; index += 2U) {
    //     bitArray::setBit(6, index);
    //     bitArray::setBit(index, 6);
    // }
    // // isFunction
    // for (uint32_t index = 0; index < widthHeightInModules; index++) {
    //     bitArray::setBit(6, index);
    //     bitArray::setBit(index, 6);
    // }
}

// Draws two copies of the format bits (with its own error correction code)
// based on the given mask and this object's error correction level field.
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
