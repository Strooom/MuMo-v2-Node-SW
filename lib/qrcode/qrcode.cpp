#include <qrcode.hpp>
#include <reedsolomon.hpp>
#include <stdlib.h>
#include <cstring>

// This code is using the vocabulary of the QR Code standard ISO/IEC 18004:2006
// See qrcode.md for a short explanation

uint32_t qrCode::theVersion{0};        // 0 is an invalid value and indicates the version has not yet been set
errorCorrectionLevel qrCode::theErrorCorrectionLevel{errorCorrectionLevel::low};
encodingFormat qrCode::theEncodingFormat{encodingFormat::numeric};
bitVector<qrCode::maxPayloadLengthInBytes * 8U> qrCode::buffer;
bitMatrix<qrCode::maxSize> qrCode::modules;
bitMatrix<qrCode::maxSize> qrCode::isData;

#pragma region publicAPI

uint32_t qrCode::versionNeeded(const char *data, errorCorrectionLevel wantedErrorCorrectionLevel) {
    uint32_t dataLength = strnlen(data, maxInputLength);
    return versionNeeded(reinterpret_cast<const uint8_t *>(data), dataLength, wantedErrorCorrectionLevel);
}

uint32_t qrCode::versionNeeded(const uint8_t *data, uint32_t dataLengthInBytes, errorCorrectionLevel wantedErrorCorrectionLevel) {
    encodingFormat neededEncodingFormat = getEncodingFormat(data, dataLengthInBytes);

    for (uint32_t candidateVersion = 1; candidateVersion <= maxVersion; candidateVersion++) {
        uint32_t totalAvailableBits;
        uint32_t bitsNeededForErrorCorrection;
        uint32_t bitsNeededForPayload;

        bitsNeededForPayload         = payloadLengthInBits(dataLengthInBytes, candidateVersion, neededEncodingFormat);
        bitsNeededForErrorCorrection = versionProperties[candidateVersion - 1].nmbrErrorCorrectionCodewordsPerBlock[static_cast<uint32_t>(wantedErrorCorrectionLevel)] * versionProperties[candidateVersion - 1].nmbrBlocks[static_cast<uint32_t>(wantedErrorCorrectionLevel)] * 8;
        totalAvailableBits           = nmbrOfDataModules(candidateVersion);
        if (bitsNeededForPayload + bitsNeededForErrorCorrection <= totalAvailableBits) {
            return candidateVersion;
        }
    }
    return 0;        // no (valid) version found which accommodates the amount of data with this encodingFormat and with the requested error correction level
}

#pragma endregion
#pragma region encodingPayload

void qrCode::encodeData(const char *data) {
    uint32_t length = strnlen(data, maxInputLength);
    encodeData(reinterpret_cast<const uint8_t *>(data), length);
}

void qrCode::encodeData(const uint8_t *data, uint32_t dataLength) {
    theEncodingFormat = getEncodingFormat(data, dataLength);
    buffer.reset();
    buffer.appendBits(modeIndicator(theEncodingFormat), 4);
    buffer.appendBits(dataLength, characterCountIndicatorLength(theVersion, theEncodingFormat));

    switch (theEncodingFormat) {
        case encodingFormat::numeric:
            encodeNumeric(data, dataLength);
            break;

        case encodingFormat::alphanumeric:
            encodeAlfaNumeric(data, dataLength);
            break;

        case encodingFormat::byte:        // intentional fallthrough
        default:
            encodeByte(data, dataLength);
            break;
    }

    addTerminator();
    addBitPadding();
    addBytePadding();
}

void qrCode::addTerminator() {
    // add terminator : up to four zero bits
    uint32_t nmbrOfTerminatorBits{versionProperties[theVersion - 1].availableDataCodeWords[static_cast<uint8_t>(theErrorCorrectionLevel)] * 8 - buffer.levelInBits()};
    if (nmbrOfTerminatorBits > 4) {
        nmbrOfTerminatorBits = 4;
    }
    buffer.appendBits(0, nmbrOfTerminatorBits);
}

