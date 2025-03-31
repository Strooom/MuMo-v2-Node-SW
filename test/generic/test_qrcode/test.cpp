// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include <qrcode.hpp>
#include <string.h>
#include <reedsolomon.hpp>
#include <stdio.h>

void setUp(void) {}
void tearDown(void) {}

#pragma region testing internal data initialization / setting

void test_initialization() {
    static constexpr uint32_t testVersion{1};
    const uint32_t size = qrCode::size(testVersion);
    qrCode::setVersion(testVersion);
    TEST_ASSERT_EQUAL(testVersion, qrCode::theVersion);
    TEST_ASSERT_EQUAL(size, qrCode::modules.getWidthHeightInBits());
    TEST_ASSERT_EQUAL(size, qrCode::isData.getWidthHeightInBits());
    static const errorCorrectionLevel testErrorCorrectionLevel{errorCorrectionLevel::low};
    qrCode::setErrorCorrectionLevel(testErrorCorrectionLevel);
    TEST_ASSERT_EQUAL(testErrorCorrectionLevel, qrCode::theErrorCorrectionLevel);

    TEST_ASSERT_EACH_EQUAL_UINT8(0, qrCode::modules.data, qrCode::modules.getSizeInBytes());
    TEST_ASSERT_EACH_EQUAL_UINT8(0xFF, qrCode::isData.data, qrCode::isData.getSizeInBytes());
    TEST_ASSERT_EACH_EQUAL_UINT8(0, qrCode::buffer.data, qrCode::buffer.lengthInBytes);
}
#pragma endregion
#pragma region testing internal helpers

void test_size() {
    TEST_ASSERT_EQUAL(21U, qrCode::size(1U));
    TEST_ASSERT_EQUAL(81U, qrCode::size(16U));
    TEST_ASSERT_EQUAL(177U, qrCode::size(40U));
}

void test_setVersion() {
    static constexpr uint32_t testVersion{1};
    qrCode::setVersion(testVersion);
    TEST_ASSERT_EQUAL(testVersion, qrCode::theVersion);
}

void test_setErrorCorrectionLevel() {
    static const errorCorrectionLevel testErrorCorrectionLevel{errorCorrectionLevel::low};
    qrCode::setErrorCorrectionLevel(testErrorCorrectionLevel);
    TEST_ASSERT_EQUAL(testErrorCorrectionLevel, qrCode::theErrorCorrectionLevel);
}
#pragma endregion
#pragma region testing encoding user data into payload
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

void test_encodeData() {
    // used https://www.nayuki.io/page/creating-a-qr-code-step-by-step to create test-vectors

    // 1. Numeric encoding mode
    {
        qrCode::setVersion(1);
        qrCode::setErrorCorrectionLevel(errorCorrectionLevel::low);
        qrCode::encodeData("0123456789");

        bitVector<200U> expected;
        expected.reset();
        expected.appendBits(0b0001, 4);                                      // set mode to numeric - always 4 bits
        expected.appendBits(0b0000001010, 10);                               // count - 10 bits
        expected.appendBits(0b00000011000101011001101010011010, 32);         // data 1/2
        expected.appendBits(0b0000001100010101100110101001101001, 2);        // data 2/2
        expected.appendBits(0b0000, 4);                                      // terminator
        expected.appendBits(0b0000, 4);                                      // bit padding
        expected.appendBits(0b11101100000100011110110000010001, 32);         // byte padding
        expected.appendBits(0b11101100000100011110110000010001, 32);         // byte padding
        expected.appendBits(0b11101100000100011110110000010001, 32);         // byte padding

        TEST_ASSERT_EQUAL(152U, qrCode::buffer.levelInBits());
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expected.data, qrCode::buffer.data, qrCode::buffer.levelInBytes());
    }
    {
        // 2. Alphanumeric encoding mode

        qrCode::setVersion(1);
        qrCode::setErrorCorrectionLevel(errorCorrectionLevel::low);
        qrCode::encodeData("PROJECT NAYUKI");

        bitVector<200U> expected;
        expected.reset();
        expected.appendBits(0b0010, 4);                                     // set mode to alfa-numeric - always 4 bits
        expected.appendBits(0b000001110, 9);                                // count - 9 bits
        expected.appendBits(0b10010000000100010010110101000001, 32);        // data 1/3
        expected.appendBits(0b01010011110110000010101110000110, 32);        // data 2/3
        expected.appendBits(0b0001110010110, 13);                           // data 3/3

        expected.appendBits(0b0000, 4);                                     // terminator
        expected.appendBits(0b00, 2);                                       // bit padding
        expected.appendBits(0b11101100000100011110110000010001, 32);        // byte padding
        expected.appendBits(0b111011000001000111101100, 24);                // byte padding

        TEST_ASSERT_EQUAL(152U, qrCode::buffer.levelInBits());
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expected.data, qrCode::buffer.data, qrCode::buffer.levelInBytes());
    }
    {
        // 3. Byte encoding mode

        qrCode::setVersion(2);
        qrCode::setErrorCorrectionLevel(errorCorrectionLevel::low);
        qrCode::encodeData("https://github.com/Strooom");

        bitVector<300U> expected;
        expected.reset();
        expected.appendBits(0b0100, 4);                                     // set mode to byte - always 4 bits
        expected.appendBits(0b00011010, 8);                                 // count - 8 bits
        expected.appendBits(0b01101000011101000111010001110000, 32);        // data 1/7
        expected.appendBits(0b01110011001110100010111100101111, 32);        // data 2/7
        expected.appendBits(0b01100111011010010111010001101000, 32);        // data 3/7
        expected.appendBits(0b01110101011000100010111001100011, 32);        // data 4/7
        expected.appendBits(0b01101111011011010010111101010011, 32);        // data 5/7
        expected.appendBits(0b01110100011100100110111101101111, 32);        // data 6/7
        expected.appendBits(0b0110111101101101, 16);                        // data 7/7

        expected.appendBits(0b0000, 4);                                     // terminator
        expected.appendBits(0b11101100000100011110110000010001, 32);        // byte padding
        expected.appendBits(0b1110110000010001, 16);                        // byte padding

        TEST_ASSERT_EQUAL(272U, qrCode::buffer.levelInBits());
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expected.data, qrCode::buffer.data, qrCode::buffer.levelInBytes());
    }
    {
        qrCode::setVersion(5);
        qrCode::setErrorCorrectionLevel(errorCorrectionLevel::quartile);
        qrCode::encodeData("https://en.wikipedia.org/wiki/QR_code#Error_correction");
        TEST_ASSERT_EQUAL(62U, qrCode::buffer.levelInBytes());
    }

    // Edge Cases Tests : Bit padding bits 0..7
    // 0 bits padded
    {
        qrCode::setVersion(1);
        qrCode::setErrorCorrectionLevel(errorCorrectionLevel::high);
        qrCode::encodeData("0123");

        bitVector<200U> expected;
        expected.reset();
        expected.appendBits(0b0001, 4);                                     // set mode to numeric - always 4 bits
        expected.appendBits(0b0000000100, 10);                              // count - 10 bits
        expected.appendBits(0b00000011000011, 14);                          // data
        expected.appendBits(0b0000, 4);                                     // terminator
        expected.appendBits(0b0, 0);                                        // bit padding : 0 (zero) bit
        expected.appendBits(0b11101100000100011110110000010001, 32);        // byte padding
        expected.appendBits(0b11101100, 8);                                 // byte padding

        TEST_ASSERT_EQUAL(72U, qrCode::buffer.levelInBits());
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expected.data, qrCode::buffer.data, qrCode::buffer.levelInBytes());
    }
    // 1 bits padded
    {
        qrCode::setVersion(1);
        qrCode::setErrorCorrectionLevel(errorCorrectionLevel::high);
        qrCode::encodeData("A");

        bitVector<200U> expected;
        expected.reset();
        expected.appendBits(0b0010, 4);                                     // set mode to alfanumeric - always 4 bits
        expected.appendBits(0b000000001, 9);                                // count - 9 bits
        expected.appendBits(0b001010, 6);                                   // data
        expected.appendBits(0b0000, 4);                                     // terminator
        expected.appendBits(0b0, 1);                                        // bit padding : 1 bit
        expected.appendBits(0b11101100000100011110110000010001, 32);        // byte padding
        expected.appendBits(0b1110110000010001, 16);                        // byte padding

        TEST_ASSERT_EQUAL(72U, qrCode::buffer.levelInBits());
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expected.data, qrCode::buffer.data, qrCode::buffer.levelInBytes());
    }
    // 6 bits padded
    {
        qrCode::setVersion(1);
        qrCode::setErrorCorrectionLevel(errorCorrectionLevel::high);
        qrCode::encodeData("0123456");

        bitVector<200U> expected;
        expected.reset();
        expected.appendBits(0b0001, 4);                             // set mode to numeric - always 4 bits
        expected.appendBits(0b0000000111, 10);                      // count - 10 bits
        expected.appendBits(0b000000110001010110010110, 24);        // data
        expected.appendBits(0b0000, 4);                             // terminator
        expected.appendBits(0b000000, 6);                           // bit padding : 6 bits
        expected.appendBits(0b111011000001000111101100, 24);        // byte padding

        TEST_ASSERT_EQUAL(72U, qrCode::buffer.levelInBits());
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expected.data, qrCode::buffer.data, qrCode::buffer.levelInBytes());
    }
    // 7 bits padded
    {
        qrCode::setVersion(1);
        qrCode::setErrorCorrectionLevel(errorCorrectionLevel::high);
        qrCode::encodeData("01");

        bitVector<200U> expected;
        expected.reset();
        expected.appendBits(0b0001, 4);                                     // set mode to numeric - always 4 bits
        expected.appendBits(0b0000000010, 10);                              // count - 10 bits
        expected.appendBits(0b0000001, 7);                                  // data
        expected.appendBits(0b0000, 4);                                     // terminator
        expected.appendBits(0b0000000, 7);                                  // bit padding : 6 bits
        expected.appendBits(0b11101100000100011110110000010001, 32);        // byte padding
        expected.appendBits(0b11101100, 8);                                 // byte padding

        TEST_ASSERT_EQUAL(72U, qrCode::buffer.levelInBits());
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expected.data, qrCode::buffer.data, qrCode::buffer.levelInBytes());
    }
    // 1 bit terminator
    {
        qrCode::setVersion(1);
        qrCode::setErrorCorrectionLevel(errorCorrectionLevel::high);
        qrCode::encodeData("01234567890123456");

        bitVector<200U> expected;
        expected.reset();
        expected.appendBits(0b0001, 4);                                     // set mode to numeric - always 4 bits
        expected.appendBits(0b0000010001, 10);                              // count - 10 bits
        expected.appendBits(0b00000011000101011001101010011011, 32);        // data
        expected.appendBits(0b1000010100111010100111000, 25);               // data
        expected.appendBits(0b0, 1);                                        // terminator, 1-bit only

        TEST_ASSERT_EQUAL(72U, qrCode::buffer.levelInBits());
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expected.data, qrCode::buffer.data, qrCode::buffer.levelInBytes());
    }
}
#pragma endregion
#pragma region testing error correction

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

