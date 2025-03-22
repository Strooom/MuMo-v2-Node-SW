// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include <qrcode.hpp>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_isNumeric() {
    // single character
    TEST_ASSERT_TRUE(qrCode::isNumeric('0'));
    TEST_ASSERT_TRUE(qrCode::isNumeric('1'));
    TEST_ASSERT_TRUE(qrCode::isNumeric('8'));
    TEST_ASSERT_TRUE(qrCode::isNumeric('9'));
    TEST_ASSERT_FALSE(qrCode::isNumeric('/'));
    TEST_ASSERT_FALSE(qrCode::isNumeric(':'));
    // null-terminated string
    TEST_ASSERT_TRUE(qrCode::isNumeric("0123456789"));
    TEST_ASSERT_FALSE(qrCode::isNumeric("0/1"));
    TEST_ASSERT_FALSE(qrCode::isNumeric("0:1"));
    // byte array with length
    const uint8_t testData1[10]{48, 49, 50, 51, 52, 53, 54, 55, 56, 57};
    TEST_ASSERT_TRUE(qrCode::isNumeric(testData1, 4));
    const uint8_t testData2[2]{47, 58};
    TEST_ASSERT_FALSE(qrCode::isNumeric(testData2, 2));
}

void test_isAlphanumeric() {
    // single character
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric('0'));
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric('9'));
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric('A'));
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric('Z'));
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric(' '));
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric('$'));
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric('%'));
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric('*'));
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric('+'));
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric('-'));
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric('.'));
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric('/'));
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric(':'));
    TEST_ASSERT_FALSE(qrCode::isAlphanumeric('_'));
    TEST_ASSERT_FALSE(qrCode::isAlphanumeric('#'));
    TEST_ASSERT_FALSE(qrCode::isAlphanumeric('^'));
    TEST_ASSERT_FALSE(qrCode::isAlphanumeric('('));
    TEST_ASSERT_FALSE(qrCode::isAlphanumeric('{'));
    // null-terminated string
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric("0123456789ABCDEF"));
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric("HTTPS://WWW.STROOOM.BE"));
    TEST_ASSERT_FALSE(qrCode::isAlphanumeric("HTTPS://WWW.STROOOM.BE?"));
    // byte array with length
    const uint8_t testData1[13]{48, 57, 65, 90, 32, 36, 37, 42, 43, 45, 46, 47, 58};
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric(testData1, 13));
    const uint8_t testData2[2]{35, 40};
    TEST_ASSERT_FALSE(qrCode::isAlphanumeric(testData2, 2));
}

void test_getEncodingFormat() {
    TEST_ASSERT_EQUAL(encodingFormat::numeric, qrCode::getEncodingFormat("0123456789"));
    TEST_ASSERT_EQUAL(encodingFormat::alphanumeric, qrCode::getEncodingFormat("HTTPS://WWW.STROOOM.BE"));
    TEST_ASSERT_EQUAL(encodingFormat::byte, qrCode::getEncodingFormat("HTTPS://WWW.STROOOM.BE?"));

    const uint8_t testData1[10]{48, 49, 50, 51, 52, 53, 54, 55, 56, 57};
    TEST_ASSERT_EQUAL(encodingFormat::numeric, qrCode::getEncodingFormat(testData1, 10));
    const uint8_t testData2[13]{48, 57, 65, 90, 32, 36, 37, 42, 43, 45, 46, 47, 58};
    TEST_ASSERT_EQUAL(encodingFormat::alphanumeric, qrCode::getEncodingFormat(testData2, 13));
    const uint8_t testData3[2]{35, 40};
    TEST_ASSERT_EQUAL(encodingFormat::byte, qrCode::getEncodingFormat(testData3, 2));
}

void test_payloadLengthInBits() {
    TEST_ASSERT_EQUAL(41, qrCode::payloadLengthInBits(8, 1, encodingFormat::numeric));
    TEST_ASSERT_EQUAL(57, qrCode::payloadLengthInBits(8, 1, encodingFormat::alphanumeric));
    TEST_ASSERT_EQUAL(76, qrCode::payloadLengthInBits(8, 1, encodingFormat::byte));

    TEST_ASSERT_EQUAL(151, qrCode::payloadLengthInBits(41, 1, encodingFormat::numeric));
    TEST_ASSERT_EQUAL(151, qrCode::payloadLengthInBits(25, 1, encodingFormat::alphanumeric));
    TEST_ASSERT_EQUAL(148, qrCode::payloadLengthInBits(17, 1, encodingFormat::byte));
}

