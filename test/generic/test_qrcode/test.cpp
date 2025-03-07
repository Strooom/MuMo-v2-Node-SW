// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include <qrcode.hpp>
#include <string.h>

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

#pragma region staticFunctions

void test_isValidVersion() {
    TEST_ASSERT_TRUE(qrCode::isValidVersion(1));
    TEST_ASSERT_TRUE(qrCode::isValidVersion(qrCode::maxVersion));
    TEST_ASSERT_FALSE(qrCode::isValidVersion(0));
    TEST_ASSERT_FALSE(qrCode::isValidVersion(qrCode::maxVersion + 1));
}

void test_size() {
    TEST_ASSERT_EQUAL(21U, qrCode::size(1U));
}

void test_isNumeric() {
    TEST_ASSERT_TRUE(qrCode::isNumeric('0'));
    TEST_ASSERT_TRUE(qrCode::isNumeric('1'));
    TEST_ASSERT_TRUE(qrCode::isNumeric('8'));
    TEST_ASSERT_TRUE(qrCode::isNumeric('9'));
    TEST_ASSERT_FALSE(qrCode::isNumeric('/'));
    TEST_ASSERT_FALSE(qrCode::isNumeric(':'));

    TEST_ASSERT_TRUE(qrCode::isNumeric("0123456789"));
    TEST_ASSERT_FALSE(qrCode::isNumeric("0/1"));
    TEST_ASSERT_FALSE(qrCode::isNumeric("0:1"));

    const char testData[4]{'0', '1', '2', 'A'};
    TEST_ASSERT_TRUE(qrCode::isNumeric(testData, 3));
    TEST_ASSERT_FALSE(qrCode::isNumeric(testData, 4));
}

void test_isAlphanumeric() {
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

    TEST_ASSERT_TRUE(qrCode::isAlphanumeric("0123456789ABCDEF"));
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric("HTTPS://WWW.STROOOM.BE"));
    TEST_ASSERT_FALSE(qrCode::isAlphanumeric("HTTPS://WWW.STROOOM.BE?"));
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

