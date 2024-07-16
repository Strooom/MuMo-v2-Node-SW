// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include <qrcode2.hpp>
#include <string.h>

void print_data(QRCode *qrcode);
void collect_data(QRCode *qrcode, bool *data);

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

#pragma region bitBucket

void test_bb_init() {
    uint8_t testStorage[8U];
    BitBucket testBucket;
    bb_initGrid(&testBucket, testStorage, 8U);

}

// void test_getGridSizeBytes() {
//     TEST_ASSERT_EQUAL(0, bitArray::getGridSizeInBytes(0));
//     TEST_ASSERT_EQUAL(1, bitArray::getGridSizeInBytes(1));
//     TEST_ASSERT_EQUAL(1, bitArray::getGridSizeInBytes(2));
//     TEST_ASSERT_EQUAL(2, bitArray::getGridSizeInBytes(3));
//     TEST_ASSERT_EQUAL(2, bitArray::getGridSizeInBytes(4));
//     TEST_ASSERT_EQUAL(4, bitArray::getGridSizeInBytes(5));
//     TEST_ASSERT_EQUAL(5, bitArray::getGridSizeInBytes(6));
//     TEST_ASSERT_EQUAL(7, bitArray::getGridSizeInBytes(7));
//     TEST_ASSERT_EQUAL(8, bitArray::getGridSizeInBytes(8));
// }

// void test_getBufferSizeBytes() {
//     TEST_ASSERT_EQUAL(0, bitArray::getBufferSizeInBytes(0));
//     TEST_ASSERT_EQUAL(1, bitArray::getBufferSizeInBytes(1));
//     TEST_ASSERT_EQUAL(1, bitArray::getBufferSizeInBytes(8));
//     TEST_ASSERT_EQUAL(2, bitArray::getBufferSizeInBytes(9));
// }


//     void initBuffer(uint8_t *data, int32_t capacityBytes);
//     void initGrid(uint8_t *data, uint8_t size);
//     void setOrClearBit(uint8_t x, uint8_t y, bool on);
//     void invertBit(uint8_t x, uint8_t y, bool invert);
//     void appendBits(uint32_t val, uint8_t length);
//     bool getBit(uint8_t x, uint8_t y);




#pragma endregion

#pragma region staticFunctions




void test_getAlphanumeric() {
    TEST_ASSERT_EQUAL(0, getAlphanumeric('0'));
    TEST_ASSERT_EQUAL(1, getAlphanumeric('1'));
    TEST_ASSERT_EQUAL(8, getAlphanumeric('8'));
    TEST_ASSERT_EQUAL(9, getAlphanumeric('9'));
    TEST_ASSERT_EQUAL(10, getAlphanumeric('A'));
    TEST_ASSERT_EQUAL(11, getAlphanumeric('B'));
    TEST_ASSERT_EQUAL(34, getAlphanumeric('Y'));
    TEST_ASSERT_EQUAL(35, getAlphanumeric('Z'));
    TEST_ASSERT_EQUAL(36, getAlphanumeric(' '));
    TEST_ASSERT_EQUAL(37, getAlphanumeric('$'));
    TEST_ASSERT_EQUAL(38, getAlphanumeric('%'));
    TEST_ASSERT_EQUAL(39, getAlphanumeric('*'));
    TEST_ASSERT_EQUAL(40, getAlphanumeric('+'));
    TEST_ASSERT_EQUAL(41, getAlphanumeric('-'));
    TEST_ASSERT_EQUAL(42, getAlphanumeric('.'));
    TEST_ASSERT_EQUAL(43, getAlphanumeric('/'));
    TEST_ASSERT_EQUAL(44, getAlphanumeric(':'));
    TEST_ASSERT_EQUAL(-1, getAlphanumeric('?'));
    TEST_ASSERT_EQUAL(-1, getAlphanumeric('a'));
}

void test_isAlphanumeric() {
    TEST_ASSERT_TRUE(isAlphanumeric("0", 1));
    TEST_ASSERT_TRUE(isAlphanumeric("9", 1));
    TEST_ASSERT_TRUE(isAlphanumeric("A", 1));
    TEST_ASSERT_TRUE(isAlphanumeric("Z", 1));
    TEST_ASSERT_TRUE(isAlphanumeric(" ", 1));
    TEST_ASSERT_FALSE(isAlphanumeric("?", 1));
    TEST_ASSERT_FALSE(isAlphanumeric("?", 1));
    TEST_ASSERT_FALSE(isAlphanumeric("\n", 1));

    TEST_ASSERT_TRUE(isAlphanumeric("HELLO WORLD", 11));
    TEST_ASSERT_TRUE(isAlphanumeric("1234567890", 10));
    TEST_ASSERT_FALSE(isAlphanumeric(" ? ", 3));
}

void test_isNumeric() {
    TEST_ASSERT_TRUE(isNumeric("1234567890", 10));
    TEST_ASSERT_FALSE(isNumeric("?", 1));
    TEST_ASSERT_FALSE(isNumeric("\n", 1));
    TEST_ASSERT_FALSE(isNumeric("000 000", 7));
}