void test_blockCalculations() {
    TEST_ASSERT_EQUAL(19, qrCode::blockLengthGroup1(1, errorCorrectionLevel::low));
    TEST_ASSERT_EQUAL(1, qrCode::nmbrBlocksGroup1(1, errorCorrectionLevel::low));
    TEST_ASSERT_EQUAL(20, qrCode::blockLengthGroup2(1, errorCorrectionLevel::low));
    TEST_ASSERT_EQUAL(0, qrCode::nmbrBlocksGroup2(1, errorCorrectionLevel::low));

    TEST_ASSERT_EQUAL(13, qrCode::blockLengthGroup1(3, errorCorrectionLevel::high));
    TEST_ASSERT_EQUAL(2, qrCode::nmbrBlocksGroup1(3, errorCorrectionLevel::high));
    TEST_ASSERT_EQUAL(14, qrCode::blockLengthGroup2(3, errorCorrectionLevel::high));
    TEST_ASSERT_EQUAL(0, qrCode::nmbrBlocksGroup2(3, errorCorrectionLevel::high));

    TEST_ASSERT_EQUAL(9, qrCode::blockLengthGroup1(4, errorCorrectionLevel::high));
    TEST_ASSERT_EQUAL(4, qrCode::nmbrBlocksGroup1(4, errorCorrectionLevel::high));
    TEST_ASSERT_EQUAL(10, qrCode::blockLengthGroup2(4, errorCorrectionLevel::high));
    TEST_ASSERT_EQUAL(0, qrCode::nmbrBlocksGroup2(4, errorCorrectionLevel::high));

    TEST_ASSERT_EQUAL(15, qrCode::blockLengthGroup1(5, errorCorrectionLevel::quartile));
    TEST_ASSERT_EQUAL(2, qrCode::nmbrBlocksGroup1(5, errorCorrectionLevel::quartile));
    TEST_ASSERT_EQUAL(16, qrCode::blockLengthGroup2(5, errorCorrectionLevel::quartile));
    TEST_ASSERT_EQUAL(2, qrCode::nmbrBlocksGroup2(5, errorCorrectionLevel::quartile));

    TEST_ASSERT_EQUAL(11, qrCode::blockLengthGroup1(5, errorCorrectionLevel::high));
    TEST_ASSERT_EQUAL(2, qrCode::nmbrBlocksGroup1(5, errorCorrectionLevel::high));
    TEST_ASSERT_EQUAL(12, qrCode::blockLengthGroup2(5, errorCorrectionLevel::high));
    TEST_ASSERT_EQUAL(2, qrCode::nmbrBlocksGroup2(5, errorCorrectionLevel::high));
}

void test_blockContents() {
    {
        // Test-vector from https://dev.to/maxart2501/let-s-develop-a-qr-code-generator-part-viii-different-sizes-1e0e#codeword-blocks
        qrCode::setVersion(5);
        qrCode::setErrorCorrectionLevel(errorCorrectionLevel::quartile);
        qrCode::encodeData("https://en.wikipedia.org/wiki/QR_code#Error_correction");
        TEST_ASSERT_EQUAL(62U, qrCode::buffer.levelInBytes());

        uint8_t expectedDataBlock0[15] = {67, 102, 135, 71, 71, 7, 51, 162, 242, 246, 86, 226, 231, 118, 150};
        uint8_t expectedDataBlock1[15] = {182, 151, 6, 86, 70, 150, 18, 230, 247, 38, 114, 247, 118, 150, 182};
        uint8_t expectedDataBlock2[16] = {146, 245, 21, 37, 246, 54, 246, 70, 82, 52, 87, 39, 38, 247, 37, 246};
        uint8_t expectedDataBlock3[16] = {54, 247, 39, 38, 86, 55, 70, 150, 246, 224, 236, 17, 236, 17, 236, 17};

        TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedDataBlock0, qrCode::buffer.data + qrCode::blockOffset(0, 5, errorCorrectionLevel::quartile), qrCode::blockLength(0, 5, errorCorrectionLevel::quartile));
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedDataBlock1, qrCode::buffer.data + qrCode::blockOffset(1, 5, errorCorrectionLevel::quartile), qrCode::blockLength(1, 5, errorCorrectionLevel::quartile));
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedDataBlock2, qrCode::buffer.data + qrCode::blockOffset(2, 5, errorCorrectionLevel::quartile), qrCode::blockLength(2, 5, errorCorrectionLevel::quartile));
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedDataBlock3, qrCode::buffer.data + qrCode::blockOffset(3, 5, errorCorrectionLevel::quartile), qrCode::blockLength(3, 5, errorCorrectionLevel::quartile));
    }
}

