#include <qrcode.hpp>
#include <reedsolomon.hpp>
#include <stdlib.h>
#include <cstring>

uint32_t qrCode::theVersion{0};        // 0 is an invalid value and indicates the version has not yet been set
errorCorrectionLevel qrCode::theErrorCorrectionLevel{errorCorrectionLevel::low};
encodingFormat qrCode::theEncodingFormat{encodingFormat::numeric};
uint32_t qrCode::theMask;
bitVector<qrCode::maxPayloadLengthInBytes * 8U> qrCode::buffer;
bitMatrix<qrCode::maxSize> qrCode::modules;
bitMatrix<qrCode::maxSize> qrCode::isData;

#pragma region public API

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

errorCorrectionLevel qrCode::errorCorrectionLevelPossible(const char *data, uint32_t someVersion) {
    uint32_t dataLength = strnlen(data, maxInputLength);
    return errorCorrectionLevelPossible(reinterpret_cast<const uint8_t *>(data), dataLength, someVersion);
}

errorCorrectionLevel qrCode::errorCorrectionLevelPossible(const uint8_t *data, uint32_t dataLengthInBytes, uint32_t someVersion) {
    if (versionNeeded(data, dataLengthInBytes, errorCorrectionLevel::high) == someVersion) {
        return errorCorrectionLevel::high;
    }
    if (versionNeeded(data, dataLengthInBytes, errorCorrectionLevel::quartile) == someVersion) {
        return errorCorrectionLevel::quartile;
    }
    if (versionNeeded(data, dataLengthInBytes, errorCorrectionLevel::medium) == someVersion) {
        return errorCorrectionLevel::medium;
    }
    return errorCorrectionLevel::low;
}

void qrCode::generate(const char *data, uint32_t someVersion, errorCorrectionLevel someErrorCorrectionLevel) {
    uint32_t dataLength = strnlen(data, maxInputLength);
    generate(reinterpret_cast<const uint8_t *>(data), dataLength, someVersion, someErrorCorrectionLevel);
}

void qrCode::generate(const uint8_t *data, uint32_t dataLength, uint32_t someVersion, errorCorrectionLevel someErrorCorrectionLevel) {
    initialize(someVersion, someErrorCorrectionLevel);
    encodeData(data, dataLength);
    addErrorCorrection();
    interleaveData();
    drawPatterns(someVersion);
    drawPayload(someVersion);
    selectMask();
}

bool qrCode::getModule(uint32_t x, uint32_t y) {
    return modules.getBit(x, y);
}

#pragma endregion
#pragma region general helpers

uint32_t qrCode::size(uint32_t someVersion) { return 17 + 4 * someVersion; }

void qrCode::initialize(uint32_t newVersion, errorCorrectionLevel newErrorCorrectionLevel) {
    if ((newVersion <= maxVersion) && (newVersion > 0)) {
        theVersion = newVersion;
        buffer.reset();
    }
    theErrorCorrectionLevel = newErrorCorrectionLevel;
    modules.setWidthHeightInBits(size(theVersion));
    modules.clearAllBits();
    isData.setWidthHeightInBits(size(theVersion));
    isData.setAllBits();
}

#pragma endregion
#pragma region encodingPayload

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