void test_characterCountIndicatorLength() {
    TEST_ASSERT_EQUAL(10, qrCode::characterCountIndicatorLength(1, encodingFormat::numeric));
    TEST_ASSERT_EQUAL(9, qrCode::characterCountIndicatorLength(1, encodingFormat::alphanumeric));
    TEST_ASSERT_EQUAL(8, qrCode::characterCountIndicatorLength(1, encodingFormat::byte));

    TEST_ASSERT_EQUAL(12, qrCode::characterCountIndicatorLength(10, encodingFormat::numeric));
    TEST_ASSERT_EQUAL(11, qrCode::characterCountIndicatorLength(10, encodingFormat::alphanumeric));
    TEST_ASSERT_EQUAL(16, qrCode::characterCountIndicatorLength(10, encodingFormat::byte));

    TEST_ASSERT_EQUAL(14, qrCode::characterCountIndicatorLength(40, encodingFormat::numeric));
    TEST_ASSERT_EQUAL(13, qrCode::characterCountIndicatorLength(40, encodingFormat::alphanumeric));
    TEST_ASSERT_EQUAL(16, qrCode::characterCountIndicatorLength(40, encodingFormat::byte));
}

void test_versionNeeded() {
    // numeric data
    const char testData1[]{"01234567890123456789012345678901234567890"};                           // 41 numeric characters
    TEST_ASSERT_EQUAL(1, qrCode::versionNeeded(testData1, errorCorrectionLevel::low));             //
    const char testData2[]{"012345678901234567890123456789012345678901"};                          // 42 numeric characters
    TEST_ASSERT_EQUAL(2, qrCode::versionNeeded(testData2, errorCorrectionLevel::low));             //
    const char testData3[]{"0123456789012345678901234567890123"};                                  // 34 numeric characters
    TEST_ASSERT_EQUAL(1, qrCode::versionNeeded(testData3, errorCorrectionLevel::medium));          //
    const char testData4[]{"01234567890123456789012345678901234"};                                 // 35 numeric characters
    TEST_ASSERT_EQUAL(2, qrCode::versionNeeded(testData4, errorCorrectionLevel::medium));          //
    const char testData5[]{"012345678901234567890123456"};                                         // 27 numeric characters
    TEST_ASSERT_EQUAL(1, qrCode::versionNeeded(testData5, errorCorrectionLevel::quartile));        //
    const char testData6[]{"0123456789012345678901234567"};                                        // 28 numeric characters
    TEST_ASSERT_EQUAL(2, qrCode::versionNeeded(testData6, errorCorrectionLevel::quartile));        //
    const char testData7[]{"01234567890123456"};                                                   // 17 numeric characters
    TEST_ASSERT_EQUAL(1, qrCode::versionNeeded(testData7, errorCorrectionLevel::high));            //
    const char testData8[]{"0123456789012345678"};                                                 // 18 numeric characters
    TEST_ASSERT_EQUAL(2, qrCode::versionNeeded(testData8, errorCorrectionLevel::high));            //

    // alfanumeric data
    const char testData10[]{"A123456789012345678901234"};                                           // 25 numeric characters
    TEST_ASSERT_EQUAL(1, qrCode::versionNeeded(testData10, errorCorrectionLevel::low));             //
    const char testData11[]{"A1234567890123456789012345"};                                          // 26 numeric characters
    TEST_ASSERT_EQUAL(2, qrCode::versionNeeded(testData11, errorCorrectionLevel::low));             //
    const char testData12[]{"A1234567890123456789"};                                                // 20 numeric characters
    TEST_ASSERT_EQUAL(1, qrCode::versionNeeded(testData12, errorCorrectionLevel::medium));          //
    const char testData13[]{"A12345678901234567890"};                                               // 21 numeric characters
    TEST_ASSERT_EQUAL(2, qrCode::versionNeeded(testData13, errorCorrectionLevel::medium));          //
    const char testData14[]{"A123456789012345"};                                                    // 16 numeric characters
    TEST_ASSERT_EQUAL(1, qrCode::versionNeeded(testData14, errorCorrectionLevel::quartile));        //
    const char testData15[]{"A1234567890123456"};                                                   // 17 numeric characters
    TEST_ASSERT_EQUAL(2, qrCode::versionNeeded(testData15, errorCorrectionLevel::quartile));        //
    const char testData16[]{"A123456789"};                                                          // 10 numeric characters
    TEST_ASSERT_EQUAL(1, qrCode::versionNeeded(testData16, errorCorrectionLevel::high));            //
    const char testData17[]{"A1234567890"};                                                         // 11 numeric characters
    TEST_ASSERT_EQUAL(2, qrCode::versionNeeded(testData17, errorCorrectionLevel::high));            //

    // byte data
    const char testData20[]{"#1234567890123456"};                                                   // 17 numeric characters
    TEST_ASSERT_EQUAL(1, qrCode::versionNeeded(testData20, errorCorrectionLevel::low));             //
    const char testData21[]{"#12345678901234567"};                                                  // 18 numeric characters
    TEST_ASSERT_EQUAL(2, qrCode::versionNeeded(testData21, errorCorrectionLevel::low));             //
    const char testData22[]{"#1234567890123"};                                                      // 14 numeric characters
    TEST_ASSERT_EQUAL(1, qrCode::versionNeeded(testData22, errorCorrectionLevel::medium));          //
    const char testData23[]{"#12345678901234"};                                                     // 15 numeric characters
    TEST_ASSERT_EQUAL(2, qrCode::versionNeeded(testData23, errorCorrectionLevel::medium));          //
    const char testData24[]{"#1234567890"};                                                         // 11 numeric characters
    TEST_ASSERT_EQUAL(1, qrCode::versionNeeded(testData24, errorCorrectionLevel::quartile));        //
    const char testData25[]{"#12345678901"};                                                        // 12 numeric characters
    TEST_ASSERT_EQUAL(2, qrCode::versionNeeded(testData25, errorCorrectionLevel::quartile));        //
    const char testData26[]{"#123456"};                                                             // 7 numeric characters
    TEST_ASSERT_EQUAL(1, qrCode::versionNeeded(testData26, errorCorrectionLevel::high));            //
    const char testData27[]{"#1234567"};                                                            // 8 numeric characters
    TEST_ASSERT_EQUAL(2, qrCode::versionNeeded(testData27, errorCorrectionLevel::high));            //

    // edge case
    // const char testData54[55]{"#12345678901234567890123456789012345678901234567890123"};
    // TEST_ASSERT_EQUAL(0, qrCode::versionNeeded(testData54, errorCorrectionLevel::low));        // This case still fails, need to investigate in detail why
}