void test_getErrorCorrectionBytes() {
    {
        uint8_t source[15] = {67, 102, 135, 71, 71, 7, 51, 162, 242, 246, 86, 226, 231, 118, 150};
        uint8_t ecc[18];
        reedSolomon::getErrorCorrectionBytes(ecc, 18, source, 15);
        uint8_t expectedEcc[18] = {0x9A, 0x96, 0x89, 0xE1, 0xF5, 0xB0, 0xDE, 0x9F, 0x29, 0x87, 0x70, 0x7D, 0x4B, 0xE0, 0x88, 0x1F, 0x85, 0xF9};
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedEcc, ecc, 18);
    }
    {
        uint8_t source[15] = {182, 151, 6, 86, 70, 150, 18, 230, 247, 38, 114, 247, 118, 150, 182};
        uint8_t ecc[18];
        reedSolomon::getErrorCorrectionBytes(ecc, 18, source, 15);
        uint8_t expectedEcc[18] = {0xDB, 0x64, 0x3A, 0x56, 0x2F, 0xBF, 0x01, 0x06, 0x79, 0x34, 0xDB, 0xC0, 0xAE, 0x0D, 0x4E, 0x6E, 0x0F, 0xC9};
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedEcc, ecc, 18);
    }
    {
        uint8_t source[16] = {146, 245, 21, 37, 246, 54, 246, 70, 82, 52, 87, 39, 38, 247, 37, 246};
        uint8_t ecc[18];
        reedSolomon::getErrorCorrectionBytes(ecc, 18, source, 16);
        uint8_t expectedEcc[18] = {0xA3, 0xE1, 0xDD, 0x45, 0xC3, 0x39, 0x8B, 0x1D, 0x42, 0xE7, 0x31, 0x5E, 0xA5, 0xE8, 0x24, 0x32, 0xBF, 0xAB};
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedEcc, ecc, 18);
    }
    {
        uint8_t source[16] = {54, 247, 39, 38, 86, 55, 70, 150, 246, 224, 236, 17, 236, 17, 236, 17};
        uint8_t ecc[18];
        reedSolomon::getErrorCorrectionBytes(ecc, 18, source, 16);
        uint8_t expectedEcc[18] = {0xF3, 0x6A, 0x27, 0x26, 0xB2, 0x7B, 0xBD, 0x43, 0xB4, 0x1A, 0xC8, 0xD4, 0xFE, 0xDB, 0xDD, 0x83, 0x9D, 0x61};
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedEcc, ecc, 18);
    }
}

void test_addErrorCorrectionBytes() {
    {
        qrCode::setVersion(1);
        qrCode::setErrorCorrectionLevel(errorCorrectionLevel::low);
        qrCode::encodeData("0123456789");
        TEST_ASSERT_EQUAL(19U, qrCode::buffer.levelInBytes());
        qrCode::addErrorCorrection();
        TEST_ASSERT_EQUAL(26U, qrCode::buffer.levelInBytes());
        uint8_t expectedData[26] = {0x10, 0x28, 0x0C, 0x56, 0x6A, 0x69, 0x00, 0xEC, 0x11, 0xEC, 0x11, 0xEC, 0x11, 0xEC, 0x11, 0xEC, 0x11, 0xEC, 0x11, 0x27, 0xAA, 0x26, 0x08, 0xEB, 0xFF, 0xD6};
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedData, qrCode::buffer.data, qrCode::buffer.levelInBytes());
    }
    {
        qrCode::setVersion(1);
        qrCode::setErrorCorrectionLevel(errorCorrectionLevel::medium);
        qrCode::encodeData("0123456789");
        TEST_ASSERT_EQUAL(16U, qrCode::buffer.levelInBytes());
        qrCode::addErrorCorrection();
        TEST_ASSERT_EQUAL(26U, qrCode::buffer.levelInBytes());
        uint8_t expectedData[26] = {0x10, 0x28, 0x0C, 0x56, 0x6A, 0x69, 0x00, 0xEC, 0x11, 0xEC, 0x11, 0xEC, 0x11, 0xEC, 0x11, 0xEC, 0x2D, 0xB7, 0x27, 0xFB, 0xF5, 0xD3, 0xAA, 0x8F, 0xD9, 0x11};
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedData, qrCode::buffer.data, qrCode::buffer.levelInBytes());
    }
    {
        qrCode::setVersion(1);
        qrCode::setErrorCorrectionLevel(errorCorrectionLevel::high);
        qrCode::encodeData("0123456789");
        TEST_ASSERT_EQUAL(9U, qrCode::buffer.levelInBytes());
        qrCode::addErrorCorrection();
        TEST_ASSERT_EQUAL(26U, qrCode::buffer.levelInBytes());
        uint8_t expectedData[26] = {0x10, 0x28, 0x0C, 0x56, 0x6A, 0x69, 0x00, 0xEC, 0x11, 0xE5, 0x3D, 0x0A, 0xBB, 0xA0, 0x26, 0x0A, 0x07, 0xA7, 0x8A, 0x51, 0xCD, 0xDD, 0x96, 0xB1, 0x68, 0x5D};
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedData, qrCode::buffer.data, qrCode::buffer.levelInBytes());
    }
    {
        qrCode::setVersion(5);
        qrCode::setErrorCorrectionLevel(errorCorrectionLevel::quartile);
        qrCode::encodeData("https://en.wikipedia.org/wiki/QR_code#Error_correction");
        TEST_ASSERT_EQUAL(62U, qrCode::buffer.levelInBytes());
        qrCode::addErrorCorrection();
        TEST_ASSERT_EQUAL(134U, qrCode::buffer.levelInBytes());
        uint8_t expectedData[134] = {
            67, 102, 135, 71, 71, 7, 51, 162, 242, 246, 86, 226, 231, 118, 150,
            182, 151, 6, 86, 70, 150, 18, 230, 247, 38, 114, 247, 118, 150, 182,
            146, 245, 21, 37, 246, 54, 246, 70, 82, 52, 87, 39, 38, 247, 37, 246,
            54, 247, 39, 38, 86, 55, 70, 150, 246, 224, 236, 17, 236, 17, 236, 17,
            0x9A, 0x96, 0x89, 0xE1, 0xF5, 0xB0, 0xDE, 0x9F, 0x29, 0x87, 0x70, 0x7D, 0x4B, 0xE0, 0x88, 0x1F, 0x85, 0xF9,
            0xDB, 0x64, 0x3A, 0x56, 0x2F, 0xBF, 0x01, 0x06, 0x79, 0x34, 0xDB, 0xC0, 0xAE, 0x0D, 0x4E, 0x6E, 0x0F, 0xC9,
            0xA3, 0xE1, 0xDD, 0x45, 0xC3, 0x39, 0x8B, 0x1D, 0x42, 0xE7, 0x31, 0x5E, 0xA5, 0xE8, 0x24, 0x32, 0xBF, 0xAB,
            0xF3, 0x6A, 0x27, 0x26, 0xB2, 0x7B, 0xBD, 0x43, 0xB4, 0x1A, 0xC8, 0xD4, 0xFE, 0xDB, 0xDD, 0x83, 0x9D, 0x61};
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedData, qrCode::buffer.data, qrCode::buffer.levelInBytes());
    }
}

