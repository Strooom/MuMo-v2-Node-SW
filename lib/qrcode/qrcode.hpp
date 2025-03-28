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
    static uint32_t errorCorrectionLevelBits(errorCorrectionLevel someLevel);
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
    static uint32_t formatInfo(uint32_t maskType);
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
    static constexpr uint32_t PENALTY_N1{3};
    static constexpr uint32_t PENALTY_N2{3};
    static constexpr uint32_t PENALTY_N3{40};
    static constexpr uint32_t PENALTY_N4{10};
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

    // internal data
    static uint32_t theVersion;
    static errorCorrectionLevel theErrorCorrectionLevel;
    static encodingFormat theEncodingFormat;
    static uint32_t theMask;

    static bitVector<maxPayloadLengthInBytes * 8U> buffer;
    static bitMatrix<maxSize> modules;
    static bitMatrix<maxSize> isData;
};