void qrCode::addBitPadding() {
    // add bit padding : add zeros to make the length of the data a multiple of 8
    uint32_t nmbrOfBytePaddingBits{8U - (buffer.levelInBits() % 8U)};
    if (nmbrOfBytePaddingBits != 8) {
        buffer.appendBits(0, nmbrOfBytePaddingBits);
    }
}

void qrCode::addBytePadding() {
    // add byte padding : fill with 0xEC and 0x11 until available space is filled
    while (true) {
        if (buffer.levelInBytes() < versionProperties[theVersion - 1].availableDataCodeWords[static_cast<uint8_t>(theErrorCorrectionLevel)]) {
            buffer.appendByte(0xEC);
        } else {
            return;
        }
        if (buffer.levelInBytes() < versionProperties[theVersion - 1].availableDataCodeWords[static_cast<uint8_t>(theErrorCorrectionLevel)]) {
            buffer.appendByte(0x11);
        } else {
            return;
        }
    }
}

void qrCode::encodeNumeric(const uint8_t *data, uint32_t dataLength) {
    uint32_t accumData  = 0;
    uint32_t accumCount = 0;
    for (uint32_t i = 0; i < dataLength; i++) {
        accumData = accumData * 10 + compressNumeric(data[i]);
        accumCount++;
        if (accumCount == 3) {
            buffer.appendBits(accumData, 10U);
            accumData  = 0;
            accumCount = 0;
        }
    }
    if (accumCount > 0) {
        buffer.appendBits(accumData, (accumCount * 3U) + 1U);
    }
}

void qrCode::encodeAlfaNumeric(const uint8_t *data, uint32_t dataLength) {
    uint32_t accumData  = 0;
    uint32_t accumCount = 0;
    for (uint32_t i = 0; i < dataLength; i++) {
        accumData = accumData * 45 + compressAlphanumeric(data[i]);
        accumCount++;
        if (accumCount == 2) {
            buffer.appendBits(accumData, 11U);
            accumData  = 0;
            accumCount = 0;
        }
    }
    if (accumCount > 0) {
        buffer.appendBits(accumData, 6U);
    }
}

void qrCode::encodeByte(const uint8_t *data, uint32_t dataLength) {
    for (uint32_t i = 0; i < dataLength; i++) {
        buffer.appendBits(data[i], 8U);
    }
}

encodingFormat qrCode::getEncodingFormat(const char *data) {
    if (isNumeric(data)) {
        return encodingFormat::numeric;
    }
    if (isAlphanumeric(data)) {
        return encodingFormat::alphanumeric;
    }
    return encodingFormat::byte;
}

encodingFormat qrCode::getEncodingFormat(const uint8_t *data, uint32_t dataLength) {
    if (isNumeric(data, dataLength)) {
        return encodingFormat::numeric;
    }
    if (isAlphanumeric(data, dataLength)) {
        return encodingFormat::alphanumeric;
    }
    return encodingFormat::byte;
}

bool qrCode::isNumeric(const uint8_t data) {
    return (data >= '0' && data <= '9');
}

bool qrCode::isNumeric(const char *data) {
    uint32_t length = strnlen(data, maxInputLength);
    for (uint32_t i = 0; i < length; i++) {
        if (!isNumeric(data[i])) {
            return false;
        }
    }
    return true;
}

bool qrCode::isNumeric(const uint8_t *data, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        if (!isNumeric(data[i])) {
            return false;
        }
    }
    return true;
}

bool qrCode::isAlphanumeric(const uint8_t data) {
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
    uint32_t length = strnlen(data, maxInputLength);
    for (uint32_t i = 0; i < length; i++) {
        if (!isAlphanumeric(data[i])) {
            return false;
        }
    }
    return true;
}

