// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################
// based on the work of Richard Moore : https://github.com/ricmoo/QRCode
// based on the work of Nayuki : https://www.nayuki.io/page/qr-code-generator-library

#pragma once
#include <stdint.h>
#include <bitvector.hpp>
#include <bitmatrix.hpp>
#include <encodingformat.hpp>

enum class errorCorrectionLevel {
    low      = 0,
    medium   = 1,
    quartile = 2,
    high     = 3,
};

static constexpr uint32_t nmbrOfErrorCorrectionLevels{4};

class qrCode {
  public:
    static constexpr uint32_t maxVersion{5};                                                                                         // 1..40 This sets the maximum version that is supported by this library. Storage for the input data and output pixelmatrix is allocated statically and depends on this value.
    static constexpr uint32_t maxSize{17 + 4 * maxVersion};                                                                          //
    static constexpr uint32_t maxInputLength{127U};                                                                                  // Maximum length of a c-style string with payload data to encode into the QRCode. This maximum is needed to limit strlen functions for safety
    static constexpr uint32_t maxPayloadLengthInBytes{36U};                                                                          //
    static uint32_t versionNeeded(const char *data, errorCorrectionLevel wantedErrorCorrectionLevel);                                // null-terminated string of data
    static uint32_t versionNeeded(const uint8_t *data, uint32_t dataLength, errorCorrectionLevel wantedErrorCorrectionLevel);        //

    static void setVersion(uint32_t theVersion);
    static void setErrorCorrectionLevel(errorCorrectionLevel theErrorCorrectionLevel);
    static encodingFormat getEncodingFormat(const char *data);                                // null-terminated string of data
    static encodingFormat getEncodingFormat(const uint8_t *data, uint32_t dataLength);        //
    static void encodeData(const char *data);                                                 // pre-set version and errorCorrection, null-terminated string of data
    static void encodeData(const uint8_t *data, uint32_t dataLength);
    static void encodeNumeric(const uint8_t *data, uint32_t dataLength);
    static void encodeAlfaNumeric(const uint8_t *data, uint32_t dataLength);
    static void encodeByte(const uint8_t *data, uint32_t dataLength);

    static void addTerminator();
    static void addBitPadding();
    static void addBytePadding();

    static uint32_t nmbrBlocksGroup1(uint32_t someVersion, errorCorrectionLevel someErrorCorrectionLevel);
    static uint32_t nmbrBlocksGroup2(uint32_t someVersion, errorCorrectionLevel someErrorCorrectionLevel);
    static uint32_t blockLengthGroup1(uint32_t someVersion, errorCorrectionLevel someErrorCorrectionLevel);
    static uint32_t blockLengthGroup2(uint32_t someVersion, errorCorrectionLevel someErrorCorrectionLevel);

#ifndef unitTesting

  private:
#endif
    static uint32_t size(uint32_t someVersion);
    static uint32_t payloadLengthInBits(uint32_t dataLengthInBytes, uint32_t someVersion, encodingFormat someEncodingFormat);
    static uint32_t nmbrOfTotalModules(uint32_t someVersion);
    static uint32_t nmbrOfDataModules(uint32_t someVersion);
    static uint32_t nmbrOfFunctionModules(uint32_t someVersion);
    static uint32_t characterCountIndicatorLength(uint32_t someVersion, encodingFormat someEncodingFormat);
    static uint32_t nmbrOfAlignmentPatterns(uint32_t someVersion);
    static uint32_t nmbrOfAlignmentPatternRowsOrCols(uint32_t someVersion);

    static constexpr struct {
        uint16_t availableDataCodeWords[nmbrOfErrorCorrectionLevels];
        uint8_t nmbrBlocks[nmbrOfErrorCorrectionLevels];
        uint8_t nmbrErrorCorrectionCodewordsPerBlock[nmbrOfErrorCorrectionLevels];
    } versionProperties[maxVersion]{
        {{19, 16, 13, 9}, {1, 1, 1, 1}, {7, 10, 13, 17}},
        {{34, 28, 22, 16}, {1, 1, 1, 1}, {10, 16, 22, 28}},
        {{55, 44, 34, 26}, {1, 1, 2, 2}, {15, 26, 18, 22}},
        {{80, 64, 48, 36}, {1, 2, 2, 4}, {20, 18, 26, 16}},
        {{108, 86, 62, 46}, {1, 2, 4, 4}, {26, 24, 18, 22}}};

    static bool isNumeric(const char *data);                            // null-terminated string of data
    static bool isNumeric(const uint8_t data);                          //
    static bool isNumeric(const uint8_t *data, uint32_t length);        //

    static bool isAlphanumeric(const char *data);                            // null-terminated string of data
    static bool isAlphanumeric(const uint8_t data);                          //
    static bool isAlphanumeric(const uint8_t *data, uint32_t length);        //

    static uint8_t compressNumeric(char c);
    static uint8_t compressAlphanumeric(char c);
    static uint8_t modeIndicator(encodingFormat theEncodingFormat);

    static bool isDataModule(uint32_t x, uint32_t y, uint32_t someVersion);
    static bool isTimingPattern(uint32_t x, uint32_t y);
    static bool isDarkModule(uint32_t x, uint32_t y, uint32_t someVersion);
    static bool isAlignmentPattern(uint32_t x, uint32_t y, uint32_t someVersion);
    static bool isFormatInformation(uint32_t x, uint32_t y, uint32_t someVersion);
    static bool isVersionInformation(uint32_t x, uint32_t y, uint32_t someVersion);
    static bool isFinderPatternOrSeparator(uint32_t x, uint32_t y, uint32_t someVersion);