void test_getModeBits() {
    TEST_ASSERT_EQUAL(10, getModeBits(1, 0));
    TEST_ASSERT_EQUAL(9, getModeBits(1, 1));
    TEST_ASSERT_EQUAL(8, getModeBits(1, 2));
    TEST_ASSERT_EQUAL(10, getModeBits(9, 0));
    TEST_ASSERT_EQUAL(9, getModeBits(9, 1));
    TEST_ASSERT_EQUAL(8, getModeBits(9, 2));

    TEST_ASSERT_EQUAL(12, getModeBits(10, 0));
    TEST_ASSERT_EQUAL(11, getModeBits(10, 1));
    TEST_ASSERT_EQUAL(16, getModeBits(10, 2));
    TEST_ASSERT_EQUAL(12, getModeBits(26, 0));
    TEST_ASSERT_EQUAL(11, getModeBits(26, 1));
    TEST_ASSERT_EQUAL(16, getModeBits(26, 2));

    TEST_ASSERT_EQUAL(14, getModeBits(27, 0));
    TEST_ASSERT_EQUAL(13, getModeBits(27, 1));
    TEST_ASSERT_EQUAL(16, getModeBits(27, 2));
    TEST_ASSERT_EQUAL(14, getModeBits(40, 0));
    TEST_ASSERT_EQUAL(13, getModeBits(40, 1));
    TEST_ASSERT_EQUAL(16, getModeBits(40, 2));
}


void test_max() {
    TEST_ASSERT_EQUAL(5, max(0, 5));
    TEST_ASSERT_EQUAL(5, max(5, 0));
    TEST_ASSERT_EQUAL(5, max(5, 5));
    TEST_ASSERT_EQUAL(5, max(5, -5));
    TEST_ASSERT_EQUAL(5, max(-5, 5));
    TEST_ASSERT_EQUAL(-5, max(-5, -5));
}

void test_bufferSize() {
    TEST_ASSERT_EQUAL(56, qrcode_getBufferSize(1));
    TEST_ASSERT_EQUAL(79, qrcode_getBufferSize(2));
    TEST_ASSERT_EQUAL(172, qrcode_getBufferSize(5));
}

void test_qrCodeSize() {
    QRCode qrcode1;
    uint8_t qrcodeData1[qrcode_getBufferSize(1)];
    qrcode_initText(&qrcode1, qrcodeData1, 1, 1, "Hello World");
    TEST_ASSERT_EQUAL(21, qrcode1.size);        // The size of the QR code is (4 * version + 17)

    QRCode qrcode2;
    uint8_t qrcodeData2[qrcode_getBufferSize(2)];
    qrcode_initText(&qrcode2, qrcodeData2, 2, 1, "Hello World");
    TEST_ASSERT_EQUAL(25, qrcode2.size);

    QRCode qrcode3;
    uint8_t qrcodeData3[qrcode_getBufferSize(5)];
    qrcode_initText(&qrcode3, qrcodeData3, 5, 1, "Hello World");
    TEST_ASSERT_EQUAL(37, qrcode3.size);

    QRCode qrcode8;
    uint8_t qrcodeData8[qrcode_getBufferSize(8)];
    qrcode_initText(&qrcode8, qrcodeData8, 8, 1, "Hello World");
    TEST_ASSERT_EQUAL(49, qrcode8.size);
}

#pragma endregion