void test_getDataOffset() {
    qrCode::setVersion(5);
    qrCode::setErrorCorrectionLevel(errorCorrectionLevel::quartile);
    // This qrCode has 62 data bytes organized in 4 blocks of 15 / 15 / 16 / 16 bytes, each block has 18 ecc bytes

    TEST_ASSERT_EQUAL(0, qrCode::dataOffset(0, 0));
    TEST_ASSERT_EQUAL(15, qrCode::dataOffset(1, 0));
    TEST_ASSERT_EQUAL(30, qrCode::dataOffset(2, 0));
    TEST_ASSERT_EQUAL(46, qrCode::dataOffset(3, 0));

    TEST_ASSERT_EQUAL(14, qrCode::dataOffset(0, 14));
    TEST_ASSERT_EQUAL(29, qrCode::dataOffset(1, 14));
    TEST_ASSERT_EQUAL(45, qrCode::dataOffset(2, 15));
    TEST_ASSERT_EQUAL(61, qrCode::dataOffset(3, 15));
}

void test_getEccOffset() {
    qrCode::setVersion(5);
    qrCode::setErrorCorrectionLevel(errorCorrectionLevel::quartile);
    // This qrCode has 62 data bytes organized in 4 blocks of 15 / 15 / 16 / 16 bytes, each block has 18 ecc bytes

    TEST_ASSERT_EQUAL(62, qrCode::eccOffset(0, 0));
    TEST_ASSERT_EQUAL(80, qrCode::eccOffset(1, 0));
    TEST_ASSERT_EQUAL(98, qrCode::eccOffset(2, 0));
    TEST_ASSERT_EQUAL(116, qrCode::eccOffset(3, 0));

    TEST_ASSERT_EQUAL(79, qrCode::eccOffset(0, 17));
    TEST_ASSERT_EQUAL(97, qrCode::eccOffset(1, 17));
    TEST_ASSERT_EQUAL(115, qrCode::eccOffset(2, 17));
    TEST_ASSERT_EQUAL(133, qrCode::eccOffset(3, 17));
}

void test_interleave() {
    {
        qrCode::setVersion(5);
        qrCode::setErrorCorrectionLevel(errorCorrectionLevel::quartile);
        qrCode::encodeData("https://en.wikipedia.org/wiki/QR_code#Error_correction");
        qrCode::addErrorCorrection();
        qrCode::interleaveData();
        // 4 blocks of 15 / 15 / 16 / 16 bytes, each block has 18 ecc bytes
        uint8_t expectedData[134U] = {0x43, 0xB6, 0x92, 0x36, 0x66, 0x97, 0xF5, 0xF7, 0x87, 0x06, 0x15, 0x27, 0x47, 0x56, 0x25, 0x26, 0x47, 0x46, 0xF6, 0x56, 0x07, 0x96, 0x36, 0x37, 0x33, 0x12, 0xF6, 0x46, 0xA2, 0xE6, 0x46, 0x96, 0xF2, 0xF7, 0x52, 0xF6, 0xF6, 0x26, 0x34, 0xE0, 0x56, 0x72, 0x57, 0xEC, 0xE2, 0xF7, 0x27, 0x11, 0xE7, 0x76, 0x26, 0xEC, 0x76, 0x96, 0xF7, 0x11, 0x96, 0xB6, 0x25, 0xEC, 0xF6, 0x11, 0x9A, 0xDB, 0xA3, 0xF3, 0x96, 0x64, 0xE1, 0x6A, 0x89, 0x3A, 0xDD, 0x27, 0xE1, 0x56, 0x45, 0x26, 0xF5, 0x2F, 0xC3, 0xB2, 0xB0, 0xBF, 0x39, 0x7B, 0xDE, 0x01, 0x8B, 0xBD, 0x9F, 0x06, 0x1D, 0x43, 0x29, 0x79, 0x42, 0xB4, 0x87, 0x34, 0xE7, 0x1A, 0x70, 0xDB, 0x31, 0xC8, 0x7D, 0xC0, 0x5E, 0xD4, 0x4B, 0xAE, 0xA5, 0xFE, 0xE0, 0x0D, 0xE8, 0xDB, 0x88, 0x4E, 0x24, 0xDD, 0x1F, 0x6E, 0x32, 0x83, 0x85, 0x0F, 0xBF, 0x9D, 0xF9, 0xC9, 0xAB, 0x61};
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedData, qrCode::buffer.data, 134U);
    }
    {
        qrCode::setVersion(4);
        qrCode::setErrorCorrectionLevel(errorCorrectionLevel::medium);
        qrCode::encodeData("https://en.wikipedia.org/wiki/QR_code#Error_correction");
        qrCode::addErrorCorrection();
        qrCode::interleaveData();
        // 2 blocks of 31 / 31 bytes, each block has 18 ecc bytes
        uint8_t expectedData[100U] = {0x43, 0x15, 0x66, 0x25, 0x87, 0xF6, 0x47, 0x36, 0x47, 0xF6, 0x07, 0x46, 0x33, 0x52, 0xA2, 0x34, 0xF2, 0x57, 0xF6, 0x27, 0x56, 0x26, 0xE2, 0xF7, 0xE7, 0x25, 0x76, 0xF6, 0x96, 0x36, 0xB6, 0xF7, 0x97, 0x27, 0x06, 0x26, 0x56, 0x56, 0x46, 0x37, 0x96, 0x46, 0x12, 0x96, 0xE6, 0xF6, 0xF7, 0xE0, 0x26, 0xEC, 0x72, 0x11, 0xF7, 0xEC, 0x76, 0x11, 0x96, 0xEC, 0xB6, 0x11, 0x92, 0xEC, 0xF5, 0x11, 0x28, 0x42, 0x41, 0xCB, 0x90, 0x4A, 0xF3, 0xD7, 0xA2, 0x85, 0xCA, 0x2F, 0x89, 0x68, 0xDD, 0xC4, 0x22, 0x4B, 0x72, 0x66, 0x54, 0x2B, 0x55, 0xAC, 0x30, 0x98, 0x07, 0x33, 0xE7, 0xAC, 0x67, 0x32, 0x71, 0x88, 0x12, 0x36};
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedData, qrCode::buffer.data, 100U);
    }
}

#pragma endregion
#pragma region testing drawing patterns and user data

