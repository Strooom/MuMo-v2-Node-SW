// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <bitvector.hpp>
#include <bitmatrix.hpp>
#include <encodingformat.hpp>

static constexpr uint32_t nmbrOfErrorCorrectionLevels{4};
enum class errorCorrectionLevel {
    low      = 0,
    medium   = 1,
    quartile = 2,
    high     = 3,
};

static constexpr uint32_t nmbrOfMasks{8};

class qrCode {
  public:
    // api
    static uint32_t versionNeeded(const char *data, errorCorrectionLevel minimumErrorCorrectionLevel);                                          // null-terminated string of data
    static uint32_t versionNeeded(const uint8_t *data, uint32_t dataLength, errorCorrectionLevel minimumErrorCorrectionLevel);                  //
    static errorCorrectionLevel errorCorrectionLevelPossible(const char *data, uint32_t someVersion);                                           // null-terminated string of data
    static errorCorrectionLevel errorCorrectionLevelPossible(const uint8_t *data, uint32_t dataLength, uint32_t someVersion);                   //
    static void generate(const char *data, uint32_t someVersion, errorCorrectionLevel someErrorCorrectionLevel);                                // null-terminated string of data
    static void generate(const uint8_t *data, uint32_t dataLength, uint32_t someVersion, errorCorrectionLevel someErrorCorrectionLevel);        //
    static bool getModule(uint32_t x, uint32_t y);

    // public compile-time constants
    static constexpr uint32_t maxVersion{5};                        // 1..40 This sets the maximum version that is supported by this library. Storage for the input data and output pixelmatrix is allocated statically and depends on this value.
    static constexpr uint32_t maxSize{17 + 4 * maxVersion};         //
    static constexpr uint32_t maxInputLength{127U};                 // Maximum length of a c-style string with payload data to encode into the QRCode. This maximum is needed to limit strlen functions for safety
    static constexpr uint32_t maxPayloadLengthInBytes{256U};        //

#ifndef unitTesting

  private:
#endif
    // general helpers
    static uint32_t size(uint32_t someVersion);
    static void setVersion(uint32_t theVersion);
    static void setErrorCorrectionLevel(errorCorrectionLevel theErrorCorrectionLevel);

    // encoding user data into payload
    static bool isNumeric(const char *data);                                                  // null-terminated string of data
    static bool isNumeric(const uint8_t data);                                                //
    static bool isNumeric(const uint8_t *data, uint32_t length);                              //
    static bool isAlphanumeric(const char *data);                                             // null-terminated string of data
    static bool isAlphanumeric(const uint8_t data);                                           //
    static bool isAlphanumeric(const uint8_t *data, uint32_t length);                         //
    static encodingFormat getEncodingFormat(const char *data);                                // null-terminated string of data
    static encodingFormat getEncodingFormat(const uint8_t *data, uint32_t dataLength);        //
    static uint8_t compressNumeric(char c);
    static uint8_t compressAlphanumeric(char c);
    static void encodeNumeric(const uint8_t *data, uint32_t dataLength);
    static void encodeAlfaNumeric(const uint8_t *data, uint32_t dataLength);
    static void encodeByte(const uint8_t *data, uint32_t dataLength);
    static uint8_t modeIndicator(encodingFormat theEncodingFormat);
    static uint32_t characterCountIndicatorLength(uint32_t someVersion, encodingFormat someEncodingFormat);
    static void addTerminator();
    static void addBitPadding();
    static void addBytePadding();
    static void encodeData(const char *data);        // pre-set version and errorCorrection, null-terminated string of data
    static void encodeData(const uint8_t *data, uint32_t dataLength);

    // error correction
    static uint32_t nmbrBlocks(uint32_t someVersion, errorCorrectionLevel someErrorCorrectionLevel);
    static uint32_t blockLength(uint32_t blockIndex, uint32_t someVersion, errorCorrectionLevel someErrorCorrectionLevel);
    static uint32_t blockOffset(uint32_t blockIndex, uint32_t someVersion, errorCorrectionLevel someErrorCorrectionLevel);
    static uint32_t nmbrBlocksGroup1(uint32_t someVersion, errorCorrectionLevel someErrorCorrectionLevel);
    static uint32_t nmbrBlocksGroup2(uint32_t someVersion, errorCorrectionLevel someErrorCorrectionLevel);
    static uint32_t blockLengthGroup1(uint32_t someVersion, errorCorrectionLevel someErrorCorrectionLevel);
    static uint32_t blockLengthGroup2(uint32_t someVersion, errorCorrectionLevel someErrorCorrectionLevel);
    static uint32_t dataOffset(uint32_t blockIndex, uint32_t rowIndex);
    static uint32_t eccOffset(uint32_t blockIndex, uint32_t rowIndex);
    static void addErrorCorrection();
    static void interleaveData();