void test_endresult() {
    // Validating the output of the QR code generation
    // Need to take into account :
    // * use different sizes : size 1 to size 40
    // * different ECC levels : 4 levels
    // * different mode : 3 modes : numeric, alphanumeric, byte

    // size 1, ECC low, numeric

    // size 2, ECC low, numeric
    // size 2, ECC medium, numeric
    // size 2, ECC quartile, numeric
    // size 2, ECC high, numeric

    // size 2. ECC medium, alphanumeric
    // size 2, ECC medium, byte

    // Note : for size > 7, there is some extra code involved, so some tests should use that size..

    QRCode qrcode1;
    uint8_t qrcode1Data[qrcode_getBufferSize(1)];
    qrcode_initText(&qrcode1, qrcode1Data, 1, 0, "0123456789");
    bool expected1[21 * 21]{true, true, true, true, true, true, true, false, true, false, true, true, true, false, true, true, true, true, true, true, true, true, false, false, false, false, false, true, false, false, false, true, true, false, false, true, false, false, false, false, false, true, true, false, true, true, true, false, true, false, true, true, false, true, false, false, true, false, true, true, true, false, true, true, false, true, true, true, false, true, false, true, true, false, false, true, false, true, false, true, true, true, false, true, true, false, true, true, true, false, true, false, true, false, false, true, false, false, true, false, true, true, true, false, true, true, false, false, false, false, false, true, false, false, true, true, true, true, false, true, false, false, false, false, false, true, true, true, true, true, true, true, true, false, true, false, true, false, true, false, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, true, true, false, false, false, false, false, false, false, false, true, true, true, true, false, false, true, false, true, true, true, true, true, true, false, false, true, true, true, false, true, false, true, true, false, true, true, false, false, false, true, false, true, true, true, true, false, false, true, true, false, false, true, false, false, true, false, false, true, false, false, false, true, true, false, false, false, false, false, false, false, true, false, true, true, true, true, true, true, false, true, false, true, true, true, false, false, true, true, true, true, false, true, false, true, true, true, false, true, true, true, false, false, false, true, false, true, false, false, true, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, true, true, false, false, true, false, false, false, false, false, true, true, true, true, true, true, true, true, false, false, false, false, true, true, false, false, true, false, false, true, true, true, true, false, false, false, false, false, true, false, false, true, true, false, false, false, false, true, true, false, false, false, true, true, false, true, true, true, false, true, false, false, false, true, false, true, true, true, true, true, true, true, false, true, true, false, true, true, true, false, true, false, true, false, true, true, false, false, true, false, true, true, false, true, false, true, false, true, true, true, false, true, false, true, true, true, false, true, false, true, true, false, false, false, false, false, true, false, false, false, false, false, true, false, true, true, false, false, false, true, false, false, false, true, true, true, true, true, true, true, true, true, true, true, false, true, false, false, false, false, true, false, false, true, true, false, true, false};
    bool actual21[21 * 21];
    collect_data(&qrcode1, actual21);
    TEST_ASSERT_EQUAL_MEMORY(expected1, actual21, 21 * 21);

    QRCode qrcode2;
    uint8_t qrcode2Data[qrcode_getBufferSize(2)];
    qrcode_initText(&qrcode2, qrcode2Data, 2, 0, "0123456789");
    bool expected2[25 * 25]{true, true, true, true, true, true, true, false, true, false, false, true, true, false, false, true, true, false, true, true, true, true, true, true, true, true, false, false, false, false, false, true, false, true, true, true, false, true, true, true, false, true, false, true, false, false, false, false, false, true, true, false, true, true, true, false, true, false, false, false, true, true, true, false, true, true, true, false, true, false, true, true, true, false, true, true, false, true, true, true, false, true, false, false, false, true, false, false, false, true, false, false, false, true, false, true, true, true, false, true, true, false, true, true, true, false, true, false, true, true, true, true, false, true, true, true, false, false, true, false, true, true, true, false, true, true, false, false, false, false, false, true, false, true, false, false, true, false, false, false, true, false, false, true, false, false, false, false, false, true, true, true, true, true, true, true, true, false, true, false, true, false, true, false, true, false, true, false, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, true, false, true, true, true, false, true, true, true, false, false, false, false, false, false, false, false, true, true, true, false, false, true, true, false, true, true, true, false, false, true, true, false, false, true, true, true, true, false, false, true, true, true, false, false, true, false, true, false, false, false, true, true, false, false, true, true, false, false, true, false, false, false, true, false, false, true, true, false, false, true, true, true, true, false, false, false, false, true, false, false, false, true, false, false, false, false, true, true, false, false, true, true, false, false, false, false, true, false, true, false, true, false, false, false, true, false, false, false, false, true, false, false, false, true, true, false, false, true, true, false, false, false, true, false, true, true, false, true, true, true, false, true, true, false, false, true, true, true, false, false, true, false, true, false, false, false, false, false, true, true, true, false, false, true, false, false, false, true, false, false, true, true, true, false, false, false, true, true, true, false, true, false, true, true, true, true, false, false, true, true, true, false, true, true, true, true, false, true, false, false, false, false, false, true, true, false, false, false, false, true, true, true, true, true, false, true, true, true, true, false, true, true, false, true, false, false, true, true, false, false, false, true, true, false, true, false, false, false, false, true, true, false, true, true, true, true, true, false, true, true, true, false, false, false, false, false, false, false, false, true, true, true, false, false, true, true, true, true, false, false, false, true, false, false, false, false, true, true, true, true, true, true, true, false, false, true, false, true, false, false, false, true, true, false, true, false, true, true, true, true, false, true, false, false, false, false, false, true, false, true, true, false, false, false, true, false, false, true, false, false, false, true, false, false, false, true, true, false, true, true, true, false, true, false, false, true, false, true, true, true, false, true, true, true, true, true, true, true, true, true, true, true, false, true, true, true, false, true, false, false, false, true, false, true, false, false, true, false, false, false, true, false, true, true, false, false, true, false, true, true, true, false, true, false, true, false, false, false, true, true, true, true, true, false, true, true, true, true, false, true, true, true, false, false, false, false, false, true, false, true, true, true, true, true, false, true, false, false, true, true, true, false, true, true, true, false, true, true, true, true, true, true, true, false, true, false, false, false, false, true, true, false, false, true, false, true, true, true, true, true, true};
    bool actual25[25 * 25];
    collect_data(&qrcode2, actual25);
    TEST_ASSERT_EQUAL_MEMORY(expected2, actual25, 25 * 25);

    QRCode qrcode3;
    uint8_t qrcode3Data[qrcode_getBufferSize(2)];
    qrcode_initText(&qrcode3, qrcode3Data, 2, 1, "0123456789");
    bool expected3[25 * 25]{true, true, true, true, true, true, true, false, true, true, false, false, true, false, false, true, true, false, true, true, true, true, true, true, true, true, false, false, false, false, false, true, false, false, true, false, true, false, true, true, false, true, false, true, false, false, false, false, false, true, true, false, true, true, true, false, true, false, true, false, true, true, false, false, true, true, true, false, true, false, true, true, true, false, true, true, false, true, true, true, false, true, false, false, true, false, false, true, false, true, false, false, false, true, false, true, true, true, false, true, true, false, true, true, true, false, true, false, false, true, false, false, false, true, true, true, false, false, true, false, true, true, true, false, true, true, false, false, false, false, false, true, false, true, true, true, true, true, false, false, true, false, false, true, false, false, false, false, false, true, true, true, true, true, true, true, true, false, true, false, true, false, true, false, true, false, true, false, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, true, false, true, true, true, false, false, false, false, false, false, false, false, true, false, true, false, false, false, true, true, false, false, false, false, true, true, true, false, false, false, false, true, false, false, true, false, true, false, false, false, false, false, true, false, true, true, false, false, false, false, true, true, false, false, true, false, false, false, true, false, false, true, false, true, true, true, true, false, true, true, true, true, true, true, false, false, false, true, false, false, false, false, true, true, false, false, true, true, false, true, true, false, true, false, false, true, true, false, true, false, true, false, false, false, false, true, false, false, false, true, true, false, false, false, true, false, true, true, true, true, false, false, true, false, true, true, false, true, true, false, false, true, true, true, false, false, true, false, false, true, false, false, true, false, true, false, false, true, false, true, false, false, false, true, false, false, true, true, true, false, false, false, true, true, false, true, true, false, true, false, true, false, true, false, true, true, true, false, true, true, true, true, false, true, false, false, false, false, false, false, false, true, false, false, false, false, false, false, true, true, false, true, true, true, true, false, true, true, false, true, false, false, true, true, true, false, false, false, true, true, true, false, false, false, false, true, true, false, true, true, true, true, true, false, true, true, true, false, false, false, false, false, false, false, false, true, true, false, false, false, true, true, true, true, false, false, false, true, false, false, false, false, true, true, true, true, true, true, true, false, true, false, true, true, false, false, false, true, true, false, true, false, true, true, true, true, false, true, false, false, false, false, false, true, false, false, false, true, false, false, true, false, false, true, false, false, false, true, false, false, false, true, true, false, true, true, true, false, true, false, false, true, false, true, true, true, false, true, true, true, true, true, true, true, true, true, true, true, false, true, true, true, false, true, false, false, false, false, false, true, false, false, true, false, false, false, true, false, true, true, false, false, true, false, true, true, true, false, true, false, true, false, false, false, true, true, true, true, true, false, true, true, true, true, false, true, true, true, false, false, false, false, false, true, false, false, true, false, true, true, false, true, false, false, true, true, true, false, true, true, true, false, true, true, true, true, true, true, true, false, true, false, true, false, false, true, true, false, false, true, false, true, true, true, true, true, true};
    collect_data(&qrcode3, actual25);
    TEST_ASSERT_EQUAL_MEMORY(expected3, actual25, 25 * 25);

    QRCode qrcode4;
    uint8_t qrcode4Data[qrcode_getBufferSize(2)];
    qrcode_initText(&qrcode4, qrcode4Data, 2, 2, "0123456789");
    bool expected4[25 * 25]{true, true, true, true, true, true, true, false, true, true, false, false, true, true, false, false, true, false, true, true, true, true, true, true, true, true, false, false, false, false, false, true, false, false, false, false, true, false, true, true, true, false, false, true, false, false, false, false, false, true, true, false, true, true, true, false, true, false, true, false, false, true, true, true, false, false, true, false, true, false, true, true, true, false, true, true, false, true, true, true, false, true, false, true, true, true, true, true, true, true, true, false, false, true, false, true, true, true, false, true, true, false, true, true, true, false, true, false, false, true, true, false, true, true, true, false, true, false, true, false, true, true, true, false, true, true, false, false, false, false, false, true, false, true, false, false, false, false, true, true, false, false, false, true, false, false, false, false, false, true, true, true, true, true, true, true, true, false, true, false, true, false, true, false, true, false, true, false, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, true, false, false, true, false, false, true, false, false, false, false, false, false, false, false, false, false, false, true, false, true, false, true, true, true, true, false, false, true, true, false, false, true, false, true, true, true, false, true, true, false, true, false, true, true, true, true, true, false, true, false, false, false, true, true, false, true, true, false, false, false, true, false, false, false, true, true, true, true, false, true, true, true, true, true, false, false, false, false, true, false, false, false, true, true, false, false, true, false, true, true, true, false, true, false, true, false, false, false, false, false, true, false, true, false, false, true, true, false, false, true, true, true, true, true, true, false, true, true, false, true, true, false, true, true, false, false, true, true, true, false, false, false, true, true, false, false, true, false, false, true, true, false, true, false, false, false, true, false, false, false, true, false, false, true, true, false, true, false, true, false, true, true, false, true, true, false, true, false, false, true, true, true, true, false, false, true, true, true, true, false, false, true, true, true, true, false, true, false, false, false, false, false, true, true, true, true, false, false, true, false, false, true, true, false, true, true, false, true, false, false, false, true, true, true, true, false, true, true, false, false, true, true, true, true, true, true, true, true, true, false, false, true, true, true, true, true, true, true, false, false, true, false, false, false, false, false, false, false, false, true, false, false, false, false, false, true, false, true, false, false, false, true, true, false, false, false, true, true, true, true, true, true, true, false, true, true, true, true, true, false, false, false, true, false, true, false, true, false, true, false, false, true, false, false, false, false, false, true, false, true, false, false, true, true, false, true, true, true, false, false, false, true, true, true, true, true, true, false, true, true, true, false, true, false, false, true, true, true, false, true, true, false, true, true, true, true, true, false, true, true, true, true, false, true, true, true, false, true, false, true, false, false, false, false, false, false, false, false, true, false, false, false, false, true, true, false, true, false, true, true, true, false, true, false, false, true, false, false, true, true, false, false, false, true, true, true, true, false, true, false, true, true, false, false, false, false, false, true, false, true, true, true, true, true, true, true, true, false, true, true, false, true, false, true, true, false, true, true, true, true, true, true, true, false, false, false, true, true, true, false, true, true, false, false, false, false, true, false, true, false, true};
    collect_data(&qrcode4, actual25);
    TEST_ASSERT_EQUAL_MEMORY(expected4, actual25, 25 * 25);

    QRCode qrcode5;
    uint8_t qrcode5Data[qrcode_getBufferSize(2)];
    qrcode_initText(&qrcode5, qrcode5Data, 2, 3, "0123456789");
    bool expected5[25 * 25]{true, true, true, true, true, true, true, false, true, true, true, false, true, true, false, false, false, false, true, true, true, true, true, true, true, true, false, false, false, false, false, true, false, false, false, true, false, false, true, true, true, false, false, true, false, false, false, false, false, true, true, false, true, true, true, false, true, false, false, true, false, false, true, true, true, false, true, false, true, false, true, true, true, false, true, true, false, true, true, true, false, true, false, true, true, false, true, false, true, true, true, false, false, true, false, true, true, true, false, true, true, false, true, true, true, false, true, false, false, true, true, true, false, false, false, false, true, false, true, false, true, true, true, false, true, true, false, false, false, false, false, true, false, false, true, true, false, true, false, false, true, true, false, true, false, false, false, false, false, true, true, true, true, true, true, true, true, false, true, false, true, false, true, false, true, false, true, false, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, true, true, false, false, true, false, false, false, false, false, false, false, false, false, false, true, false, true, true, true, false, true, true, false, true, false, false, false, true, true, true, false, false, false, true, false, false, true, false, false, true, true, true, false, false, true, false, true, false, false, true, true, false, true, false, false, false, true, false, false, false, true, true, true, false, false, true, false, true, true, true, false, false, false, true, false, true, false, false, true, true, false, true, true, false, false, true, true, false, true, false, false, true, true, false, true, false, true, true, false, false, false, true, false, false, true, true, true, false, true, true, false, false, false, true, false, false, true, true, true, true, false, false, true, false, true, false, false, false, true, true, false, false, true, false, false, true, true, false, true, true, false, false, true, false, true, true, false, false, true, false, false, false, true, true, true, false, false, true, false, false, true, false, true, false, false, false, false, true, true, true, true, true, true, true, true, false, false, true, true, false, true, false, false, false, false, true, false, false, true, false, false, true, true, false, true, true, false, true, false, true, false, false, false, true, false, false, false, true, true, true, true, false, true, false, true, false, true, true, true, false, true, true, false, true, true, false, true, true, true, true, true, true, true, true, true, false, true, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false, false, true, false, false, false, true, true, false, true, false, true, true, true, true, true, true, true, false, false, false, false, true, true, false, true, false, true, false, true, false, true, false, true, false, false, true, false, false, false, false, false, true, false, true, false, true, false, true, true, false, true, true, false, false, false, true, true, false, true, true, true, false, true, true, true, false, true, false, true, false, true, true, true, true, false, false, true, true, true, true, true, false, true, false, true, true, false, true, true, true, false, true, false, false, false, true, false, false, true, false, false, false, true, false, false, false, false, true, true, false, true, false, true, true, true, false, true, false, true, false, true, true, false, false, true, false, true, true, true, false, true, false, false, false, true, true, false, false, false, false, false, true, false, false, true, false, true, false, true, true, true, false, false, true, false, false, false, true, false, false, true, true, true, true, true, true, true, false, false, true, true, false, true, true, false, true, false, false, false, false, true, false, true, false, true};
    collect_data(&qrcode5, actual25);
    TEST_ASSERT_EQUAL_MEMORY(expected5, actual25, 25 * 25);

    QRCode qrcode6;
    uint8_t qrcode6Data[qrcode_getBufferSize(2)];
    qrcode_initText(&qrcode6, qrcode6Data, 2, 1, "ABCEDFGHIJ");
    bool expected6[25 * 25]{true, true, true, true, true, true, true, false, true, false, true, true, true, false, false, true, true, false, true, true, true, true, true, true, true, true, false, false, false, false, false, true, false, true, false, false, true, false, false, false, false, true, false, true, false, false, false, false, false, true, true, false, true, true, true, false, true, false, true, true, true, false, true, false, false, true, true, false, true, false, true, true, true, false, true, true, false, true, true, true, false, true, false, false, true, true, true, false, false, false, false, true, false, true, false, true, true, true, false, true, true, false, true, true, true, false, true, false, true, true, false, true, true, false, true, true, true, false, true, false, true, true, true, false, true, true, false, false, false, false, false, true, false, false, false, true, true, false, false, false, true, true, false, true, false, false, false, false, false, true, true, true, true, true, true, true, true, false, true, false, true, false, true, false, true, false, true, false, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, true, true, false, true, true, false, false, false, false, false, false, false, false, true, false, false, true, true, true, true, true, true, false, true, true, true, true, false, false, false, true, false, false, true, false, true, true, true, false, false, true, true, true, true, false, true, true, false, false, true, false, true, false, false, true, true, true, false, true, true, true, true, true, false, true, true, true, true, false, true, false, false, true, false, false, false, true, false, true, true, false, false, true, true, true, false, false, true, true, false, false, true, true, true, false, true, true, false, false, true, false, true, false, false, true, true, false, false, false, false, false, true, true, true, false, false, false, false, true, true, false, true, true, false, true, true, true, false, true, true, false, false, true, true, false, true, false, true, true, false, false, false, true, true, false, true, false, true, true, false, false, true, true, false, true, false, true, false, false, true, false, true, true, true, true, true, true, true, true, true, true, true, true, false, true, true, true, false, false, true, false, true, true, true, true, false, true, false, true, false, true, false, false, true, false, false, false, true, false, true, false, false, false, true, false, true, true, true, false, false, true, false, true, true, false, true, true, false, true, true, false, false, false, true, false, false, false, true, false, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, true, true, true, true, true, true, false, false, false, true, true, true, false, true, true, true, true, true, true, true, true, false, true, false, true, true, false, false, false, true, true, false, true, false, true, false, false, true, true, true, false, false, false, false, false, true, false, true, true, false, false, true, false, true, false, true, false, false, false, true, false, false, true, true, true, false, true, true, true, false, true, false, true, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, true, true, true, false, true, false, true, true, true, false, false, false, true, true, true, false, true, false, true, false, false, false, true, true, false, true, true, true, false, true, false, false, true, true, false, true, false, true, true, false, false, true, false, true, false, true, true, true, true, false, false, false, false, false, true, false, false, false, false, false, false, false, true, false, true, false, false, true, false, false, true, true, false, true, true, true, true, true, true, true, false, true, false, false, false, false, true, true, false, false, true, false, false, false, false, false, true, true};
    collect_data(&qrcode6, actual25);
    TEST_ASSERT_EQUAL_MEMORY(expected6, actual25, 25 * 25);

    QRCode qrcode7;
    uint8_t qrcode7Data[qrcode_getBufferSize(2)];
    qrcode_initText(&qrcode7, qrcode7Data, 2, 1, "!@#$%^&*()");
    bool expected7[25 * 25]{true, true, true, true, true, true, true, false, true, true, false, true, true, true, true, true, false, false, true, true, true, true, true, true, true, true, false, false, false, false, false, true, false, false, false, false, true, false, false, false, true, false, false, true, false, false, false, false, false, true, true, false, true, true, true, false, true, false, true, true, false, false, true, true, false, false, false, false, true, false, true, true, true, false, true, true, false, true, true, true, false, true, false, false, false, true, true, false, false, false, true, true, false, true, false, true, true, true, false, true, true, false, true, true, true, false, true, false, false, true, true, false, false, false, false, true, true, false, true, false, true, true, true, false, true, true, false, false, false, false, false, true, false, true, false, true, true, false, true, true, false, true, false, true, false, false, false, false, false, true, true, true, true, true, true, true, true, false, true, false, true, false, true, false, true, false, true, false, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, true, false, true, false, false, false, false, false, false, false, false, false, false, false, true, false, true, false, false, false, true, true, false, true, false, true, true, true, false, true, true, false, false, true, false, false, true, false, true, false, true, false, false, true, false, false, true, false, false, false, false, false, false, false, false, true, false, true, false, false, false, false, false, true, true, true, true, false, true, true, true, false, true, false, true, true, false, true, true, false, true, true, false, true, true, false, false, true, true, false, true, false, true, false, true, false, false, false, true, true, false, true, false, true, true, true, true, false, false, true, false, false, false, false, true, true, true, false, true, true, true, true, false, false, true, true, false, true, true, false, false, true, true, true, false, true, false, true, true, false, true, false, true, false, true, false, false, false, false, true, false, false, true, true, false, false, false, false, false, false, true, false, false, false, true, true, true, true, false, false, true, true, true, true, true, true, false, false, false, true, false, false, false, true, false, true, true, false, true, false, false, false, false, true, false, false, true, true, false, false, false, false, true, false, false, false, false, true, false, true, false, false, true, false, true, true, true, true, false, true, true, true, true, false, true, true, true, true, false, true, true, true, true, true, true, true, false, true, true, false, false, false, false, false, false, false, false, true, false, false, false, true, false, false, false, true, false, false, false, true, true, false, false, true, true, true, true, true, true, true, true, false, true, false, true, false, true, true, true, true, true, false, true, false, true, false, true, true, true, true, false, false, false, false, false, true, false, false, true, true, true, true, false, true, true, true, false, false, false, true, true, false, false, true, true, false, true, true, true, false, true, false, false, true, false, false, false, true, true, false, true, true, true, true, true, false, true, true, false, true, false, true, true, true, false, true, false, false, true, false, false, false, true, true, false, true, true, true, false, true, true, true, false, false, true, false, true, true, true, false, true, false, true, false, true, true, false, false, false, false, false, true, true, true, true, true, false, true, true, true, false, false, false, false, false, true, false, false, false, true, false, false, true, false, true, false, false, false, false, true, false, false, false, false, true, true, true, true, true, true, true, false, true, false, false, true, true, true, false, true, true, false, true, false, true, true, false, false, true};
    collect_data(&qrcode7, actual25);
    TEST_ASSERT_EQUAL_MEMORY(expected7, actual25, 25 * 25);

    QRCode qrcode8;
    uint8_t qrcode8Data[qrcode_getBufferSize(8)];
    qrcode_initText(&qrcode8, qrcode8Data, 8, 1, "!@#$%^&*()");
    bool expected8[49 * 49]{true, true, true, true, true, true, true, false, false, true, false, true, true, false, true, true, true, false, true, true, false, true, false, true, true, false, false, false, false, false, true, true, false, true, false, true, true, false, false, false, true, false, true, true, true, true, true, true, true, true, false, false, false, false, false, true, false, false, false, true, true, false, false, false, false, true, true, false, false, true, false, false, false, true, true, false, true, false, true, true, false, false, false, false, false, true, true, true, true, true, false, true, false, false, false, false, false, true, true, false, true, true, true, false, true, false, true, false, false, false, true, true, true, true, false, false, true, false, false, true, false, true, true, false, false, true, false, false, true, false, false, true, false, false, true, false, false, true, true, false, true, false, true, true, true, false, true, true, false, true, true, true, false, true, false, true, false, true, true, true, true, false, true, false, true, false, true, false, false, false, false, true, true, false, false, true, true, true, true, true, false, false, true, false, false, false, true, false, false, true, false, true, true, true, false, true, true, false, true, true, true, false, true, false, true, true, false, false, true, false, false, true, true, true, false, false, false, true, true, true, true, true, true, true, true, false, true, false, true, true, false, false, false, false, false, false, false, false, true, false, true, true, true, false, true, true, false, false, false, false, false, true, false, true, false, true, false, false, false, false, false, false, false, true, true, false, true, true, false, false, false, true, false, true, true, true, true, true, false, false, true, false, false, true, false, false, false, true, false, false, false, false, false, true, true, true, true, true, true, true, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, true, true, true, false, false, true, false, false, false, false, true, false, false, true, true, false, false, false, true, true, false, false, true, false, false, true, false, false, true, false, false, true, true, false, false, false, false, false, false, false, false, true, false, true, true, true, true, true, false, false, false, false, false, false, true, false, true, true, false, true, false, false, false, true, true, true, true, true, false, true, true, true, true, true, false, false, true, false, true, false, false, false, false, true, true, true, true, true, false, false, true, false, false, true, false, true, false, true, false, false, true, false, false, true, false, false, true, true, false, true, true, true, false, true, true, false, false, true, true, false, true, false, true, true, false, false, false, false, false, true, false, false, true, true, false, false, true, false, true, false, true, true, true, false, false, true, false, false, false, true, false, false, false, true, false, false, false, true, false, true, false, false, false, true, true, false, false, true, true, true, true, true, false, false, true, false, true, false, false, false, true, true, false, false, true, true, true, false, false, true, false, true, false, false, false, false, false, false, true, false, false, false, false, false, true, false, false, true, true, true, false, false, true, false, false, true, false, false, true, false, false, true, false, false, true, false, false, true, false, true, false, false, false, false, true, false, false, true, false, true, true, true, true, true, true, true, true, false, false, true, true, false, true, true, true, true, false, false, false, false, true, false, true, false, false, true, true, true, true, true, false, false, true, false, true, false, true, false, false, false, true, true, true, false, true, false, false, false, true, false, true, false, false, false, false, true, false, false, false, false, true, true, true, false, false, true, true, false, true, false, false, false, false, true, true, false, true, false, true, true, false, false, false, false, false, false, true, false, true, false, true, true, false, false, true, false, false, false, false, true, true, true, true, false, true, true, true, true, false, true, false, false, false, false, false, true, true, true, true, false, true, false, false, true, true, true, true, true, false, false, true, false, true, false, true, true, false, false, true, true, false, false, true, true, false, true, true, false, false, true, false, false, true, true, true, false, true, true, true, true, true, true, true, true, true, false, true, false, true, false, false, true, false, true, false, false, false, false, true, false, true, true, true, false, true, false, true, false, false, true, false, true, true, false, true, false, true, false, true, true, true, true, false, false, true, true, true, true, true, true, true, true, true, false, true, false, true, false, true, false, false, true, false, false, true, true, true, true, true, false, false, true, true, false, true, true, true, true, false, false, false, true, true, false, false, false, true, false, false, false, false, false, true, true, false, false, false, true, true, false, false, false, false, false, true, false, false, false, false, true, true, true, false, false, true, false, true, false, false, true, true, false, true, false, true, false, true, true, false, false, true, false, false, true, false, false, false, true, true, true, false, false, true, false, true, false, true, true, false, true, true, false, true, true, true, false, true, false, false, true, false, true, true, true, true, false, true, false, false, false, true, true, true, true, true, true, true, false, true, false, false, false, false, false, false, false, false, true, true, true, false, true, false, true, true, false, false, false, true, false, false, false, true, false, true, true, true, false, false, true, false, true, false, false, true, true, true, true, true, false, true, false, true, true, false, true, false, false, false, true, false, true, true, false, true, true, false, false, false, false, true, true, false, true, false, true, true, true, true, false, true, false, true, false, false, true, true, false, false, false, false, false, true, true, false, true, false, true, true, true, true, true, true, false, true, true, true, false, false, true, false, false, true, true, false, false, false, false, true, true, true, false, true, true, true, true, false, false, false, false, true, false, false, true, true, false, false, true, false, true, false, false, true, true, true, true, true, true, false, false, true, true, false, true, false, true, true, false, true, false, true, true, true, true, true, true, true, true, false, false, false, false, false, true, true, true, true, false, false, true, true, true, true, true, true, false, false, true, false, false, true, false, false, true, false, false, true, true, true, true, true, true, false, false, false, true, true, true, false, true, true, false, false, false, true, false, false, true, false, false, true, false, true, true, true, false, false, true, true, false, false, false, true, false, false, true, false, true, false, false, true, true, true, true, true, true, true, false, false, false, true, true, false, true, true, false, true, true, true, true, false, true, false, true, true, false, true, false, false, true, true, true, false, true, true, false, false, true, false, true, false, true, true, false, false, false, false, false, true, true, false, true, false, true, true, true, false, true, false, true, false, false, false, true, false, true, false, true, true, false, false, false, true, true, true, false, false, true, true, false, true, true, false, false, false, false, true, false, false, false, true, false, false, true, false, true, false, false, true, true, true, true, true, false, true, false, false, false, true, true, false, true, true, true, false, false, true, true, true, true, true, true, true, true, true, true, true, false, false, false, true, true, true, false, true, true, true, true, true, true, true, false, false, true, false, false, true, false, false, true, false, false, true, true, true, true, true, true, false, false, false, true, true, false, false, false, false, true, false, false, false, false, true, false, false, true, false, true, false, false, true, false, false, false, false, true, false, false, false, false, false, true, false, true, false, false, true, true, true, true, true, true, true, true, true, true, false, true, false, true, true, false, false, true, false, false, true, true, true, true, false, true, false, true, true, true, false, true, false, true, true, false, false, true, false, true, true, true, true, false, false, false, false, false, true, true, false, true, false, true, false, false, true, false, false, false, false, false, false, true, false, false, true, false, false, true, false, false, false, false, true, false, false, true, false, true, false, false, true, false, false, false, false, true, false, false, true, false, false, true, false, true, false, false, true, true, true, true, true, false, true, false, false, true, true, false, true, false, false, false, true, true, false, false, true, true, true, true, false, false, true, true, true, false, false, true, true, false, true, false, true, true, false, false, true, true, true, false, false, true, false, false, true, false, false, true, false, false, false, true, true, false, false, true, true, true, false, true, true, false, true, true, true, false, false, true, true, true, true, true, true, true, false, true, true, true, true, false, false, true, false, true, false, false, false, false, false, false, true, true, false, true, false, true, true, false, false, true, false, false, true, true, true, false, true, false, false, true, true, true, false, false, false, true, false, true, false, true, true, false, false, true, false, false, false, false, false, false, true, false, true, true, true, true, true, false, true, true, false, false, false, false, false, true, true, false, true, true, true, true, true, false, true, false, false, false, false, false, true, false, false, false, false, false, false, false, true, true, false, false, false, false, true, true, false, true, true, true, true, false, true, false, true, false, false, false, true, true, false, true, false, true, true, false, false, true, false, false, false, false, false, true, false, true, true, false, false, false, false, true, false, true, false, false, false, true, false, true, true, true, true, false, true, true, false, true, true, false, true, true, true, true, true, false, true, true, false, false, false, false, false, true, true, false, true, true, false, true, true, false, false, true, false, true, true, true, false, false, true, false, false, false, false, true, false, false, true, false, false, false, false, true, false, true, true, true, true, false, true, false, false, true, false, false, true, false, false, true, false, false, true, false, false, true, false, false, false, false, false, true, false, true, true, true, false, true, false, false, false, true, false, true, false, false, true, false, true, true, false, false, false, false, false, false, true, false, true, true, true, true, false, true, true, true, true, true, false, false, true, false, true, false, true, true, true, false, true, false, true, true, false, true, false, false, true, false, true, false, false, true, true, true, false, false, true, true, true, true, true, false, true, true, true, true, true, false, true, false, true, true, true, false, true, false, true, true, false, false, false, false, false, true, false, false, false, false, false, false, true, false, true, false, true, false, false, false, true, true, true, false, false, true, true, true, false, true, false, true, true, false, false, true, true, false, true, true, true, true, false, true, true, true, true, true, false, false, true, false, true, false, true, false, true, false, true, false, true, true, true, false, false, true, true, true, false, false, false, false, true, false, false, false, false, true, false, true, false, false, true, true, true, true, true, false, true, false, false, true, false, false, true, false, false, true, false, false, true, false, false, false, false, false, true, false, false, false, true, false, false, true, true, true, false, false, false, true, true, false, true, false, true, true, true, false, false, false, false, false, false, false, true, true, true, true, true, true, false, true, true, true, true, true, false, false, true, false, true, false, true, true, true, true, true, true, true, false, true, false, false, false, false, false, false, false, false, false, true, false, true, false, false, false, true, false, true, false, true, true, true, true, true, false, false, false, true, true, true, false, true, false, true, true, false, false, false, false, false, true, true, false, false, false, true, false, true, false, true, true, true, true, true, true, true, true, false, false, false, true, false, true, true, true, false, true, false, false, false, true, false, true, false, true, false, true, false, true, true, true, true, true, false, false, true, false, true, false, false, true, false, true, false, true, true, true, true, false, true, false, false, false, false, false, true, false, true, false, false, true, true, false, true, false, true, false, true, true, true, true, true, false, false, false, true, true, false, false, true, false, false, true, false, false, true, false, false, true, true, false, false, false, true, false, true, false, false, true, false, true, true, true, false, true, false, true, false, false, true, true, true, true, false, false, false, false, false, false, true, true, true, true, true, true, false, true, true, true, true, true, false, false, true, false, true, false, false, true, true, true, true, true, true, false, true, false, true, false, true, true, true, false, true, false, true, false, false, false, true, true, false, false, true, true, true, true, true, false, false, true, true, false, false, true, true, false, true, false, true, true, false, false, false, false, false, false, false, true, false, false, true, true, false, false, true, true, false, true, true, true, false, true, false, true, false, false, true, true, false, false, false, true, false, false, false, true, true, false, false, true, true, false, false, true, true, true, true, true, false, false, true, false, true, false, true, false, false, false, false, false, false, false, false, true, true, false, false, false, false, false, true, false, false, false, false, false, true, false, false, false, true, true, true, true, true, false, false, true, true, false, false, true, false, true, false, false, false, false, true, false, true, true, true, false, false, true, false, false, false, true, false, false, true, true, true, true, true, true, true, true, false, true, true, false, true, true, true, false, false, false, false, false, true, false, true, false, false, true, true, false, false, true, false, false, true, true, true, true, true, false, false, true, true, false, false, true, true, false, false, false, true, true};
    bool actual49[49 * 49];
    collect_data(&qrcode8, actual49);
    TEST_ASSERT_EQUAL_MEMORY(expected8, actual49, 49 * 49);
}