void test_drawAllFindersAndSeparators() {
    static constexpr uint32_t testVersion{2};
    qrCode::setVersion(testVersion);
    qrCode::drawAllFinderPatternsAndSeparators(testVersion);
    static constexpr uint32_t testNmbrOfModules{(testVersion * 4 + 17) * (testVersion * 4 + 17)};
    bitVector<testNmbrOfModules> expectedModules;
    bitVector<testNmbrOfModules> expectedIsData;
    TEST_ASSERT_EQUAL(625, testNmbrOfModules);
    TEST_ASSERT_EQUAL(testNmbrOfModules, qrCode::modules.getSizeInBits());
    TEST_ASSERT_EQUAL(testNmbrOfModules, expectedModules.length);
    TEST_ASSERT_EQUAL(testNmbrOfModules, expectedIsData.length);

    expectedModules.appendBits(0b1111111000000000001111111, 25);
    expectedModules.appendBits(0b1000001000000000001000001, 25);
    expectedModules.appendBits(0b1011101000000000001011101, 25);
    expectedModules.appendBits(0b1011101000000000001011101, 25);
    expectedModules.appendBits(0b1011101000000000001011101, 25);
    expectedModules.appendBits(0b1000001000000000001000001, 25);
    expectedModules.appendBits(0b1111111000000000001111111, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b1111111000000000000000000, 25);
    expectedModules.appendBits(0b1000001000000000000000000, 25);
    expectedModules.appendBits(0b1011101000000000000000000, 25);
    expectedModules.appendBits(0b1011101000000000000000000, 25);
    expectedModules.appendBits(0b1011101000000000000000000, 25);
    expectedModules.appendBits(0b1000001000000000000000000, 25);
    expectedModules.appendBits(0b1111111000000000000000000, 25);

    TEST_ASSERT_EQUAL(testNmbrOfModules, expectedModules.levelInBits());

    expectedIsData.appendBits(0b0000000011111111100000000, 25);
    expectedIsData.appendBits(0b0000000011111111100000000, 25);
    expectedIsData.appendBits(0b0000000011111111100000000, 25);
    expectedIsData.appendBits(0b0000000011111111100000000, 25);
    expectedIsData.appendBits(0b0000000011111111100000000, 25);
    expectedIsData.appendBits(0b0000000011111111100000000, 25);
    expectedIsData.appendBits(0b0000000011111111100000000, 25);
    expectedIsData.appendBits(0b0000000011111111100000000, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b0000000011111111111111111, 25);
    expectedIsData.appendBits(0b0000000011111111111111111, 25);
    expectedIsData.appendBits(0b0000000011111111111111111, 25);
    expectedIsData.appendBits(0b0000000011111111111111111, 25);
    expectedIsData.appendBits(0b0000000011111111111111111, 25);
    expectedIsData.appendBits(0b0000000011111111111111111, 25);
    expectedIsData.appendBits(0b0000000011111111111111111, 25);
    expectedIsData.appendBits(0b0000000011111111111111111, 25);

    TEST_ASSERT_EQUAL(testNmbrOfModules, expectedIsData.levelInBits());

    for (uint32_t y = 0; y < 25; y++) {
        for (uint32_t x = 0; x < 25; x++) {
            char message[32];
            snprintf(message, 30, "mismatch %d,%d", x, y);
            TEST_ASSERT_EQUAL_MESSAGE(expectedModules.getBit(y * 25 + x), qrCode::modules.getBit(x, y), message);
            TEST_ASSERT_EQUAL_MESSAGE(expectedIsData.getBit(y * 25 + x), qrCode::isData.getBit(x, y), message);
        }
    }
}

void test_drawDarkModule() {
    static constexpr uint32_t testVersion{2};
    qrCode::setVersion(testVersion);
    qrCode::drawDarkModule(testVersion);
    static constexpr uint32_t testNmbrOfModules{(testVersion * 4 + 17) * (testVersion * 4 + 17)};
    bitVector<testNmbrOfModules> expectedModules;
    bitVector<testNmbrOfModules> expectedIsData;
    TEST_ASSERT_EQUAL(625, testNmbrOfModules);
    TEST_ASSERT_EQUAL(testNmbrOfModules, qrCode::modules.getSizeInBits());
    TEST_ASSERT_EQUAL(testNmbrOfModules, expectedModules.length);
    TEST_ASSERT_EQUAL(testNmbrOfModules, expectedIsData.length);

    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(000000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000010000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);

    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111101111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);

    TEST_ASSERT_EQUAL(testNmbrOfModules, expectedIsData.levelInBits());

    for (uint32_t y = 0; y < 25; y++) {
        for (uint32_t x = 0; x < 25; x++) {
            char message[32];
            snprintf(message, 30, "mismatch %d,%d", x, y);
            TEST_ASSERT_EQUAL_MESSAGE(expectedModules.getBit(y * 25 + x), qrCode::modules.getBit(x, y), message);
            TEST_ASSERT_EQUAL_MESSAGE(expectedIsData.getBit(y * 25 + x), qrCode::isData.getBit(x, y), message);
        }
    }
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

void test_drawAllAlignmentPatterns() {
    static constexpr uint32_t testVersion{2};
    qrCode::setVersion(testVersion);
    qrCode::drawAllAlignmentPatterns(testVersion);
    static constexpr uint32_t testNmbrOfModules{(testVersion * 4 + 17) * (testVersion * 4 + 17)};
    bitVector<testNmbrOfModules> expectedModules;
    bitVector<testNmbrOfModules> expectedIsData;
    TEST_ASSERT_EQUAL(625, testNmbrOfModules);
    TEST_ASSERT_EQUAL(testNmbrOfModules, qrCode::modules.getSizeInBits());
    TEST_ASSERT_EQUAL(testNmbrOfModules, expectedModules.length);
    TEST_ASSERT_EQUAL(testNmbrOfModules, expectedIsData.length);

    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(000000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000111110000, 25);
    expectedModules.appendBits(0b0000000000000000100010000, 25);
    expectedModules.appendBits(0b0000000000000000101010000, 25);
    expectedModules.appendBits(0b0000000000000000100010000, 25);
    expectedModules.appendBits(0b0000000000000000111110000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);

    TEST_ASSERT_EQUAL(testNmbrOfModules, expectedModules.levelInBits());

    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111000001111, 25);
    expectedIsData.appendBits(0b1111111111111111000001111, 25);
    expectedIsData.appendBits(0b1111111111111111000001111, 25);
    expectedIsData.appendBits(0b1111111111111111000001111, 25);
    expectedIsData.appendBits(0b1111111111111111000001111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);

    TEST_ASSERT_EQUAL(testNmbrOfModules, expectedIsData.levelInBits());

    for (uint32_t y = 0; y < 25; y++) {
        for (uint32_t x = 0; x < 25; x++) {
            char message[32];
            snprintf(message, 30, "mismatch %d,%d", x, y);
            TEST_ASSERT_EQUAL_MESSAGE(expectedModules.getBit(y * 25 + x), qrCode::modules.getBit(x, y), message);
            TEST_ASSERT_EQUAL_MESSAGE(expectedIsData.getBit(y * 25 + x), qrCode::isData.getBit(x, y), message);
        }
    }
}

void test_drawTimingPatterns() {
    static constexpr uint32_t testVersion{2};
    qrCode::setVersion(testVersion);
    qrCode::drawTimingPattern(testVersion);
    static constexpr uint32_t testNmbrOfModules{(testVersion * 4 + 17) * (testVersion * 4 + 17)};
    bitVector<testNmbrOfModules> expectedModules;
    bitVector<testNmbrOfModules> expectedIsData;
    TEST_ASSERT_EQUAL(625, testNmbrOfModules);
    TEST_ASSERT_EQUAL(testNmbrOfModules, qrCode::modules.getSizeInBits());
    TEST_ASSERT_EQUAL(testNmbrOfModules, expectedModules.length);
    TEST_ASSERT_EQUAL(testNmbrOfModules, expectedIsData.length);

    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(000000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000010101010100000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000001000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000001000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000001000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000001000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000001000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);
    expectedModules.appendBits(0b0000000000000000000000000, 25);

    TEST_ASSERT_EQUAL(testNmbrOfModules, expectedModules.levelInBits());

    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111100000000011111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111110111111111111111111, 25);
    expectedIsData.appendBits(0b1111110111111111111111111, 25);
    expectedIsData.appendBits(0b1111110111111111111111111, 25);
    expectedIsData.appendBits(0b1111110111111111111111111, 25);
    expectedIsData.appendBits(0b1111110111111111111111111, 25);
    expectedIsData.appendBits(0b1111110111111111111111111, 25);
    expectedIsData.appendBits(0b1111110111111111111111111, 25);
    expectedIsData.appendBits(0b1111110111111111111111111, 25);
    expectedIsData.appendBits(0b1111110111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);

    TEST_ASSERT_EQUAL(testNmbrOfModules, expectedIsData.levelInBits());

    for (uint32_t y = 0; y < 25; y++) {
        for (uint32_t x = 0; x < 25; x++) {
            char message[32];
            snprintf(message, 30, "mismatch %d,%d", x, y);
            TEST_ASSERT_EQUAL_MESSAGE(expectedModules.getBit(y * 25 + x), qrCode::modules.getBit(x, y), message);
            TEST_ASSERT_EQUAL_MESSAGE(expectedIsData.getBit(y * 25 + x), qrCode::isData.getBit(x, y), message);
        }
    }
}