bool qrCode::isAlphanumeric(const uint8_t *data, uint32_t length) {
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

#pragma endregion

#pragma region errorCorrection

uint32_t qrCode::nmbrBlocks(uint32_t someVersion, errorCorrectionLevel someErrorCorrectionLevel) {
    return versionProperties[someVersion - 1].nmbrBlocks[static_cast<uint8_t>(someErrorCorrectionLevel)];
}

uint32_t qrCode::blockLength(uint32_t blockIndex, uint32_t someVersion, errorCorrectionLevel someErrorCorrectionLevel) {
    if (blockIndex >= nmbrBlocks(someVersion, someErrorCorrectionLevel)) {
        return 0;
    }
    if (blockIndex < nmbrBlocksGroup1(someVersion, someErrorCorrectionLevel)) {
        return blockLengthGroup1(someVersion, someErrorCorrectionLevel);
    } else {
        return blockLengthGroup2(someVersion, someErrorCorrectionLevel);
    }
}

uint32_t qrCode::blockOffset(uint32_t blockIndex, uint32_t someVersion, errorCorrectionLevel someErrorCorrectionLevel) {
    if (blockIndex >= nmbrBlocks(someVersion, someErrorCorrectionLevel)) {
        return 0;
    }
    if (blockIndex < nmbrBlocksGroup1(someVersion, someErrorCorrectionLevel)) {
        return blockLengthGroup1(someVersion, someErrorCorrectionLevel) * blockIndex;
    } else {
        uint32_t offsetGroup2 = blockLengthGroup1(someVersion, someErrorCorrectionLevel) * nmbrBlocksGroup1(someVersion, someErrorCorrectionLevel);
        uint32_t indexGroup2  = blockIndex - nmbrBlocksGroup1(someVersion, someErrorCorrectionLevel);
        return offsetGroup2 + indexGroup2 * blockLengthGroup2(someVersion, someErrorCorrectionLevel);
    }
}

uint32_t qrCode::nmbrBlocksGroup1(uint32_t someVersion, errorCorrectionLevel someErrorCorrectionLevel) {
    return (nmbrBlocks(someVersion, someErrorCorrectionLevel) * blockLengthGroup2(someVersion, someErrorCorrectionLevel)) - versionProperties[someVersion - 1].availableDataCodeWords[static_cast<uint8_t>(someErrorCorrectionLevel)];
}

uint32_t qrCode::nmbrBlocksGroup2(uint32_t someVersion, errorCorrectionLevel someErrorCorrectionLevel) {
    return nmbrBlocks(someVersion, someErrorCorrectionLevel) - nmbrBlocksGroup1(someVersion, someErrorCorrectionLevel);
}

uint32_t qrCode::blockLengthGroup1(uint32_t someVersion, errorCorrectionLevel someErrorCorrectionLevel) {
    return versionProperties[someVersion - 1].availableDataCodeWords[static_cast<uint8_t>(someErrorCorrectionLevel)] / nmbrBlocks(someVersion, someErrorCorrectionLevel);
}

uint32_t qrCode::blockLengthGroup2(uint32_t someVersion, errorCorrectionLevel someErrorCorrectionLevel) {
    return blockLengthGroup1(someVersion, someErrorCorrectionLevel) + 1;
}

#pragma endregion

#pragma region drawing

bool qrCode::isDataModule(uint32_t x, uint32_t y, uint32_t someVersion) {
    if (isFinderPatternOrSeparator(x, y, someVersion)) {
        return false;
    }
    if (isTimingPattern(x, y)) {
        return false;
    }
    if (isAlignmentPattern(x, y, someVersion)) {
        return false;
    }
    if (isFormatInformation(x, y, someVersion)) {
        return false;
    }
    if (isVersionInformation(x, y, someVersion)) {
        return false;
    }
    return true;
}

bool qrCode::isFinderPatternOrSeparator(uint32_t x, uint32_t y, uint32_t someVersion) {
    if (x <= 7 && y <= 7) {
        return true;
    }
    if (x <= 7 && y >= size(someVersion) - 8) {
        return true;
    }
    if (x >= size(someVersion) - 8 && y <= 7) {
        return true;
    }
    return false;
}

bool qrCode::isTimingPattern(uint32_t x, uint32_t y) {
    if (x == 6 || y == 6) {
        return true;
    }
    return false;
}

bool qrCode::isDarkModule(uint32_t x, uint32_t y, uint32_t someVersion) {
    if ((x == 4 * someVersion + 9) && (y == 8)) {
        return true;
    }
    return false;
}

bool qrCode::isAlignmentPattern(uint32_t x, uint32_t y, uint32_t someVersion) {
    if (someVersion == 1) {
        return false;
    }
    uint32_t nmbrRowsCols = nmbrOfAlignmentPatternRowsOrCols(someVersion);
    for (uint32_t index = 0; index < nmbrRowsCols; index++) {
        uint32_t coordinate = alignmentPatternCoordinate(someVersion, index);
        if (((x >= coordinate - 2) && (x <= coordinate + 2)) && ((y >= coordinate - 2) && (y <= coordinate + 2))) {
            return true;
        }
    }
    return false;
}

bool qrCode::isFormatInformation(uint32_t x, uint32_t y, uint32_t someVersion) {
    // TODO : remove some overlap with other functions, eg timing pattern and dark module
    if ((y == 8) && (x <= 8)) {
        return true;
    }
    if ((y == 8) && (x > (size(someVersion) - 8))) {
        return true;
    }
    if ((x == 8) && (y <= 8)) {
        return true;
    }
    if ((x == 8) && (x > (size(someVersion) - 8))) {
        return true;
    }
    return false;
}

bool qrCode::isVersionInformation(uint32_t x, uint32_t y, uint32_t someVersion) {
    if (someVersion < 7) {
        return false;
    }
    if ((x < 6) && (y >= size(someVersion) - 11) && (y <= size(someVersion) - 9)) {
        return true;
    }
    if ((y < 6) && (x >= size(someVersion) - 11) && (x <= size(someVersion) - 9)) {
        return true;
    }
    return false;
}

void qrCode::drawFinderPattern(uint32_t centerX, uint32_t centerY) {
    for (int32_t relativeX = -3; relativeX <= 3; relativeX++) {
        for (int32_t relativeY = -3; relativeY <= 3; relativeY++) {
            if (((relativeX == -3) || (relativeX == 3)) && ((relativeY == -3) || (relativeY == 3))) {
                modules.setBit(centerX + relativeX, centerY + relativeY);
                return;
            }
            if ((relativeX >= -1) && (relativeX <= 1) && (relativeY >= -1) && (relativeY <= 1)) {
                modules.setBit(centerX + relativeX, centerY + relativeY);
                return;
            }
        }
    }
}

void qrCode::drawAllFinderPatterns(uint32_t someVersion) {
    drawFinderPattern(3, 3);
    drawFinderPattern(size(someVersion) - 4, 3);
    drawFinderPattern(3, size(someVersion) - 4);
}

void qrCode::drawAlignmentPattern(uint32_t centerX, uint32_t centerY, uint32_t someVersion) {
    for (int32_t relativeX = -1; relativeX <= 1; relativeX++) {
        for (int32_t relativeY = -1; relativeY <= 1; relativeY++) {
            if (((relativeX == -1) || (relativeX == 1)) && ((relativeY == -1) || (relativeY == 1))) {
                modules.setBit(centerX + relativeX, centerY + relativeY);
                return;
            }
            if ((relativeX == 0) && (relativeY == 0)) {
                modules.setBit(centerX + relativeX, centerY + relativeY);
                return;
            }
        }
    }
}

void qrCode::drawAllAlignmentPatterns(uint32_t someVersion) {
    if (someVersion == 1) {
        return;
    }
    uint32_t nmbrRowsCols = nmbrOfAlignmentPatternRowsOrCols(someVersion);
    for (uint32_t index = 0; index < nmbrRowsCols; index++) {
        uint32_t coordinate = alignmentPatternCoordinate(someVersion, index);
        drawAlignmentPattern(coordinate, coordinate, someVersion);
    }
}

void qrCode::drawTimingPattern(uint32_t someVersion) {
    for (uint32_t index = 0; index < size(someVersion); index += 2U) {
        modules.setBit(6, index);
        modules.setBit(index, 6);
    }
}

void qrCode::drawFormatInfo(uint32_t theVersion) {
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
}

void qrCode::drawVersionInfo(uint32_t theVersion) {
    if (theVersion < 7) {
        return;
    }

    // // Calculate error correction code and pack bits
    // uint32_t rem = version;        // version is uint6, in the range [7, 40]
    // for (uint8_t i = 0; i < 12; i++) {
    //     rem = (rem << 1) ^ ((rem >> 11) * 0x1F25);
    // }

    // uint32_t data = version << 12 | rem;        // uint18

    // for (uint8_t i = 0; i < 18; i++) {
    //     bool bit  = ((data >> i) & 1) != 0;
    //     uint8_t a = size(theVersion) - 11 + i % 3;
    //     uint8_t b = i / 3;
    //     modules.setOrClearBit(a, b, bit);
    //     modules.setOrClearBit(b, a, bit);
    // }
}

void qrCode::applyMask(uint8_t maskType) {
    // for (uint8_t y = 0; y < size; y++) {
    //     for (uint8_t x = 0; x < size; x++) {
    //         if (!isDataModule(x, y)) {
    //             continue;
    //         }
    //         bool invert{false};
    //         switch (maskType) {
    //             case 0:
    //                 invert = (x + y) % 2 == 0;
    //                 break;
    //             case 1:
    //                 invert = y % 2 == 0;
    //                 break;
    //             case 2:
    //                 invert = x % 3 == 0;
    //                 break;
    //             case 3:
    //                 invert = (x + y) % 3 == 0;
    //                 break;
    //             case 4:
    //                 invert = (x / 3 + y / 2) % 2 == 0;
    //                 break;
    //             case 5:
    //                 invert = x * y % 2 + x * y % 3 == 0;
    //                 break;
    //             case 6:
    //                 invert = (x * y % 2 + x * y % 3) % 2 == 0;
    //                 break;
    //             case 7:
    //                 invert = ((x + y) % 2 + x * y % 3) % 2 == 0;
    //                 break;
    //         }
    //         if (invert) {
    //             modules.invertBit(x, y);
    //         }
    //     }
    // }
}

#pragma endregion

uint32_t qrCode::size(uint32_t someVersion) { return 17 + 4 * someVersion; }

uint32_t qrCode::payloadLengthInBits(uint32_t dataLengthInBytes, uint32_t someVersion, encodingFormat theEncodingFormat) {
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
            return modeIndicatorLength + characterCountIndicatorLength(someVersion, theEncodingFormat) + (10 * (dataLengthInBytes / 3)) + remainderLength;
        case encodingFormat::alphanumeric:
            return modeIndicatorLength + characterCountIndicatorLength(someVersion, theEncodingFormat) + (11 * (dataLengthInBytes / 2)) + (6 * (dataLengthInBytes % 2));
        case encodingFormat::byte:        // intentional fallthrough
        default:
            return modeIndicatorLength + characterCountIndicatorLength(someVersion, theEncodingFormat) + (8 * dataLengthInBytes);
    }
}