    // drawing function patterns and user data
    static void drawFinderPattern(uint32_t centerX, uint32_t centerY);
    static void drawFinderSeparators(uint32_t someVersion);
    static void drawAllFinderPatternsAndSeparators(uint32_t theVersion);
    static void drawDummyFormatBits(uint32_t someVersion);
    static void drawDarkModule(uint32_t someVersion);
    static void drawAlignmentPattern(uint32_t centerX, uint32_t centerY, uint32_t someVersion);
    static uint32_t nmbrOfAlignmentPatterns(uint32_t someVersion);
    static uint32_t nmbrOfAlignmentPatternRowsOrCols(uint32_t someVersion);
    static uint32_t alignmentPatternSpacing(uint32_t someVersion);
    static uint32_t alignmentPatternCoordinate(uint32_t someVersion, uint32_t index);
    static void drawAllAlignmentPatterns(uint32_t theVersion);
    static void drawTimingPattern(uint32_t theVersion);
    static uint32_t calculateFormatInfo(errorCorrectionLevel someErrorCorrectionLevel, uint32_t mask);
    static void drawFormatInfoCopy1(uint32_t data);
    static void drawFormatInfoCopy2(uint32_t data);
    static void drawFormatInfo();
    static void drawVersionInfo(uint32_t theVersion);
    static void drawPatterns(uint32_t theVersion);
    static void drawPayload(uint32_t theVersion);
    static void applyMask(uint8_t maskType);
    static uint32_t getPenaltyScore();
    static uint32_t penalty1();
    static uint32_t penalty2();
    static uint32_t penalty3();
    static uint32_t penalty4();

    // ????

    static uint32_t payloadLengthInBits(uint32_t dataLengthInBytes, uint32_t someVersion, encodingFormat someEncodingFormat);
    static uint32_t nmbrOfTotalModules(uint32_t someVersion);
    static uint32_t nmbrOfDataModules(uint32_t someVersion);
    static uint32_t nmbrOfFunctionModules(uint32_t someVersion);
    static uint32_t nmbrOfErrorCorrectionModules(uint32_t theVersion, errorCorrectionLevel theErrorCorrectionLevel);

    // private compile-time constants
    static constexpr struct {
        uint16_t availableDataCodeWords[nmbrOfErrorCorrectionLevels];
        uint8_t nmbrBlocks[nmbrOfErrorCorrectionLevels];
        uint8_t nmbrErrorCorrectionCodewordsPerBlock[nmbrOfErrorCorrectionLevels];
    } versionProperties[maxVersion]{
        // {payloadBytes}, {blocks}, {errorCorrection bytes per block}
        {{19, 16, 13, 9}, {1, 1, 1, 1}, {7, 10, 13, 17}},
        {{34, 28, 22, 16}, {1, 1, 1, 1}, {10, 16, 22, 28}},
        {{55, 44, 34, 26}, {1, 1, 2, 2}, {15, 26, 18, 22}},
        {{80, 64, 48, 36}, {1, 2, 2, 4}, {20, 18, 26, 16}},
        {{108, 86, 62, 46}, {1, 2, 4, 4}, {26, 24, 18, 22}}
        //        {{108, 86, 62, 46}, {2, 4, 4, 4}, {36, 24, 18, 22}}
    };

    static constexpr uint16_t formatInfoBits[nmbrOfErrorCorrectionLevels][nmbrOfMasks]{
        {0b111011111000100, 0b111001011110011, 0b111110110101010, 0b111100010011101, 0b110011000101111, 0b110001100011000, 0b110110001000001, 0b110100101110110},        // formatInfo bits taken from https://www.thonky.com/qr-code-tutorial/format-version-tables
        {0b101010000010010, 0b101000100100101, 0b101111001111100, 0b101101101001011, 0b100010111111001, 0b100000011001110, 0b100111110010111, 0b100101010100000},
        {0b011010101011111, 0b011000001101000, 0b011111100110001, 0b011101000000110, 0b010010010110100, 0b010000110000011, 0b010111011011010, 0b010101111101101},
        {0b001011010001001, 0b001001110111110, 0b001110011100111, 0b001100111010000, 0b000011101100010, 0b000001001010101, 0b000110100001100, 0b000100000111011}};

    static constexpr uint32_t versionBits[34]{
        0b000111110010010100,
        0b001000010110111100,
        0b001001101010011001,
        0b001010010011010011,
        0b001011101111110110,
        0b001100011101100010,
        0b001101100001000111,
        0b001110011000001101,
        0b001111100100101000,
        0b010000101101111000,
        0b010001010001011101,
        0b010010101000010111,
        0b010011010100110010,
        0b010100100110100110,
        0b010101011010000011,
        0b0010110100011001001, // ??
        0b010111011111101100,
        0b011000111011000100,
        0b011001000111100001,
        0b011010111110101011,
        0b011011000010001110,
        0b011100110000011010,
        0b011101001100111111,
        0b011110110101110101,
        0b011111001001010000,
        0b100000100111010101,
        0b100001011011110000,
        0b100010100010111010,
        0b100011011110011111,
        0b100100101100001011,
        0b100101010000101110,
        0b100110101001100100,
        0b100111010101000001,
        0b101000110001101001};        // versionInfo bits taken from https://www.thonky.com/qr-code-tutorial/format-version-tables

    // internal data
    static uint32_t theVersion;
    static errorCorrectionLevel theErrorCorrectionLevel;
    static encodingFormat theEncodingFormat;
    static uint32_t theMask;

    static bitVector<maxPayloadLengthInBytes * 8U> buffer;
    static bitMatrix<maxSize> modules;
    static bitMatrix<maxSize> isData;
};