void test_drawDummyFormatInfo() {
    static constexpr uint32_t testVersion{2};
    qrCode::setVersion(testVersion);
    qrCode::drawDummyFormatBits(testVersion);
    static constexpr uint32_t testNmbrOfModules{(testVersion * 4 + 17) * (testVersion * 4 + 17)};
    bitVector<testNmbrOfModules> expectedIsData;
    TEST_ASSERT_EQUAL(625, testNmbrOfModules);
    TEST_ASSERT_EQUAL(testNmbrOfModules, qrCode::modules.getSizeInBits());
    TEST_ASSERT_EQUAL(testNmbrOfModules, expectedIsData.length);

    expectedIsData.appendBits(0b1111111101111111111111111, 25);
    expectedIsData.appendBits(0b1111111101111111111111111, 25);
    expectedIsData.appendBits(0b1111111101111111111111111, 25);
    expectedIsData.appendBits(0b1111111101111111111111111, 25);
    expectedIsData.appendBits(0b1111111101111111111111111, 25);
    expectedIsData.appendBits(0b1111111101111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111101111111111111111, 25);
    expectedIsData.appendBits(0b0000001001111111100000000, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111111111111111111111, 25);
    expectedIsData.appendBits(0b1111111101111111111111111, 25);
    expectedIsData.appendBits(0b1111111101111111111111111, 25);
    expectedIsData.appendBits(0b1111111101111111111111111, 25);
    expectedIsData.appendBits(0b1111111101111111111111111, 25);
    expectedIsData.appendBits(0b1111111101111111111111111, 25);
    expectedIsData.appendBits(0b1111111101111111111111111, 25);
    expectedIsData.appendBits(0b1111111101111111111111111, 25);

    TEST_ASSERT_EQUAL(testNmbrOfModules, expectedIsData.levelInBits());

    for (uint32_t y = 0; y < 25; y++) {
        for (uint32_t x = 0; x < 25; x++) {
            char message[32];
            snprintf(message, 30, "mismatch %d,%d", x, y);
            TEST_ASSERT_EQUAL_MESSAGE(false, qrCode::modules.getBit(x, y), message);
            TEST_ASSERT_EQUAL_MESSAGE(expectedIsData.getBit(y * 25 + x), qrCode::isData.getBit(x, y), message);
        }
    }
}

void test_drawFormatInfo() { TEST_IGNORE_MESSAGE("TODO: implement me"); }
void test_drawVersionInfo() { TEST_IGNORE_MESSAGE("TODO: implement me"); }
void test_drawAllPatterns() { TEST_IGNORE_MESSAGE("TODO: implement me"); }

void test_drawPayload() {
    static constexpr uint32_t testVersion{1};
    qrCode::setVersion(testVersion);
    qrCode::setErrorCorrectionLevel(errorCorrectionLevel::low);

    bitVector<208> expectedBuffer;
    expectedBuffer.appendBits(0b00010000000001000000000011101100, 32);        // https://www.nayuki.io/page/creating-a-qr-code-step-by-step with payload "0"
    expectedBuffer.appendBits(0b00010001111011000001000111101100, 32);
    expectedBuffer.appendBits(0b00010001111011000001000111101100, 32);
    expectedBuffer.appendBits(0b00010001111011000001000111101100, 32);
    expectedBuffer.appendBits(0b00010001111011000001000110000011, 32);
    expectedBuffer.appendBits(0b00000111010001011100001111000011, 32);
    expectedBuffer.appendBits(0b1100110000011001, 16);

    qrCode::encodeData("0");
    qrCode::addErrorCorrection();

    for (uint32_t bitIndex = 0; bitIndex < 208; bitIndex++) {
        char message[32];
        snprintf(message, 30, "mismatch %d", bitIndex);
        TEST_ASSERT_EQUAL_MESSAGE(expectedBuffer.getBit(bitIndex), qrCode::buffer.getBit(bitIndex), message);
    }

    qrCode::drawPatterns(testVersion);
    qrCode::drawPayload(testVersion);

    static constexpr uint32_t testNmbrOfModules{(testVersion * 4 + 17) * (testVersion * 4 + 17)};
    bitVector<testNmbrOfModules> expectedModules;
    expectedModules.appendBits(0b111111100111001111111, 21);        // https://www.nayuki.io/page/creating-a-qr-code-step-by-step with payload "0"
    expectedModules.appendBits(0b100000100010001000001, 21);
    expectedModules.appendBits(0b101110100111001011101, 21);
    expectedModules.appendBits(0b101110100000001011101, 21);
    expectedModules.appendBits(0b101110100000001011101, 21);
    expectedModules.appendBits(0b100000100101101000001, 21);
    expectedModules.appendBits(0b111111101010101111111, 21);
    expectedModules.appendBits(0b000000000000100000000, 21);
    expectedModules.appendBits(0b000000100101100000000, 21);
    expectedModules.appendBits(0b000011011011011101100, 21);

    expectedModules.appendBits(0b101100100000001000100, 21);
    expectedModules.appendBits(0b010000000001011101100, 21);
    expectedModules.appendBits(0b101111111110000000000, 21);
    expectedModules.appendBits(0b000000001000000000000, 21);

    expectedModules.appendBits(0b111111100001110111010, 21);
    expectedModules.appendBits(0b100000100100100010000, 21);
    expectedModules.appendBits(0b101110100111110111000, 21);
    expectedModules.appendBits(0b101110100101011101100, 21);
    expectedModules.appendBits(0b101110100000001000100, 21);
    expectedModules.appendBits(0b100000100101011101110, 21);
    expectedModules.appendBits(0b111111100100000000000, 21);

    uint32_t bitIndex{0};
    for (int32_t y = 20; y > 0; y--) {
        for (int32_t x = 20; x > 0; x--) {
            char message[64];
            snprintf(message, 63, "mismatch bit %d, (%d,%d)", bitIndex, x, y);
            TEST_ASSERT_EQUAL_MESSAGE(expectedModules.getBit(y * 21 + x), qrCode::modules.getBit(x, y), message);
            bitIndex++;
        }
    }

    // TODO : also do this for a V2 which has alignment patterns and V7 which has version info
}