void test_getModeBits() {
    TEST_ASSERT_EQUAL(0b0001, qrCode::modeIndicator(encodingFormat::numeric));
    TEST_ASSERT_EQUAL(0b0010, qrCode::modeIndicator(encodingFormat::alphanumeric));
    TEST_ASSERT_EQUAL(0b0100, qrCode::modeIndicator(encodingFormat::byte));
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

void test_calculatePayloadLength() {
    TEST_ASSERT_EQUAL(41, qrCode::payloadLengthInBits(8, 1, encodingFormat::numeric));
    TEST_ASSERT_EQUAL(57, qrCode::payloadLengthInBits(8, 1, encodingFormat::alphanumeric));
    TEST_ASSERT_EQUAL(76, qrCode::payloadLengthInBits(8, 1, encodingFormat::byte));

    TEST_ASSERT_EQUAL(151, qrCode::payloadLengthInBits(41, 1, encodingFormat::numeric));
    TEST_ASSERT_EQUAL(151, qrCode::payloadLengthInBits(25, 1, encodingFormat::alphanumeric));
    TEST_ASSERT_EQUAL(148, qrCode::payloadLengthInBits(17, 1, encodingFormat::byte));
}

void test_encodeDataNumeric() {
    const uint32_t testVersion{1};
    const char testData[]{"01234567"};
    const uint32_t testDataLength{strlen(testData)};
    const uint32_t testPayloadLengthInBits{qrCode::payloadLengthInBits(testDataLength, testVersion, encodingFormat::numeric)};
    const uint32_t testPayloadLengthInBytes{bitVector::neededLengthInBytes(testPayloadLengthInBits)};
    uint8_t storage[testPayloadLengthInBytes]{};
    bitVector theBitVector{storage, testPayloadLengthInBits};
    qrCode::encodeData(theBitVector, testData, testVersion, encodingFormat::numeric);

    uint8_t expectedStorage[testPayloadLengthInBytes]{};
    bitVector expected(expectedStorage, testPayloadLengthInBits);
    expected.appendBits(0b00010000'00100000'00001100'01010110, 32);
    expected.appendBits(0b01100001'1, 9);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedStorage, storage, testPayloadLengthInBytes);
}

void test_encodeDataAlphaNumeric() {
    const uint32_t testVersion{1};
    const char testData[]{"AC-42"};
    const uint32_t testDataLength{strlen(testData)};
    const uint32_t testPayloadLengthInBits{qrCode::payloadLengthInBits(testDataLength, testVersion, encodingFormat::alphanumeric)};
    const uint32_t testPayloadLengthInBytes{bitVector::neededLengthInBytes(testPayloadLengthInBits)};
    uint8_t storage[testPayloadLengthInBytes]{};
    bitVector theBitVector{storage, testPayloadLengthInBits};
    qrCode::encodeData(theBitVector, testData, testVersion, encodingFormat::alphanumeric);

    uint8_t expectedStorage[testPayloadLengthInBytes]{};
    bitVector expected(expectedStorage, testPayloadLengthInBits);
    expected.appendBits(0b00100000'00101001'11001110'11100111, 32);
    expected.appendBits(0b00100001'0, 9);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedStorage, storage, testPayloadLengthInBytes);
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

void test_alignmentPatternSpacing() {
    // Version 1 has no alignment patterns
    TEST_ASSERT_EQUAL(18-6, qrCode::alignmentPatternSpacing(2));
    TEST_ASSERT_EQUAL(22-6, qrCode::alignmentPatternSpacing(3));
    TEST_ASSERT_EQUAL(26-6, qrCode::alignmentPatternSpacing(4));
    TEST_ASSERT_EQUAL(30-6, qrCode::alignmentPatternSpacing(5));
    TEST_ASSERT_EQUAL(34-6, qrCode::alignmentPatternSpacing(6));

    TEST_ASSERT_EQUAL(38-22, qrCode::alignmentPatternSpacing(7));
    TEST_ASSERT_EQUAL(42-24, qrCode::alignmentPatternSpacing(8));
    TEST_ASSERT_EQUAL(46-26, qrCode::alignmentPatternSpacing(9));
    TEST_ASSERT_EQUAL(50-28, qrCode::alignmentPatternSpacing(10));
    TEST_ASSERT_EQUAL(54-30, qrCode::alignmentPatternSpacing(11));
    TEST_ASSERT_EQUAL(58-32, qrCode::alignmentPatternSpacing(12));
    TEST_ASSERT_EQUAL(62-34, qrCode::alignmentPatternSpacing(13));

    TEST_ASSERT_EQUAL(66-46, qrCode::alignmentPatternSpacing(14));
    TEST_ASSERT_EQUAL(46-26, qrCode::alignmentPatternSpacing(14));
    TEST_ASSERT_EQUAL(70-48, qrCode::alignmentPatternSpacing(15));
    TEST_ASSERT_EQUAL(48-26, qrCode::alignmentPatternSpacing(15));
    TEST_ASSERT_EQUAL(74-50, qrCode::alignmentPatternSpacing(16));
    TEST_ASSERT_EQUAL(50-26, qrCode::alignmentPatternSpacing(16));
    TEST_ASSERT_EQUAL(78-54, qrCode::alignmentPatternSpacing(17));
    TEST_ASSERT_EQUAL(54-30, qrCode::alignmentPatternSpacing(17));
    TEST_ASSERT_EQUAL(82-56, qrCode::alignmentPatternSpacing(18));
    TEST_ASSERT_EQUAL(56-30, qrCode::alignmentPatternSpacing(18));
    TEST_ASSERT_EQUAL(86-58, qrCode::alignmentPatternSpacing(19));
    TEST_ASSERT_EQUAL(58-30, qrCode::alignmentPatternSpacing(19));
    TEST_ASSERT_EQUAL(90-62, qrCode::alignmentPatternSpacing(20));
    TEST_ASSERT_EQUAL(62-34, qrCode::alignmentPatternSpacing(20));

// TODO : complete for higher versions


    TEST_ASSERT_EQUAL(138-112, qrCode::alignmentPatternSpacing(32));
    TEST_ASSERT_EQUAL(112-86, qrCode::alignmentPatternSpacing(32));
    TEST_ASSERT_EQUAL(86-60, qrCode::alignmentPatternSpacing(32));
    TEST_ASSERT_EQUAL(60-34, qrCode::alignmentPatternSpacing(32));

    TEST_ASSERT_EQUAL(170-142, qrCode::alignmentPatternSpacing(40));
    TEST_ASSERT_EQUAL(142-114, qrCode::alignmentPatternSpacing(40));
    TEST_ASSERT_EQUAL(114-86, qrCode::alignmentPatternSpacing(40));
    TEST_ASSERT_EQUAL(86-58, qrCode::alignmentPatternSpacing(40));
    TEST_ASSERT_EQUAL(58-30, qrCode::alignmentPatternSpacing(40));
}

void test_alignmentPatternCoordinates() {
    TEST_ASSERT_EQUAL(6, qrCode::alignmentPatternCoordinate(0,2));
    TEST_ASSERT_EQUAL(18, qrCode::alignmentPatternCoordinate(1,2));

    TEST_ASSERT_EQUAL(6, qrCode::alignmentPatternCoordinate(0,7));
    TEST_ASSERT_EQUAL(22, qrCode::alignmentPatternCoordinate(1,7));
    TEST_ASSERT_EQUAL(38, qrCode::alignmentPatternCoordinate(2,7));

    TEST_ASSERT_EQUAL(6, qrCode::alignmentPatternCoordinate(0,32));
    TEST_ASSERT_EQUAL(34, qrCode::alignmentPatternCoordinate(1,32));
    TEST_ASSERT_EQUAL(60, qrCode::alignmentPatternCoordinate(2,32));
    TEST_ASSERT_EQUAL(86, qrCode::alignmentPatternCoordinate(3,32));
    TEST_ASSERT_EQUAL(112, qrCode::alignmentPatternCoordinate(4,32));
    TEST_ASSERT_EQUAL(138, qrCode::alignmentPatternCoordinate(5,32));

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

void test_versionNeeded() {
    TEST_ASSERT_EQUAL(1, qrCode::versionNeeded(encodingFormat::numeric, 41, errorCorrectionLevel::low));
    TEST_ASSERT_EQUAL(2, qrCode::versionNeeded(encodingFormat::numeric, 42, errorCorrectionLevel::low));
    TEST_ASSERT_EQUAL(1, qrCode::versionNeeded(encodingFormat::alphanumeric, 25, errorCorrectionLevel::low));
    TEST_ASSERT_EQUAL(2, qrCode::versionNeeded(encodingFormat::alphanumeric, 26, errorCorrectionLevel::low));
    TEST_ASSERT_EQUAL(1, qrCode::versionNeeded(encodingFormat::byte, 17, errorCorrectionLevel::low));
    TEST_ASSERT_EQUAL(2, qrCode::versionNeeded(encodingFormat::byte, 18, errorCorrectionLevel::low));

    TEST_ASSERT_EQUAL(1, qrCode::versionNeeded(encodingFormat::numeric, 34, errorCorrectionLevel::medium));
    TEST_ASSERT_EQUAL(2, qrCode::versionNeeded(encodingFormat::numeric, 35, errorCorrectionLevel::medium));
    TEST_ASSERT_EQUAL(1, qrCode::versionNeeded(encodingFormat::alphanumeric, 20, errorCorrectionLevel::medium));
    TEST_ASSERT_EQUAL(2, qrCode::versionNeeded(encodingFormat::alphanumeric, 21, errorCorrectionLevel::medium));
    TEST_ASSERT_EQUAL(1, qrCode::versionNeeded(encodingFormat::byte, 14, errorCorrectionLevel::medium));
    TEST_ASSERT_EQUAL(2, qrCode::versionNeeded(encodingFormat::byte, 15, errorCorrectionLevel::medium));

    TEST_ASSERT_EQUAL(1, qrCode::versionNeeded(encodingFormat::numeric, 27, errorCorrectionLevel::quartile));
    TEST_ASSERT_EQUAL(2, qrCode::versionNeeded(encodingFormat::numeric, 28, errorCorrectionLevel::quartile));
    TEST_ASSERT_EQUAL(1, qrCode::versionNeeded(encodingFormat::alphanumeric, 16, errorCorrectionLevel::quartile));
    TEST_ASSERT_EQUAL(2, qrCode::versionNeeded(encodingFormat::alphanumeric, 17, errorCorrectionLevel::quartile));
    TEST_ASSERT_EQUAL(1, qrCode::versionNeeded(encodingFormat::byte, 11, errorCorrectionLevel::quartile));
    TEST_ASSERT_EQUAL(2, qrCode::versionNeeded(encodingFormat::byte, 12, errorCorrectionLevel::quartile));

    TEST_ASSERT_EQUAL(1, qrCode::versionNeeded(encodingFormat::numeric, 17, errorCorrectionLevel::high));
    TEST_ASSERT_EQUAL(2, qrCode::versionNeeded(encodingFormat::numeric, 18, errorCorrectionLevel::high));
    TEST_ASSERT_EQUAL(1, qrCode::versionNeeded(encodingFormat::alphanumeric, 10, errorCorrectionLevel::high));
    TEST_ASSERT_EQUAL(2, qrCode::versionNeeded(encodingFormat::alphanumeric, 11, errorCorrectionLevel::high));
    TEST_ASSERT_EQUAL(1, qrCode::versionNeeded(encodingFormat::byte, 7, errorCorrectionLevel::high));
    TEST_ASSERT_EQUAL(2, qrCode::versionNeeded(encodingFormat::byte, 8, errorCorrectionLevel::high));
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


#pragma endregion

#pragma region testHelpers

#pragma endregion

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_isValidVersion);
    RUN_TEST(test_size);
    RUN_TEST(test_isNumeric);
    RUN_TEST(test_isAlphanumeric);
    RUN_TEST(test_compress);
    RUN_TEST(test_getModeBits);
    RUN_TEST(test_characterCountIndicatorLength);
    RUN_TEST(test_calculatePayloadLength);
    RUN_TEST(test_encodeDataNumeric);
    RUN_TEST(test_encodeDataAlphaNumeric);
    RUN_TEST(test_nmbrOfAlignmentPatterns);
    RUN_TEST(test_alignmentPatternSpacing);
    RUN_TEST(test_alignmentPatternCoordinates);
    RUN_TEST(test_nmbrOfRawDataModules);
    RUN_TEST(test_nmbrOfErrorCorrectionModules);
    RUN_TEST(test_versionNeeded);
    UNITY_END();
}