void test_compress() {
    TEST_ASSERT_EQUAL(0, qrCode::compressNumeric('0'));
    TEST_ASSERT_EQUAL(1, qrCode::compressNumeric('1'));
    TEST_ASSERT_EQUAL(8, qrCode::compressNumeric('8'));
    TEST_ASSERT_EQUAL(9, qrCode::compressNumeric('9'));

    TEST_ASSERT_EQUAL(0, qrCode::compressAlphanumeric('0'));
    TEST_ASSERT_EQUAL(1, qrCode::compressAlphanumeric('1'));
    TEST_ASSERT_EQUAL(8, qrCode::compressAlphanumeric('8'));
    TEST_ASSERT_EQUAL(9, qrCode::compressAlphanumeric('9'));
    TEST_ASSERT_EQUAL(10, qrCode::compressAlphanumeric('A'));
    TEST_ASSERT_EQUAL(11, qrCode::compressAlphanumeric('B'));
    TEST_ASSERT_EQUAL(34, qrCode::compressAlphanumeric('Y'));
    TEST_ASSERT_EQUAL(35, qrCode::compressAlphanumeric('Z'));
    TEST_ASSERT_EQUAL(36, qrCode::compressAlphanumeric(' '));
    TEST_ASSERT_EQUAL(37, qrCode::compressAlphanumeric('$'));
    TEST_ASSERT_EQUAL(38, qrCode::compressAlphanumeric('%'));
    TEST_ASSERT_EQUAL(39, qrCode::compressAlphanumeric('*'));
    TEST_ASSERT_EQUAL(40, qrCode::compressAlphanumeric('+'));
    TEST_ASSERT_EQUAL(41, qrCode::compressAlphanumeric('-'));
    TEST_ASSERT_EQUAL(42, qrCode::compressAlphanumeric('.'));
    TEST_ASSERT_EQUAL(43, qrCode::compressAlphanumeric('/'));
    TEST_ASSERT_EQUAL(44, qrCode::compressAlphanumeric(':'));
}