void test_masking() {
    {
        static constexpr uint32_t testVersion{1};
        static constexpr uint32_t maskType{0};
        qrCode::setVersion(testVersion);
        qrCode::applyMask(maskType);
        static constexpr uint32_t testNmbrOfModules{(testVersion * 4 + 17) * (testVersion * 4 + 17)};
        bitVector<testNmbrOfModules> expectedModules;
        TEST_ASSERT_EQUAL(441, testNmbrOfModules);
        TEST_ASSERT_EQUAL(testNmbrOfModules, qrCode::modules.getSizeInBits());
        TEST_ASSERT_EQUAL(testNmbrOfModules, expectedModules.length);

        expectedModules.appendBits(0b101010101010101010101, 21);
        expectedModules.appendBits(0b010101010101010101010, 21);
        expectedModules.appendBits(0b101010101010101010101, 21);
        expectedModules.appendBits(0b010101010101010101010, 21);
        expectedModules.appendBits(0b101010101010101010101, 21);
        expectedModules.appendBits(0b010101010101010101010, 21);
        expectedModules.appendBits(0b101010101010101010101, 21);
        expectedModules.appendBits(0b010101010101010101010, 21);
        expectedModules.appendBits(0b101010101010101010101, 21);
        expectedModules.appendBits(0b010101010101010101010, 21);
        expectedModules.appendBits(0b101010101010101010101, 21);
        expectedModules.appendBits(0b010101010101010101010, 21);
        expectedModules.appendBits(0b101010101010101010101, 21);
        expectedModules.appendBits(0b010101010101010101010, 21);
        expectedModules.appendBits(0b101010101010101010101, 21);
        expectedModules.appendBits(0b010101010101010101010, 21);
        expectedModules.appendBits(0b101010101010101010101, 21);
        expectedModules.appendBits(0b010101010101010101010, 21);
        expectedModules.appendBits(0b101010101010101010101, 21);
        expectedModules.appendBits(0b010101010101010101010, 21);
        expectedModules.appendBits(0b101010101010101010101, 21);

        for (uint32_t y = 0; y < 21; y++) {
            for (uint32_t x = 0; x < 21; x++) {
                char message[32];
                snprintf(message, 30, "mismatch %d,%d", x, y);
                TEST_ASSERT_EQUAL_MESSAGE(expectedModules.getBit(y * 21 + x), qrCode::modules.getBit(x, y), message);
            }
        }
    }
    {
        static constexpr uint32_t testVersion{1};
        static constexpr uint32_t maskType{1};
        qrCode::setVersion(testVersion);
        qrCode::applyMask(maskType);
        static constexpr uint32_t testNmbrOfModules{(testVersion * 4 + 17) * (testVersion * 4 + 17)};
        bitVector<testNmbrOfModules> expectedModules;
        TEST_ASSERT_EQUAL(441, testNmbrOfModules);
        TEST_ASSERT_EQUAL(testNmbrOfModules, qrCode::modules.getSizeInBits());
        TEST_ASSERT_EQUAL(testNmbrOfModules, expectedModules.length);

        expectedModules.appendBits(0b111111111111111111111, 21);
        expectedModules.appendBits(0b000000000000000000000, 21);
        expectedModules.appendBits(0b111111111111111111111, 21);
        expectedModules.appendBits(0b000000000000000000000, 21);
        expectedModules.appendBits(0b111111111111111111111, 21);
        expectedModules.appendBits(0b000000000000000000000, 21);
        expectedModules.appendBits(0b111111111111111111111, 21);
        expectedModules.appendBits(0b000000000000000000000, 21);
        expectedModules.appendBits(0b111111111111111111111, 21);
        expectedModules.appendBits(0b000000000000000000000, 21);
        expectedModules.appendBits(0b111111111111111111111, 21);
        expectedModules.appendBits(0b000000000000000000000, 21);
        expectedModules.appendBits(0b111111111111111111111, 21);
        expectedModules.appendBits(0b000000000000000000000, 21);
        expectedModules.appendBits(0b111111111111111111111, 21);
        expectedModules.appendBits(0b000000000000000000000, 21);
        expectedModules.appendBits(0b111111111111111111111, 21);
        expectedModules.appendBits(0b000000000000000000000, 21);
        expectedModules.appendBits(0b111111111111111111111, 21);
        expectedModules.appendBits(0b000000000000000000000, 21);
        expectedModules.appendBits(0b111111111111111111111, 21);

        for (uint32_t y = 0; y < 21; y++) {
            for (uint32_t x = 0; x < 21; x++) {
                char message[32];
                snprintf(message, 30, "mismatch %d,%d", x, y);
                TEST_ASSERT_EQUAL_MESSAGE(expectedModules.getBit(y * 21 + x), qrCode::modules.getBit(x, y), message);
            }
        }
    }
    {
        static constexpr uint32_t testVersion{1};
        static constexpr uint32_t maskType{2};
        qrCode::setVersion(testVersion);
        qrCode::applyMask(maskType);
        static constexpr uint32_t testNmbrOfModules{(testVersion * 4 + 17) * (testVersion * 4 + 17)};
        bitVector<testNmbrOfModules> expectedModules;
        TEST_ASSERT_EQUAL(441, testNmbrOfModules);
        TEST_ASSERT_EQUAL(testNmbrOfModules, qrCode::modules.getSizeInBits());
        TEST_ASSERT_EQUAL(testNmbrOfModules, expectedModules.length);

        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);

        for (uint32_t y = 0; y < 21; y++) {
            for (uint32_t x = 0; x < 21; x++) {
                char message[32];
                snprintf(message, 30, "mismatch %d,%d", x, y);
                TEST_ASSERT_EQUAL_MESSAGE(expectedModules.getBit(y * 21 + x), qrCode::modules.getBit(x, y), message);
            }
        }
    }
    {
        static constexpr uint32_t testVersion{1};
        static constexpr uint32_t maskType{3};
        qrCode::setVersion(testVersion);
        qrCode::applyMask(maskType);
        static constexpr uint32_t testNmbrOfModules{(testVersion * 4 + 17) * (testVersion * 4 + 17)};
        bitVector<testNmbrOfModules> expectedModules;
        TEST_ASSERT_EQUAL(441, testNmbrOfModules);
        TEST_ASSERT_EQUAL(testNmbrOfModules, qrCode::modules.getSizeInBits());
        TEST_ASSERT_EQUAL(testNmbrOfModules, expectedModules.length);

        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b001001001001001001001, 21);
        expectedModules.appendBits(0b010010010010010010010, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b001001001001001001001, 21);
        expectedModules.appendBits(0b010010010010010010010, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b001001001001001001001, 21);
        expectedModules.appendBits(0b010010010010010010010, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b001001001001001001001, 21);
        expectedModules.appendBits(0b010010010010010010010, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b001001001001001001001, 21);
        expectedModules.appendBits(0b010010010010010010010, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b001001001001001001001, 21);
        expectedModules.appendBits(0b010010010010010010010, 21);
        expectedModules.appendBits(0b100100100100100100100, 21);
        expectedModules.appendBits(0b001001001001001001001, 21);
        expectedModules.appendBits(0b010010010010010010010, 21);

        for (uint32_t y = 0; y < 21; y++) {
            for (uint32_t x = 0; x < 21; x++) {
                char message[32];
                snprintf(message, 30, "mismatch %d,%d", x, y);
                TEST_ASSERT_EQUAL_MESSAGE(expectedModules.getBit(y * 21 + x), qrCode::modules.getBit(x, y), message);
            }
        }
    }
    {
        static constexpr uint32_t testVersion{1};
        static constexpr uint32_t maskType{4};
        qrCode::setVersion(testVersion);
        qrCode::applyMask(maskType);
        static constexpr uint32_t testNmbrOfModules{(testVersion * 4 + 17) * (testVersion * 4 + 17)};
        bitVector<testNmbrOfModules> expectedModules;
        TEST_ASSERT_EQUAL(441, testNmbrOfModules);
        TEST_ASSERT_EQUAL(testNmbrOfModules, qrCode::modules.getSizeInBits());
        TEST_ASSERT_EQUAL(testNmbrOfModules, expectedModules.length);

        expectedModules.appendBits(0b111000111000111000111, 21);
        expectedModules.appendBits(0b111000111000111000111, 21);
        expectedModules.appendBits(0b000111000111000111000, 21);
        expectedModules.appendBits(0b000111000111000111000, 21);
        expectedModules.appendBits(0b111000111000111000111, 21);
        expectedModules.appendBits(0b111000111000111000111, 21);
        expectedModules.appendBits(0b000111000111000111000, 21);
        expectedModules.appendBits(0b000111000111000111000, 21);
        expectedModules.appendBits(0b111000111000111000111, 21);
        expectedModules.appendBits(0b111000111000111000111, 21);
        expectedModules.appendBits(0b000111000111000111000, 21);
        expectedModules.appendBits(0b000111000111000111000, 21);
        expectedModules.appendBits(0b111000111000111000111, 21);
        expectedModules.appendBits(0b111000111000111000111, 21);
        expectedModules.appendBits(0b000111000111000111000, 21);
        expectedModules.appendBits(0b000111000111000111000, 21);
        expectedModules.appendBits(0b111000111000111000111, 21);
        expectedModules.appendBits(0b111000111000111000111, 21);
        expectedModules.appendBits(0b000111000111000111000, 21);
        expectedModules.appendBits(0b000111000111000111000, 21);
        expectedModules.appendBits(0b111000111000111000111, 21);

        for (uint32_t y = 0; y < 21; y++) {
            for (uint32_t x = 0; x < 21; x++) {
                char message[32];
                snprintf(message, 30, "mismatch %d,%d", x, y);
                TEST_ASSERT_EQUAL_MESSAGE(expectedModules.getBit(y * 21 + x), qrCode::modules.getBit(x, y), message);
            }
        }
    }
}

