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
    TEST_ASSERT_TRUE(qrCode::isValid(1));
    TEST_ASSERT_TRUE(qrCode::isValid(qrCode::maxVersion));
    TEST_ASSERT_FALSE(qrCode::isValid(0));
    TEST_ASSERT_FALSE(qrCode::isValid(qrCode::maxVersion + 1));
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
    TEST_ASSERT_EQUAL(41, qrCode::payloadLengthInBits(5, 1, encodingFormat::alphanumeric));
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
    UNITY_END();
}

// void test_nmbrDataModulesAvailable() {
//     // Based on info from https://www.qrcode.com/en/about/version.html
//     TEST_ASSERT_EQUAL(152, qrCode::nmbrDataModulesAvailable(1, errorCorrectionLevel::low));
//     TEST_ASSERT_EQUAL(128, qrCode::nmbrDataModulesAvailable(1, errorCorrectionLevel::medium));
//     TEST_ASSERT_EQUAL(104, qrCode::nmbrDataModulesAvailable(1, errorCorrectionLevel::quartile));
//     TEST_ASSERT_EQUAL(72, qrCode::nmbrDataModulesAvailable(1, errorCorrectionLevel::high));

//     TEST_ASSERT_EQUAL(272, qrCode::nmbrDataModulesAvailable(2, errorCorrectionLevel::low));
//     TEST_ASSERT_EQUAL(224, qrCode::nmbrDataModulesAvailable(2, errorCorrectionLevel::medium));
//     TEST_ASSERT_EQUAL(176, qrCode::nmbrDataModulesAvailable(2, errorCorrectionLevel::quartile));
//     TEST_ASSERT_EQUAL(128, qrCode::nmbrDataModulesAvailable(2, errorCorrectionLevel::high));

//     TEST_ASSERT_EQUAL(2192, qrCode::nmbrDataModulesAvailable(10, errorCorrectionLevel::low));
//     TEST_ASSERT_EQUAL(1728, qrCode::nmbrDataModulesAvailable(10, errorCorrectionLevel::medium));
//     TEST_ASSERT_EQUAL(1232, qrCode::nmbrDataModulesAvailable(10, errorCorrectionLevel::quartile));
//     TEST_ASSERT_EQUAL(976, qrCode::nmbrDataModulesAvailable(10, errorCorrectionLevel::high));
// }