void test_nmbrOfAlignmentPatterns() {
    TEST_ASSERT_EQUAL(0, qrCode::nmbrOfAlignmentPatterns(1));
    TEST_ASSERT_EQUAL(1, qrCode::nmbrOfAlignmentPatterns(2));
    TEST_ASSERT_EQUAL(1, qrCode::nmbrOfAlignmentPatterns(6));
    TEST_ASSERT_EQUAL(6, qrCode::nmbrOfAlignmentPatterns(7));
    TEST_ASSERT_EQUAL(6, qrCode::nmbrOfAlignmentPatterns(13));
    TEST_ASSERT_EQUAL(13, qrCode::nmbrOfAlignmentPatterns(14));
    TEST_ASSERT_EQUAL(13, qrCode::nmbrOfAlignmentPatterns(20));
    TEST_ASSERT_EQUAL(22, qrCode::nmbrOfAlignmentPatterns(21));
    TEST_ASSERT_EQUAL(22, qrCode::nmbrOfAlignmentPatterns(27));
    TEST_ASSERT_EQUAL(33, qrCode::nmbrOfAlignmentPatterns(28));
    TEST_ASSERT_EQUAL(33, qrCode::nmbrOfAlignmentPatterns(34));
    TEST_ASSERT_EQUAL(46, qrCode::nmbrOfAlignmentPatterns(35));
    TEST_ASSERT_EQUAL(46, qrCode::nmbrOfAlignmentPatterns(40));
}

void test_size() {
    TEST_ASSERT_EQUAL(21U, qrCode::size(1U));
    TEST_ASSERT_EQUAL(81U, qrCode::size(16U));
    TEST_ASSERT_EQUAL(177U, qrCode::size(40U));
}

void test_nmbrOfRawDataModules() {
    TEST_ASSERT_EQUAL(208, qrCode::nmbrOfDataModules(1));
    TEST_ASSERT_EQUAL(359, qrCode::nmbrOfDataModules(2));
    TEST_ASSERT_EQUAL(567, qrCode::nmbrOfDataModules(3));
    TEST_ASSERT_EQUAL(807, qrCode::nmbrOfDataModules(4));
    TEST_ASSERT_EQUAL(1079, qrCode::nmbrOfDataModules(5));
    TEST_ASSERT_EQUAL(1383, qrCode::nmbrOfDataModules(6));
    TEST_ASSERT_EQUAL(1568, qrCode::nmbrOfDataModules(7));
    TEST_ASSERT_EQUAL(1936, qrCode::nmbrOfDataModules(8));
    TEST_ASSERT_EQUAL(2336, qrCode::nmbrOfDataModules(9));
    TEST_ASSERT_EQUAL(2768, qrCode::nmbrOfDataModules(10));
    TEST_ASSERT_EQUAL(3232, qrCode::nmbrOfDataModules(11));
    TEST_ASSERT_EQUAL(3728, qrCode::nmbrOfDataModules(12));
    TEST_ASSERT_EQUAL(4256, qrCode::nmbrOfDataModules(13));
    TEST_ASSERT_EQUAL(4651, qrCode::nmbrOfDataModules(14));
    TEST_ASSERT_EQUAL(5243, qrCode::nmbrOfDataModules(15));
    TEST_ASSERT_EQUAL(5867, qrCode::nmbrOfDataModules(16));
}