#pragma endregion
#pragma region testing api
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
    const char testData8[]{"012345678901234567"};                                                  // 18 numeric characters
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
    const char testData54[55]{"#12345678901234567890123456789012345678901234567890123"};
    TEST_ASSERT_EQUAL(0, qrCode::versionNeeded(testData54, errorCorrectionLevel::high));        // This case still fails, need to investigate in detail why
}

void test_errorCorrectionPossible() {
    {
        const char testData[]{"01234567890123456"};                                                              // 17 numeric characters
        TEST_ASSERT_EQUAL(errorCorrectionLevel::high, qrCode::errorCorrectionLevelPossible(testData, 1));        //
    }
    {
        const char testData[]{"012345678901234567"};                                                                 // 18 numeric characters
        TEST_ASSERT_EQUAL(errorCorrectionLevel::quartile, qrCode::errorCorrectionLevelPossible(testData, 1));        //
    }
    {
        const char testData[]{"012345678901234567890123456"};                                                        // 27 numeric characters
        TEST_ASSERT_EQUAL(errorCorrectionLevel::quartile, qrCode::errorCorrectionLevelPossible(testData, 1));        //
    }
    {
        const char testData[]{"0123456789012345678901234567"};                                                     // 28 numeric characters
        TEST_ASSERT_EQUAL(errorCorrectionLevel::medium, qrCode::errorCorrectionLevelPossible(testData, 1));        //
    }
    {
        const char testData[]{"0123456789012345678901234567890123"};                                               // 34 numeric characters
        TEST_ASSERT_EQUAL(errorCorrectionLevel::medium, qrCode::errorCorrectionLevelPossible(testData, 1));        //
    }
    {
        const char testData[]{"01234567890123456789012345678901234"};                                           // 35 numeric characters
        TEST_ASSERT_EQUAL(errorCorrectionLevel::low, qrCode::errorCorrectionLevelPossible(testData, 1));        //
    }
}

void test_generate() { TEST_IGNORE_MESSAGE("TODO: implement me"); }

#pragma endregion

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

void test_calculatePayloadLength() {
    TEST_ASSERT_EQUAL(41, qrCode::payloadLengthInBits(8, 1, encodingFormat::numeric));
    TEST_ASSERT_EQUAL(57, qrCode::payloadLengthInBits(8, 1, encodingFormat::alphanumeric));
    TEST_ASSERT_EQUAL(76, qrCode::payloadLengthInBits(8, 1, encodingFormat::byte));

    TEST_ASSERT_EQUAL(151, qrCode::payloadLengthInBits(41, 1, encodingFormat::numeric));
    TEST_ASSERT_EQUAL(151, qrCode::payloadLengthInBits(25, 1, encodingFormat::alphanumeric));
    TEST_ASSERT_EQUAL(148, qrCode::payloadLengthInBits(17, 1, encodingFormat::byte));
}

void test_formatInfo() {
    static constexpr uint16_t formatInfoBits[nmbrOfErrorCorrectionLevels][nmbrOfMasks]{
        {0b111011111000100, 0b111001011110011, 0b111110110101010, 0b111100010011101, 0b110011000101111, 0b110001100011000, 0b110110001000001, 0b110100101110110},        // formatInfo bits taken from https://www.thonky.com/qr-code-tutorial/format-version-tables
        {0b101010000010010, 0b101000100100101, 0b101111001111100, 0b101101101001011, 0b100010111111001, 0b100000011001110, 0b100111110010111, 0b100101010100000},
        {0b011010101011111, 0b011000001101000, 0b011111100110001, 0b011101000000110, 0b010010010110100, 0b010000110000011, 0b010111011011010, 0b010101111101101},
        {0b001011010001001, 0b001001110111110, 0b001110011100111, 0b001100111010000, 0b000011101100010, 0b000001001010101, 0b000110100001100, 0b000100000111011}};

    for (uint32_t eccLevelIndex = 0; eccLevelIndex < 4; eccLevelIndex++) {
        for (uint32_t maskTypeIndex = 0; maskTypeIndex < 8; maskTypeIndex++) {
            TEST_ASSERT_EQUAL(formatInfoBits[eccLevelIndex][maskTypeIndex], qrCode::calculateFormatInfo(static_cast<errorCorrectionLevel>(eccLevelIndex), maskTypeIndex));
        }
    }
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    // testing internal data initialization / setting
    RUN_TEST(test_initialization);

    // general helpers
    RUN_TEST(test_size);
    RUN_TEST(test_setVersion);
    RUN_TEST(test_setErrorCorrectionLevel);

    // encoding user data into payload
    RUN_TEST(test_isNumeric);
    RUN_TEST(test_isAlphanumeric);
    RUN_TEST(test_getEncodingFormat);
    RUN_TEST(test_compress);
    RUN_TEST(test_getModeBits);
    RUN_TEST(test_payloadLengthInBits);
    RUN_TEST(test_encodeData);
    RUN_TEST(test_characterCountIndicatorLength);
    RUN_TEST(test_calculatePayloadLength);

    // error correction
    RUN_TEST(test_blockCalculations);
    RUN_TEST(test_blockContents);
    RUN_TEST(test_getErrorCorrectionBytes);
    RUN_TEST(test_addErrorCorrectionBytes);
    RUN_TEST(test_getDataOffset);
    RUN_TEST(test_getEccOffset);
    RUN_TEST(test_interleave);

    // drawing function patterns and user data
    RUN_TEST(test_drawAllFindersAndSeparators);

    RUN_TEST(test_nmbrOfAlignmentPatterns);
    RUN_TEST(test_alignmentPatternSpacing);
    RUN_TEST(test_alignmentPatternCoordinates);
    RUN_TEST(test_drawAllAlignmentPatterns);

    RUN_TEST(test_drawTimingPatterns);
    RUN_TEST(test_drawDummyFormatInfo);
    RUN_TEST(test_drawDarkModule);
    RUN_TEST(test_drawFormatInfo);

    RUN_TEST(test_drawVersionInfo);
    RUN_TEST(test_drawAllPatterns);
    RUN_TEST(test_drawPayload);
    RUN_TEST(test_masking);

    // api
    RUN_TEST(test_versionNeeded);
    RUN_TEST(test_errorCorrectionPossible);
    RUN_TEST(test_generate);

    // temporary tests
    RUN_TEST(test_nmbrOfErrorCorrectionModules);
    RUN_TEST(test_nmbrOfRawDataModules);
    RUN_TEST(test_calculatePayloadLength);
    RUN_TEST(test_formatInfo);

    UNITY_END();
}