uint32_t qrCode::characterCountIndicatorLength(uint32_t someVersion, encodingFormat theEncodingFormat) {
    if (someVersion < 10) {
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
    if (someVersion < 27) {
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

uint32_t qrCode::nmbrOfDataModules(uint32_t someVersion) {
    return nmbrOfTotalModules(someVersion) - nmbrOfFunctionModules(someVersion);
}

void qrCode::setVersion(uint32_t newVersion) {
    if (theVersion <= maxVersion) {
        theVersion = newVersion;
    }
}

void qrCode::setErrorCorrectionLevel(errorCorrectionLevel newErrorCorrectionLevel) { theErrorCorrectionLevel = newErrorCorrectionLevel; }

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

uint32_t qrCode::nmbrOfTotalModules(uint32_t someVersion) {
    return size(someVersion) * size(someVersion);
}

uint32_t qrCode::nmbrOfFunctionModules(uint32_t someVersion) {
    uint32_t count{0};
    count += 3 * 64;                                           // Finder patterns including separators
    count += 2 * ((4 * someVersion) + 1);                      // Timing patterns
    count += 1;                                                // Dark module
    count += nmbrOfAlignmentPatterns(someVersion) * 25;        // Alignment patterns
    if (nmbrOfAlignmentPatternRowsOrCols(someVersion) > 2) {
        count -= 2 * ((nmbrOfAlignmentPatternRowsOrCols(someVersion) - 2) * 5);        // overlap of timing and alignment
    }
    count += (6U + 6U + 3U + 8U + 7U);        // Format information
    if (someVersion >= 7) {
        count += (2U * 6U * 3U);        // version information
    }
    return count;
}

uint32_t qrCode::nmbrOfErrorCorrectionModules(uint32_t someVersion, errorCorrectionLevel theErrorCorrectionLevel) {
    return versionProperties[someVersion - 1].nmbrErrorCorrectionCodewordsPerBlock[static_cast<uint32_t>(theErrorCorrectionLevel)] * versionProperties[someVersion - 1].nmbrBlocks[static_cast<uint32_t>(theErrorCorrectionLevel)] * 8;
}

uint32_t qrCode::nmbrOfAlignmentPatterns(uint32_t someVersion) {
    if (someVersion == 1) {
        return 0;
    } else {
        return (nmbrOfAlignmentPatternRowsOrCols(someVersion) * nmbrOfAlignmentPatternRowsOrCols(someVersion)) - 3;
    }
}

uint32_t qrCode::nmbrOfAlignmentPatternRowsOrCols(uint32_t someVersion) {
    if (someVersion == 1) {
        return 0;
    }
    return ((someVersion / 7U) + 2);
}

uint32_t qrCode::alignmentPatternSpacing(uint32_t someVersion) {
    if (someVersion != 32U) {
        return (someVersion * 4U + (nmbrOfAlignmentPatternRowsOrCols(someVersion) * 2U) + 1U) / ((2U * nmbrOfAlignmentPatternRowsOrCols(someVersion)) - 2U) * 2U;
    } else {
        return 26;
    }
}

uint32_t qrCode::alignmentPatternCoordinate(uint32_t someVersion, uint32_t index) {
    uint32_t maxIndex = nmbrOfAlignmentPatternRowsOrCols(someVersion) - 1;
    if (index > maxIndex) {
        return 0;
    }
    if (index == 0) {
        return 6U;
    } else {
        return size(someVersion) - 7 - (alignmentPatternSpacing(someVersion) * (maxIndex - index));
    }
}

uint32_t qrCode::getPenaltyScore() {
    uint32_t result = 0;

    // uint8_t size = modules->bitOffsetOrWidth;

    // // Adjacent modules in row having same color
    // for (uint8_t y = 0; y < size; y++) {
    //     bool colorX = bb_getBit(modules, 0, y);
    //     for (uint8_t x = 1, runX = 1; x < size; x++) {
    //         bool cx = bb_getBit(modules, x, y);
    //         if (cx != colorX) {
    //             colorX = cx;
    //             runX   = 1;

    //         } else {
    //             runX++;
    //             if (runX == 5) {
    //                 result += PENALTY_N1;
    //             } else if (runX > 5) {
    //                 result++;
    //             }
    //         }
    //     }
    // }

    // // Adjacent modules in column having same color
    // for (uint8_t x = 0; x < size; x++) {
    //     bool colorY = bb_getBit(modules, x, 0);
    //     for (uint8_t y = 1, runY = 1; y < size; y++) {
    //         bool cy = bb_getBit(modules, x, y);
    //         if (cy != colorY) {
    //             colorY = cy;
    //             runY   = 1;
    //         } else {
    //             runY++;
    //             if (runY == 5) {
    //                 result += PENALTY_N1;
    //             } else if (runY > 5) {
    //                 result++;
    //             }
    //         }
    //     }
    // }

    // uint16_t black = 0;
    // for (uint8_t y = 0; y < size; y++) {
    //     uint16_t bitsRow = 0, bitsCol = 0;
    //     for (uint8_t x = 0; x < size; x++) {
    //         bool color = bb_getBit(modules, x, y);

    //         // 2*2 blocks of modules having same color
    //         if (x > 0 && y > 0) {
    //             bool colorUL = bb_getBit(modules, x - 1, y - 1);
    //             bool colorUR = bb_getBit(modules, x, y - 1);
    //             bool colorL  = bb_getBit(modules, x - 1, y);
    //             if (color == colorUL && color == colorUR && color == colorL) {
    //                 result += PENALTY_N2;
    //             }
    //         }

    //         // Finder-like pattern in rows and columns
    //         bitsRow = ((bitsRow << 1) & 0x7FF) | color;
    //         bitsCol = ((bitsCol << 1) & 0x7FF) | bb_getBit(modules, y, x);

    //         // Needs 11 bits accumulated
    //         if (x >= 10) {
    //             if (bitsRow == 0x05D || bitsRow == 0x5D0) {
    //                 result += PENALTY_N3;
    //             }
    //             if (bitsCol == 0x05D || bitsCol == 0x5D0) {
    //                 result += PENALTY_N3;
    //             }
    //         }

    //         // Balance of black and white modules
    //         if (color) {
    //             black++;
    //         }
    //     }
    // }

    // // Find smallest k such that (45-5k)% <= dark/total <= (55+5k)%
    // uint16_t total = size * size;
    // for (uint16_t k = 0; black * 20 < (9 - k) * total || black * 20 > (11 + k) * total; k++) {
    //     result += PENALTY_N4;
    // }

    return result;
}

void qrCode::addErrorCorrection() {
    uint32_t nmbrOfBlocks     = nmbrBlocks(theVersion, theErrorCorrectionLevel);
    uint32_t protectionLength = versionProperties[theVersion - 1].nmbrErrorCorrectionCodewordsPerBlock[static_cast<uint32_t>(theErrorCorrectionLevel)];
    uint8_t ecc[protectionLength];
    for (uint32_t blockIndex = 0; blockIndex < nmbrOfBlocks; blockIndex++) {
        uint32_t theBlockLength = blockLength(blockIndex, theVersion, theErrorCorrectionLevel);
        uint8_t message[theBlockLength];
        for (uint32_t i = 0; i < theBlockLength; i++) {
            message[i] = buffer.getByte(i + blockOffset(blockIndex, theVersion, theErrorCorrectionLevel));
        }
        reedSolomon::getErrorCorrectionBytes(ecc, protectionLength, message, theBlockLength);
        for (uint32_t i = 0; i < protectionLength; i++) {
            buffer.appendByte(ecc[i]);
        }
    }
}

void qrCode::interleaveData() {
    if (theVersion > 4) {
        uint8_t tempBuffer[maxPayloadLengthInBytes]{};        // build interleaved data in tempBuffer, then copy back to buffer
        uint32_t sourceOffset{0};
        uint32_t destinationOffset{0};
        // iterate over all data blocks

        // iterate over all error correction blocks
        buffer.reset();
        for (uint32_t i = 0; i < maxPayloadLengthInBytes; i++) {
            buffer.appendByte(tempBuffer[i]);
        }
    }
}

void qrCode::drawPatterns(uint32_t theVersion) {
}

void qrCode::drawPayload(uint32_t theVersion) {
}