void test_nmbrOfErrorCorrectionModules() {
    TEST_ASSERT_EQUAL(7 * 1 * 8, qrCode::nmbrOfErrorCorrectionModules(1, errorCorrectionLevel::low));
    TEST_ASSERT_EQUAL(10 * 1 * 8, qrCode::nmbrOfErrorCorrectionModules(1, errorCorrectionLevel::medium));
    TEST_ASSERT_EQUAL(13 * 1 * 8, qrCode::nmbrOfErrorCorrectionModules(1, errorCorrectionLevel::quartile));
    TEST_ASSERT_EQUAL(17 * 1 * 8, qrCode::nmbrOfErrorCorrectionModules(1, errorCorrectionLevel::high));

    TEST_ASSERT_EQUAL(10 * 1 * 8, qrCode::nmbrOfErrorCorrectionModules(2, errorCorrectionLevel::low));
    TEST_ASSERT_EQUAL(16 * 1 * 8, qrCode::nmbrOfErrorCorrectionModules(2, errorCorrectionLevel::medium));
    TEST_ASSERT_EQUAL(22 * 1 * 8, qrCode::nmbrOfErrorCorrectionModules(2, errorCorrectionLevel::quartile));
    TEST_ASSERT_EQUAL(28 * 1 * 8, qrCode::nmbrOfErrorCorrectionModules(2, errorCorrectionLevel::high));

    TEST_ASSERT_EQUAL(15 * 1 * 8, qrCode::nmbrOfErrorCorrectionModules(3, errorCorrectionLevel::low));
    TEST_ASSERT_EQUAL(26 * 1 * 8, qrCode::nmbrOfErrorCorrectionModules(3, errorCorrectionLevel::medium));
    TEST_ASSERT_EQUAL(18 * 2 * 8, qrCode::nmbrOfErrorCorrectionModules(3, errorCorrectionLevel::quartile));
    TEST_ASSERT_EQUAL(22 * 2 * 8, qrCode::nmbrOfErrorCorrectionModules(3, errorCorrectionLevel::high));
}

// * nmbrOfDataModules
// * nmbrOfTotalModules
// * nmbrOfFunctionModules

void test_getModeBits() {
    TEST_ASSERT_EQUAL(0b0001, qrCode::modeIndicator(encodingFormat::numeric));
    TEST_ASSERT_EQUAL(0b0010, qrCode::modeIndicator(encodingFormat::alphanumeric));
    TEST_ASSERT_EQUAL(0b0100, qrCode::modeIndicator(encodingFormat::byte));
}

void test_calculatePayloadLength() {
    TEST_ASSERT_EQUAL(41, qrCode::payloadLengthInBits(8, 1, encodingFormat::numeric));
    TEST_ASSERT_EQUAL(57, qrCode::payloadLengthInBits(8, 1, encodingFormat::alphanumeric));
    TEST_ASSERT_EQUAL(76, qrCode::payloadLengthInBits(8, 1, encodingFormat::byte));

    TEST_ASSERT_EQUAL(151, qrCode::payloadLengthInBits(41, 1, encodingFormat::numeric));
    TEST_ASSERT_EQUAL(151, qrCode::payloadLengthInBits(25, 1, encodingFormat::alphanumeric));
    TEST_ASSERT_EQUAL(148, qrCode::payloadLengthInBits(17, 1, encodingFormat::byte));
}