#pragma region testHelpers




void print_data(QRCode *qrcode) {
    char output[49U * 49U * 4U];
    output[0] = 0;
    strcat(output, "{");
    for (uint8_t y = 0; y < qrcode->size; y++) {
        for (uint8_t x = 0; x < qrcode->size; x++) {
            if (qrcode_getModule(qrcode, x, y)) {
                strcat(output, "1, ");
            } else {
                strcat(output, "0, ");
            }
        }
    }
    strcat(output, "}");
    TEST_MESSAGE(output);
}

void collect_data(QRCode *qrcode, bool *data) {
    for (uint8_t y = 0; y < qrcode->size; y++) {
        for (uint8_t x = 0; x < qrcode->size; x++) {
            data[y * qrcode->size + x] = qrcode_getModule(qrcode, x, y);
        }
    }
}

#pragma endregion

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_bb_init);
    //RUN_TEST(test_getGridSizeBytes);
    //RUN_TEST(test_getBufferSizeBytes);
 
    RUN_TEST(test_max);
    RUN_TEST(test_getAlphanumeric);
    RUN_TEST(test_isAlphanumeric);
    RUN_TEST(test_isNumeric);
    RUN_TEST(test_getModeBits);
 
    RUN_TEST(test_bufferSize);
    RUN_TEST(test_qrCodeSize);
    RUN_TEST(test_endresult);
    UNITY_END();
}