uint32_t qrCode::characterCountIndicatorLength(uint32_t someVersion, encodingFormat someEncodingFormat) {
    if (someVersion < 10) {
        switch (someEncodingFormat) {
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
        switch (someEncodingFormat) {
            case encodingFormat::numeric:
                return 12;
            case encodingFormat::alphanumeric:
                return 11;
            case encodingFormat::byte:        // intentional fallthrough
            default:
                return 16;
        }
    }
    switch (someEncodingFormat) {
        case encodingFormat::numeric:
            return 14;
        case encodingFormat::alphanumeric:
            return 13;
        case encodingFormat::byte:        // intentional fallthrough
        default:
            return 16;
    }
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

void qrCode::encodeData(const char *data) {
    uint32_t length = strnlen(data, maxInputLength);
    encodeData(reinterpret_cast<const uint8_t *>(data), length);
}

void qrCode::encodeData(const uint8_t *data, uint32_t dataLength) {
    theEncodingFormat = getEncodingFormat(data, dataLength);
    buffer.reset();
    static constexpr uint8_t modeIndicator[nmbrOfEncodingFormats] = {0b0001, 0b0010, 0b0100};
    buffer.appendBits(modeIndicator[static_cast<uint32_t>(theEncodingFormat)], 4);
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

uint32_t qrCode::dataOffset(uint32_t blockIndex, uint32_t rowIndex) {
    uint32_t result{0};
    for (uint32_t block = 0; block < blockIndex; block++) {
        result += blockLength(block, theVersion, theErrorCorrectionLevel);
    }
    result += rowIndex;
    return result;
}

uint32_t qrCode::eccOffset(uint32_t blockIndex, uint32_t rowIndex) {
    uint32_t result{versionProperties[theVersion - 1].availableDataCodeWords[static_cast<uint32_t>(theErrorCorrectionLevel)]};
    result += blockIndex * versionProperties[theVersion - 1].nmbrErrorCorrectionCodewordsPerBlock[static_cast<uint32_t>(theErrorCorrectionLevel)];
    result += rowIndex;
    return result;
}

void qrCode::addErrorCorrection() {
    uint32_t nmbrOfBlocks     = nmbrBlocks(theVersion, theErrorCorrectionLevel);
    uint32_t protectionLength = versionProperties[theVersion - 1].nmbrErrorCorrectionCodewordsPerBlock[static_cast<uint32_t>(theErrorCorrectionLevel)];
    uint8_t ecc[protectionLength];
    for (uint32_t blockIndex = 0; blockIndex < nmbrOfBlocks; blockIndex++) {
        uint32_t theBlockLength = blockLength(blockIndex, theVersion, theErrorCorrectionLevel);
        if (theBlockLength > 0) {
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
}

void qrCode::interleaveData() {
    if (nmbrBlocks(theVersion, theErrorCorrectionLevel) > 1) {
        uint32_t lastBlockIndex             = nmbrBlocks(theVersion, theErrorCorrectionLevel) - 1;
        uint32_t longestBlockLength         = blockLength(lastBlockIndex, theVersion, theErrorCorrectionLevel);
        uint32_t nmbrErrorCodewordsPerBlock = versionProperties[theVersion - 1].nmbrErrorCorrectionCodewordsPerBlock[static_cast<uint32_t>(theErrorCorrectionLevel)];
        uint8_t tempBuffer[256U]{};
        uint32_t destinationIndex{0};

        // iterate over all data blocks
        for (uint32_t rowIndex = 0; rowIndex < longestBlockLength; rowIndex++) {
            for (uint32_t blockIndex = 0; blockIndex <= lastBlockIndex; blockIndex++) {
                if (rowIndex < blockLength(blockIndex, theVersion, theErrorCorrectionLevel)) {
                    tempBuffer[destinationIndex] = buffer.getByte(dataOffset(blockIndex, rowIndex));
                    destinationIndex++;
                }
            }
        }
        // iterate over all error correction blocks
        for (uint32_t rowIndex = 0; rowIndex < nmbrErrorCodewordsPerBlock; rowIndex++) {
            for (uint32_t blockIndex = 0; blockIndex <= lastBlockIndex; blockIndex++) {
                tempBuffer[destinationIndex] = buffer.getByte(eccOffset(blockIndex, rowIndex));
                destinationIndex++;
            }
        }

        // Copy back to source buffer
        buffer.reset();
        for (uint32_t i = 0; i < maxPayloadLengthInBytes; i++) {
            buffer.appendByte(tempBuffer[i]);
        }
    }
}

#pragma endregion
#pragma region drawing

void qrCode::drawFinderPattern(uint32_t centerX, uint32_t centerY) {
    for (int32_t relativeY = -3; relativeY <= 3; relativeY++) {
        for (int32_t relativeX = -3; relativeX <= 3; relativeX++) {
            if ((relativeX == -3) || (relativeX == 3) || (relativeY == -3) || (relativeY == 3)) {
                modules.setBit(centerX + relativeX, centerY + relativeY);
            }
            if ((relativeX >= -1) && (relativeX <= 1) && (relativeY >= -1) && (relativeY <= 1)) {
                modules.setBit(centerX + relativeX, centerY + relativeY);
            }
            isData.clearBit(centerX + relativeX, centerY + relativeY);
        }
    }
}

void qrCode::drawFinderSeparators(uint32_t someVersion) {
    uint32_t end = size(someVersion) - 1;
    for (uint32_t index = 0; index < 8; index++) {
        isData.clearBit(index, 7);              // bottom of top-left finder pattern
        isData.clearBit(7, index);              // right of top-left finder pattern
        isData.clearBit(end - 7, index);        // left of top-right finder pattern
        isData.clearBit(end - index, 7);        // bottom of top-right finder pattern
        isData.clearBit(index, end - 7);        // top of bottom-left finder pattern
        isData.clearBit(7, end - index);        // right of bottom-left finder pattern
    }
}

void qrCode::drawAllFinderPatternsAndSeparators(uint32_t someVersion) {
    drawFinderPattern(3, 3);
    drawFinderPattern(size(someVersion) - 4, 3);
    drawFinderPattern(3, size(someVersion) - 4);
    drawFinderSeparators(someVersion);
}

void qrCode::drawDummyFormatBits(uint32_t someVersion) {
    uint32_t endCoordinate = size(someVersion) - 1;
    for (uint32_t coordinate = 0; coordinate < 6; coordinate++) {
        isData.clearBit(coordinate, 8);        // bottom of top-left finder pattern
        isData.clearBit(8, coordinate);        // right of top-left finder pattern
    }
    isData.clearBit(8, 8);        // corner of top-left finder pattern
    isData.clearBit(7, 8);        // corner of top-left finder pattern
    isData.clearBit(8, 7);        // corner of top-left finder pattern
    for (uint32_t xCoordinate = (endCoordinate - 7); xCoordinate <= endCoordinate; xCoordinate++) {
        isData.clearBit(xCoordinate, 8);        // bottom of top-right finder pattern
    }
    for (uint32_t yCoordinate = (endCoordinate - 6); yCoordinate <= endCoordinate; yCoordinate++) {
        isData.clearBit(8, yCoordinate);        // right of bottom-left finder pattern
    }
}

void qrCode::drawDarkModule(uint32_t someVersion) {
    modules.setBit(8, size(someVersion) - 8);
    isData.clearBit(8, size(someVersion) - 8);
}

void qrCode::drawAlignmentPattern(uint32_t centerX, uint32_t centerY) {
    for (int32_t relativeY = -2; relativeY <= 2; relativeY++) {
        for (int32_t relativeX = -2; relativeX <= 2; relativeX++) {
            if ((relativeX == -2) || (relativeX == 2) || (relativeY == -2) || (relativeY == 2)) {
                modules.setBit(centerX + relativeX, centerY + relativeY);
            }
            if ((relativeX == 0) && (relativeY == 0)) {
                modules.setBit(centerX + relativeX, centerY + relativeY);
            }
            isData.clearBit(centerX + relativeX, centerY + relativeY);
        }
    }
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

void qrCode::drawAllAlignmentPatterns(uint32_t someVersion) {
    if (someVersion == 1) {
        return;
    }
    uint32_t nmbrRowsCols = nmbrOfAlignmentPatternRowsOrCols(someVersion);

    for (uint32_t rowIndex = 0; rowIndex < nmbrRowsCols; rowIndex++) {
        for (uint32_t columnIndex = 0; columnIndex < nmbrRowsCols; columnIndex++) {
            if ((rowIndex == 0) && (columnIndex == 0)) {
                continue;
            }
            if ((rowIndex == 0) && (columnIndex == (nmbrRowsCols - 1))) {
                continue;
            }
            if ((rowIndex == (nmbrRowsCols - 1)) && (columnIndex == 0)) {
                continue;
            }
            uint32_t xCoordinate = alignmentPatternCoordinate(someVersion, columnIndex);
            uint32_t yCoordinate = alignmentPatternCoordinate(someVersion, rowIndex);
            drawAlignmentPattern(xCoordinate, yCoordinate);
        }
    }
}

void qrCode::drawTimingPattern(uint32_t someVersion) {
    const uint32_t endIndex = size(someVersion) - 9;
    for (uint32_t index = 8; index <= endIndex; index++) {
        if (index % 2 == 0) {
            modules.setBit(index, 6);
            modules.setBit(6, index);
        }
        isData.clearBit(index, 6);
        isData.clearBit(6, index);
    }
}

uint32_t qrCode::calculateFormatInfo(errorCorrectionLevel someErrorCorrectionLevel, uint32_t mask) {
    uint32_t formatBits{0};
    static constexpr uint8_t errorCorrectionLevelBits[nmbrOfErrorCorrectionLevels]{0b01, 0b00, 0b11, 0b10};        // 2 bits
    formatBits = errorCorrectionLevelBits[static_cast<uint32_t>(someErrorCorrectionLevel)] << 3;                   //
    formatBits |= mask;                                                                                            // 3 bits

    uint32_t remainder = formatBits;
    for (int i = 0; i < 10; i++) {
        remainder = (remainder << 1) ^ ((remainder >> 9) * 0x537);
    }

    formatBits = formatBits << 10 | remainder;
    formatBits ^= 0x5412;        // uint15
    return formatBits;
}

void qrCode::drawFormatInfoCopy1(uint32_t data) {
    // Draw first copy - around top-left finder, skipping timing patterns
    bitVector<15> formatInfoBits;
    formatInfoBits.appendBits(data, 15);
    for (uint32_t bitIndex = 0; bitIndex < 6; bitIndex++) {
        modules.setOrClearBit(bitIndex, 8, formatInfoBits.getBit(bitIndex));
        isData.clearBit(bitIndex, 8);
    }

    modules.setOrClearBit(7, 8, formatInfoBits.getBit(6));
    isData.clearBit(7, 8);
    modules.setOrClearBit(8, 8, formatInfoBits.getBit(7));
    isData.clearBit(8, 8);
    modules.setOrClearBit(8, 7, formatInfoBits.getBit(8));
    isData.clearBit(8, 7);

    for (uint32_t bitIndex = 9; bitIndex < 15; bitIndex++) {
        modules.setOrClearBit(8, 14 - bitIndex, formatInfoBits.getBit(bitIndex));
        isData.clearBit(8, 14 - bitIndex);
    }
}

void qrCode::drawFormatInfoCopy2(uint32_t data) {
    // Draw second copy - right from bottom-left finder, under top-right finder, skipping the dark module
    bitVector<15> formatInfoBits;
    formatInfoBits.appendBits(data, 15);
    uint32_t endIndex = size(theVersion) - 1;
    for (uint32_t bitIndex = 0; bitIndex < 7; bitIndex++) {
        modules.setOrClearBit(8, endIndex - bitIndex, formatInfoBits.getBit(bitIndex));
        isData.clearBit(8, endIndex - bitIndex);
    }
    for (uint32_t bitIndex = 7; bitIndex < 15; bitIndex++) {
        modules.setOrClearBit(endIndex - 14 + bitIndex, 8, formatInfoBits.getBit(bitIndex));
        isData.clearBit(endIndex - 14 + bitIndex, 8);
    }
}

void qrCode::drawFormatInfo(errorCorrectionLevel someErrorCorrectionLevel, uint32_t someMask) {
    uint32_t formatInfoBits = calculateFormatInfo(someErrorCorrectionLevel, someMask);
    drawFormatInfoCopy1(formatInfoBits);
    drawFormatInfoCopy2(formatInfoBits);
}

void qrCode::drawVersionInfo(uint32_t someVersion) {
    if (someVersion < 7) {
        return;
    }

    // Calculate error correction code and pack bits
    uint32_t rem = someVersion;        // version is uint6, in the range [7, 40]
    for (uint8_t i = 0; i < 12; i++) {
        rem = (rem << 1) ^ ((rem >> 11) * 0x1F25);
    }

    uint32_t data = someVersion << 12 | rem;        // uint18

    for (uint8_t i = 0; i < 18; i++) {
        bool bit  = ((data >> i) & 1) != 0;
        uint8_t a = size(someVersion) - 11 + i % 3;
        uint8_t b = i / 3;
        modules.setOrClearBit(a, b, bit);
        modules.setOrClearBit(b, a, bit);
    }
}

void qrCode::drawPatterns(uint32_t someVersion) {
    drawAllFinderPatternsAndSeparators(someVersion);
    drawTimingPattern(someVersion);
    drawAllAlignmentPatterns(someVersion);
    drawDarkModule(someVersion);
    drawDummyFormatBits(someVersion);
    // drawVersionInfo(theVersion);
}

void qrCode::drawPayload(uint32_t someVersion) {
    uint32_t codeSize = size(someVersion);
    uint32_t bitIndex{0};
    uint32_t x{0};
    bool isUpwards{false};
    uint32_t y{0};

    for (int32_t columnPairIndex = (codeSize - 1); columnPairIndex >= 1; columnPairIndex -= 2) {
        if (columnPairIndex == 6) {        // Skip the timing pattern
            columnPairIndex = 5;
        }
        for (uint32_t verticalIndex = 0; verticalIndex < codeSize; verticalIndex++) {
            for (int columnIndex = 0; columnIndex < 2; columnIndex++) {
                x         = columnPairIndex - columnIndex;
                isUpwards = ((columnPairIndex & 2) == 0) ^ (x < 6);
                y         = isUpwards ? (codeSize - 1 - verticalIndex) : verticalIndex;
                if (isData.getBit(x, y)) {
                    if (buffer.getBit(bitIndex)) {
                        modules.setBit(x, y);
                    }
                    bitIndex++;
                }
            }
        }
    }
}

void qrCode::applyMask(uint8_t maskType) {
    bool invert{false};
    for (uint32_t x = 0; x < size(theVersion); x++) {
        for (uint32_t y = 0; y < size(theVersion); y++) {
            if (isData.getBit(x, y)) {
                switch (maskType) {
                    default:        // intentional fallthrough
                    case 0:
                        invert = ((x + y) % 2 == 0);
                        break;
                    case 1:
                        invert = (y % 2 == 0);
                        break;
                    case 2:
                        invert = (x % 3 == 0);
                        break;
                    case 3:
                        invert = ((x + y) % 3 == 0);
                        break;
                    case 4:
                        invert = ((x / 3 + y / 2) % 2 == 0);
                        break;
                    case 5:
                        invert = (x * y % 2 + x * y % 3 == 0);
                        break;
                    case 6:
                        invert = ((x * y % 2 + x * y % 3) % 2 == 0);
                        break;
                    case 7:
                        invert = (((x + y) % 2 + x * y % 3) % 2 == 0);
                        break;
                }
                if (invert) {
                    modules.invertBit(x, y);
                }
            }
        }
    }
}

uint32_t qrCode::penalty() {
    return penalty1() + penalty2() + penalty3() + penalty4();
}

uint32_t qrCode::penalty1() {
    uint32_t result{0};
    static constexpr uint32_t penaltyN1{3};
    uint32_t width  = size(theVersion);
    uint32_t height = size(theVersion);

    for (uint8_t y = 0; y < height; y++) {
        bool colorX = modules.getBit(0, y);
        for (uint8_t x = 1, runX = 1; x < width; x++) {
            bool cx = modules.getBit(x, y);
            if (cx != colorX) {
                colorX = cx;
                runX   = 1;

            } else {
                runX++;
                if (runX == 5) {
                    result += penaltyN1;
                } else if (runX > 5) {
                    result++;
                }
            }
        }
    }

    for (uint8_t x = 0; x < width; x++) {
        bool colorY = modules.getBit(x, 0);
        for (uint8_t y = 1, runY = 1; y < height; y++) {
            bool cy = modules.getBit(x, y);
            if (cy != colorY) {
                colorY = cy;
                runY   = 1;
            } else {
                runY++;
                if (runY == 5) {
                    result += penaltyN1;
                } else if (runY > 5) {
                    result++;
                }
            }
        }
    }
    return result;
}

uint32_t qrCode::penalty2() {
    uint32_t result{0};
    static constexpr uint32_t penaltyN2{3};
    uint32_t maxX = size(theVersion) - 2;
    uint32_t maxY = size(theVersion) - 2;

    for (uint8_t y = 0; y <= maxY; y++) {
        for (uint8_t x = 0; x <= maxX; x++) {
            bool colorTopLeft     = modules.getBit(x, y);
            bool colorTopRight    = modules.getBit(x + 1, y);
            bool colorBottomLeft  = modules.getBit(x, y + 1);
            bool colorBottomRight = modules.getBit(x + 1, y + 1);
            if ((colorTopLeft == colorTopRight) && (colorTopLeft == colorBottomLeft) && (colorTopLeft == colorBottomRight)) {
                result += penaltyN2;
            }
        }
    }
    return result;
}

uint32_t qrCode::penalty3() {
    uint32_t result{0};
    static constexpr uint32_t penaltyN3{40};
    uint32_t width  = size(theVersion);
    uint32_t height = size(theVersion);

    for (uint8_t y = 0; y < height; y++) {
        uint16_t bitsRow{0};
        uint16_t bitsCol{0};
        for (uint8_t x = 0; x < width; x++) {
            bool color = modules.getBit(x, y);
            // Finder-like pattern in rows and columns
            bitsRow = ((bitsRow << 1) & 0x7FF) | color;
            bitsCol = ((bitsCol << 1) & 0x7FF) | modules.getBit(y, x);

            // Needs 11 bits accumulated
            if (x >= 10) {
                if (bitsRow == 0x05D || bitsRow == 0x5D0) {
                    result += penaltyN3;
                }
                if (bitsCol == 0x05D || bitsCol == 0x5D0) {
                    result += penaltyN3;
                }
            }
        }
    }
    return result;
}

uint32_t qrCode::penalty4() {
    static constexpr uint32_t penaltyN4{10};
    uint32_t maxX = size(theVersion) - 1;
    uint32_t maxY = size(theVersion) - 1;
    uint32_t blackModulesCount{0};
    for (uint8_t y = 0; y <= maxY; y++) {
        for (uint8_t x = 0; x <= maxX; x++) {
            if (modules.getBit(x, y)) {
                blackModulesCount++;
            }
        }
    }
    int32_t ratio  = (blackModulesCount * 100U) / (size(theVersion) * size(theVersion));
    uint32_t delta = abs(ratio - 50) / 5;
    return delta * penaltyN4;
}

void qrCode::selectMask() {
    uint32_t penalties[nmbrOfMasks]{0};
    for (uint32_t maskIndex = 0; maskIndex < nmbrOfMasks; maskIndex++) {
        applyMask(maskIndex);
        drawFormatInfo(theErrorCorrectionLevel, maskIndex);
        penalties[maskIndex] = penalty();
        applyMask(maskIndex);        // applying again reverts because it is XOR operation
    }

    uint32_t lowestPenalty = penalties[0];
    uint32_t lowestPenaltyIndex{0};
    for (uint32_t maskIndex = 1; maskIndex < nmbrOfMasks; maskIndex++) {
        if (penalties[maskIndex] < lowestPenalty) {
            lowestPenalty      = penalties[maskIndex];
            lowestPenaltyIndex = maskIndex;
        }
    }
    theMask = lowestPenaltyIndex;
    applyMask(theMask);
    drawFormatInfo(theErrorCorrectionLevel, theMask);
}

#pragma endregion

// ?????

uint32_t qrCode::payloadLengthInBits(uint32_t dataLengthInBytes, uint32_t someVersion, encodingFormat someEncodingFormat) {
    uint32_t modeIndicatorLength{4U};
    switch (someEncodingFormat) {
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
            return modeIndicatorLength + characterCountIndicatorLength(someVersion, someEncodingFormat) + (10 * (dataLengthInBytes / 3)) + remainderLength;
        case encodingFormat::alphanumeric:
            return modeIndicatorLength + characterCountIndicatorLength(someVersion, someEncodingFormat) + (11 * (dataLengthInBytes / 2)) + (6 * (dataLengthInBytes % 2));
        case encodingFormat::byte:        // intentional fallthrough
        default:
            return modeIndicatorLength + characterCountIndicatorLength(someVersion, someEncodingFormat) + (8 * dataLengthInBytes);
    }
}

uint32_t qrCode::nmbrOfDataModules(uint32_t someVersion) {
    return nmbrOfTotalModules(someVersion) - nmbrOfFunctionModules(someVersion);
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

uint32_t qrCode::nmbrOfErrorCorrectionModules(uint32_t someVersion, errorCorrectionLevel someErrorCorrectionLevel) {
    return versionProperties[someVersion - 1].nmbrErrorCorrectionCodewordsPerBlock[static_cast<uint32_t>(someErrorCorrectionLevel)] * versionProperties[someVersion - 1].nmbrBlocks[static_cast<uint32_t>(someErrorCorrectionLevel)] * 8;
}