void test_encodeData() {
    qrCode::setVersion(1);
    qrCode::setErrorCorrectionLevel(errorCorrectionLevel::low);
    qrCode::encodeData("01234567");

    bitVector<qrCode::maxSize> expected;
    expected.reset();
    expected.appendBits(0b00010000'00100000'00001100'01010110, 32);
    expected.appendBits(0b01100001'1, 9);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected.data, qrCode::buffer.data, qrCode::buffer.lengthInBytes);

    qrCode::encodeData("AC-42");
    expected.reset();
    expected.appendBits(0b00100000'00101001'11001110'11100111, 32);
    expected.appendBits(0b00100001'0, 9);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected.data, qrCode::buffer.data, qrCode::buffer.lengthInBytes);

    // TODO : add test for byte encodingFormat
}

void test_alignmentPatternSpacing() {
    // Version 1 has no alignment patterns and so also no spacing
    TEST_ASSERT_EQUAL(18 - 6, qrCode::alignmentPatternSpacing(2));
    TEST_ASSERT_EQUAL(22 - 6, qrCode::alignmentPatternSpacing(3));
    TEST_ASSERT_EQUAL(26 - 6, qrCode::alignmentPatternSpacing(4));
    TEST_ASSERT_EQUAL(30 - 6, qrCode::alignmentPatternSpacing(5));
    TEST_ASSERT_EQUAL(34 - 6, qrCode::alignmentPatternSpacing(6));

    TEST_ASSERT_EQUAL(38 - 22, qrCode::alignmentPatternSpacing(7));
    TEST_ASSERT_EQUAL(42 - 24, qrCode::alignmentPatternSpacing(8));
    TEST_ASSERT_EQUAL(46 - 26, qrCode::alignmentPatternSpacing(9));
    TEST_ASSERT_EQUAL(50 - 28, qrCode::alignmentPatternSpacing(10));
    TEST_ASSERT_EQUAL(54 - 30, qrCode::alignmentPatternSpacing(11));
    TEST_ASSERT_EQUAL(58 - 32, qrCode::alignmentPatternSpacing(12));
    TEST_ASSERT_EQUAL(62 - 34, qrCode::alignmentPatternSpacing(13));

    TEST_ASSERT_EQUAL(66 - 46, qrCode::alignmentPatternSpacing(14));
    TEST_ASSERT_EQUAL(46 - 26, qrCode::alignmentPatternSpacing(14));
    TEST_ASSERT_EQUAL(70 - 48, qrCode::alignmentPatternSpacing(15));
    TEST_ASSERT_EQUAL(48 - 26, qrCode::alignmentPatternSpacing(15));
    TEST_ASSERT_EQUAL(74 - 50, qrCode::alignmentPatternSpacing(16));
    TEST_ASSERT_EQUAL(50 - 26, qrCode::alignmentPatternSpacing(16));
    TEST_ASSERT_EQUAL(78 - 54, qrCode::alignmentPatternSpacing(17));
    TEST_ASSERT_EQUAL(54 - 30, qrCode::alignmentPatternSpacing(17));
    TEST_ASSERT_EQUAL(82 - 56, qrCode::alignmentPatternSpacing(18));
    TEST_ASSERT_EQUAL(56 - 30, qrCode::alignmentPatternSpacing(18));
    TEST_ASSERT_EQUAL(86 - 58, qrCode::alignmentPatternSpacing(19));
    TEST_ASSERT_EQUAL(58 - 30, qrCode::alignmentPatternSpacing(19));
    TEST_ASSERT_EQUAL(90 - 62, qrCode::alignmentPatternSpacing(20));
    TEST_ASSERT_EQUAL(62 - 34, qrCode::alignmentPatternSpacing(20));

    // TODO : complete for higher versions

    TEST_ASSERT_EQUAL(138 - 112, qrCode::alignmentPatternSpacing(32));
    TEST_ASSERT_EQUAL(112 - 86, qrCode::alignmentPatternSpacing(32));
    TEST_ASSERT_EQUAL(86 - 60, qrCode::alignmentPatternSpacing(32));
    TEST_ASSERT_EQUAL(60 - 34, qrCode::alignmentPatternSpacing(32));

    TEST_ASSERT_EQUAL(170 - 142, qrCode::alignmentPatternSpacing(40));
    TEST_ASSERT_EQUAL(142 - 114, qrCode::alignmentPatternSpacing(40));
    TEST_ASSERT_EQUAL(114 - 86, qrCode::alignmentPatternSpacing(40));
    TEST_ASSERT_EQUAL(86 - 58, qrCode::alignmentPatternSpacing(40));
    TEST_ASSERT_EQUAL(58 - 30, qrCode::alignmentPatternSpacing(40));
}

void test_alignmentPatternCoordinates() {
    TEST_ASSERT_EQUAL(6, qrCode::alignmentPatternCoordinate(2, 0));
    TEST_ASSERT_EQUAL(18, qrCode::alignmentPatternCoordinate(2, 1));
    TEST_ASSERT_EQUAL(6, qrCode::alignmentPatternCoordinate(7, 0));
    TEST_ASSERT_EQUAL(22, qrCode::alignmentPatternCoordinate(7, 1));
    TEST_ASSERT_EQUAL(38, qrCode::alignmentPatternCoordinate(7, 2));
    TEST_ASSERT_EQUAL(6, qrCode::alignmentPatternCoordinate(32, 0));
    TEST_ASSERT_EQUAL(34, qrCode::alignmentPatternCoordinate(32, 1));
    TEST_ASSERT_EQUAL(60, qrCode::alignmentPatternCoordinate(32, 2));
    TEST_ASSERT_EQUAL(86, qrCode::alignmentPatternCoordinate(32, 3));
    TEST_ASSERT_EQUAL(112, qrCode::alignmentPatternCoordinate(32, 4));
    TEST_ASSERT_EQUAL(138, qrCode::alignmentPatternCoordinate(32, 5));
}

void test_isFinderPatternOrSeparator() {
    TEST_ASSERT_TRUE(qrCode::isFinderPatternOrSeparator(0, 0, 1));
    TEST_ASSERT_TRUE(qrCode::isFinderPatternOrSeparator(0, 7, 1));
    TEST_ASSERT_TRUE(qrCode::isFinderPatternOrSeparator(7, 0, 1));

    TEST_ASSERT_TRUE(qrCode::isFinderPatternOrSeparator(20, 0, 1));
    TEST_ASSERT_TRUE(qrCode::isFinderPatternOrSeparator(2, 20, 1));
    TEST_ASSERT_FALSE(qrCode::isFinderPatternOrSeparator(20, 20, 1));
}

void test_isTimingPattern() {
    qrCode::setVersion(1);
    TEST_ASSERT_TRUE(qrCode::isTimingPattern(6, 8));
    TEST_ASSERT_TRUE(qrCode::isTimingPattern(6, 12));
    TEST_ASSERT_TRUE(qrCode::isTimingPattern(8, 6));
    TEST_ASSERT_TRUE(qrCode::isTimingPattern(12, 6));
}

void test_isFormatInformation() {
    TEST_ASSERT_TRUE(qrCode::isFormatInformation(0, 8, 1));
}

void test_isVersionInformation() {
    // problem, maxVersion is < 7 and version info is only available from version 7
    // TEST_ASSERT_TRUE(qrCode::isVersionInformation(0, 35, 7));
}

void test_isAlignmentPattern() {
    TEST_ASSERT_TRUE(qrCode::isAlignmentPattern(18, 18, 2));
    TEST_ASSERT_TRUE(qrCode::isAlignmentPattern(16, 16, 2));
    TEST_ASSERT_TRUE(qrCode::isAlignmentPattern(20, 20, 2));
    TEST_ASSERT_FALSE(qrCode::isAlignmentPattern(15, 15, 2));
    TEST_ASSERT_FALSE(qrCode::isAlignmentPattern(21, 21, 2));

    qrCode::setVersion(1);
    TEST_ASSERT_FALSE(qrCode::isAlignmentPattern(18, 18, 1));        // Version 1 does not have alignment patterns
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_isNumeric);
    RUN_TEST(test_isAlphanumeric);
    RUN_TEST(test_getEncodingFormat);
    RUN_TEST(test_payloadLengthInBits),
        RUN_TEST(test_encodeData);

    RUN_TEST(test_size);
    RUN_TEST(test_nmbrOfAlignmentPatterns);

    RUN_TEST(test_compress);
    RUN_TEST(test_getModeBits);
    RUN_TEST(test_characterCountIndicatorLength);
    RUN_TEST(test_calculatePayloadLength);
    RUN_TEST(test_alignmentPatternSpacing);
    RUN_TEST(test_alignmentPatternCoordinates);
    RUN_TEST(test_nmbrOfRawDataModules);
    RUN_TEST(test_nmbrOfErrorCorrectionModules);
    RUN_TEST(test_versionNeeded);
    RUN_TEST(test_isFinderPatternOrSeparator);
    RUN_TEST(test_isTimingPattern);
    RUN_TEST(test_isFormatInformation);
    RUN_TEST(test_isVersionInformation);
    RUN_TEST(test_isAlignmentPattern);
    UNITY_END();
}