    static void drawFinderPattern(uint32_t centerX, uint32_t centerY);
    static void drawAllFinderPatterns(uint32_t theVersion);
    static void drawAlignmentPattern(uint32_t centerX, uint32_t centerY, uint32_t someVersion);
    static void drawAllAlignmentPatterns(uint32_t theVersion);
    static void drawTimingPattern(uint32_t theVersion);
    static void drawFormatInfo(uint32_t theVersion);
    static void drawVersionInfo(uint32_t theVersion);
    static void drawPayload(uint32_t theVersion);
    static void applyMask(uint8_t maskType);
    static uint32_t getPenaltyScore();

    static uint32_t nmbrOfErrorCorrectionModules(uint32_t theVersion, errorCorrectionLevel theErrorCorrectionLevel);
    static uint32_t alignmentPatternSpacing(uint32_t someVersion);
    static uint32_t alignmentPatternCoordinate(uint32_t someVersion, uint32_t index);

    static void addErrorCorrection();

    static void interleaveData(uint8_t *destination, const uint8_t *source, const uint32_t sourceLength, const uint32_t nmbrOfBlocksGroup1, const uint32_t nmbrOfBlocksGroup2, const uint32_t blockLength);

    // static constexpr uint16_t nmbrRawDataModules[maxVersion]                                        = {208, 359, 567, 807, 1079, 1383, 1568, 1936, 2336, 2768, 3232, 3728, 4256, 4651, 5243, 5867, 6523, 7211, 7931, 8683, 9252, 10068, 10916, 11796, 12708, 13652, 14628, 15371, 16411, 17483, 18587, 19723, 20891, 22091, 23008, 24272, 25568, 26896, 28256, 29648};
    // static constexpr uint32_t nmbrOfErrorCorrectionLevels                                           = static_cast<uint32_t>(errorCorrectionLevel::nmbrOfErrorCorrectionLevels);
    // static constexpr uint32_t nmbrErrorCorrectionCodewords[nmbrOfErrorCorrectionLevels][maxVersion] = {
    //     {7, 10, 15, 20, 26, 36, 40, 48, 60, 72, 80, 96, 104, 120, 132, 144, 168, 180, 196, 224, 224, 252, 270, 300, 312, 336, 360, 390, 420, 450, 480, 510, 540, 570, 570, 600, 630, 660, 720, 750},                                // Low
    //     {10, 16, 26, 36, 48, 64, 72, 88, 110, 130, 150, 176, 198, 216, 240, 280, 308, 338, 364, 416, 442, 476, 504, 560, 588, 644, 700, 728, 784, 812, 868, 924, 980, 1036, 1064, 1120, 1204, 1260, 1316, 1372},                    // Medium
    //     {13, 22, 36, 52, 72, 96, 108, 132, 160, 192, 224, 260, 288, 320, 360, 408, 448, 504, 546, 600, 644, 690, 750, 810, 870, 952, 1020, 1050, 1140, 1200, 1290, 1350, 1440, 1530, 1590, 1680, 1770, 1860, 1950, 2040},           // Quartile
    //     {17, 28, 44, 64, 88, 112, 130, 156, 192, 224, 264, 308, 352, 384, 432, 480, 532, 588, 650, 700, 750, 816, 900, 960, 1050, 1110, 1200, 1260, 1350, 1440, 1530, 1620, 1710, 1800, 1890, 1980, 2100, 2220, 2310, 2430},        // High
    // };
    // static constexpr uint8_t nmbrErrorCorrectionBlocks[static_cast<uint32_t>(errorCorrectionLevel::nmbrOfErrorCorrectionLevels)][maxVersion] = {
    //     // Note : index 0 is for padding, and is set to an illegal value)
    //     {1, 1, 1, 1, 1, 2, 2, 2, 2, 4, 4, 4, 4, 4, 6, 6, 6, 6, 7, 8, 8, 9, 9, 10, 12, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25},                     // Low
    //     {1, 1, 1, 2, 2, 4, 4, 4, 5, 5, 5, 8, 9, 9, 10, 10, 11, 13, 14, 16, 17, 17, 18, 20, 21, 23, 25, 26, 28, 29, 31, 33, 35, 37, 38, 40, 43, 45, 47, 49},            // Medium
    //     {1, 1, 2, 2, 4, 4, 6, 6, 8, 8, 8, 10, 12, 16, 12, 17, 16, 18, 21, 20, 23, 23, 25, 27, 29, 34, 34, 35, 38, 40, 43, 45, 48, 51, 53, 56, 59, 62, 65, 68},         // Quartile
    //     {1, 1, 2, 4, 4, 4, 5, 6, 8, 8, 11, 11, 16, 16, 18, 16, 19, 21, 25, 25, 25, 34, 30, 32, 35, 37, 40, 42, 45, 48, 51, 54, 57, 60, 63, 66, 70, 74, 77, 81},        // High
    // };

    static uint32_t theVersion;
    static errorCorrectionLevel theErrorCorrectionLevel;
    static encodingFormat theEncodingFormat;

    static bitVector<maxPayloadLengthInBytes * 8U> buffer;
    static bitMatrix<maxSize> modules;
    static bitMatrix<maxSize> isData;

    static constexpr uint32_t PENALTY_N1{3};
    static constexpr uint32_t PENALTY_N2{3};
    static constexpr uint32_t PENALTY_N3{40};
    static constexpr uint32_t PENALTY_N4{10};
};