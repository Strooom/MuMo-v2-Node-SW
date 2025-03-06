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

uint8_t getFormatBits(uint8_t ecc) {
    static const uint8_t ECC_FORMAT_BITS = (0x02 << 6) | (0x03 << 4) | (0x00 << 2) | (0x01 << 0);
    return (ECC_FORMAT_BITS >> (2 * ecc)) & 0x03;
}

uint32_t getCapacityInBytes(uint32_t version, uint32_t errorCorrectionLevel) {
    static const uint16_t NUM_RAW_DATA_MODULES[40]              = {208, 359, 567, 807, 1079, 1383, 1568, 1936, 2336, 2768, 3232, 3728, 4256, 4651, 5243, 5867, 6523, 7211, 7931, 8683, 9252, 10068, 10916, 11796, 12708, 13652, 14628, 15371, 16411, 17483, 18587, 19723, 20891, 22091, 23008, 24272, 25568, 26896, 28256, 29648};
    static const uint16_t NUM_ERROR_CORRECTION_CODEWORDS[4][40] = {
        {7, 10, 15, 20, 26, 36, 40, 48, 60, 72, 80, 96, 104, 120, 132, 144, 168, 180, 196, 224, 224, 252, 270, 300, 312, 336, 360, 390, 420, 450, 480, 510, 540, 570, 570, 600, 630, 660, 720, 750},                                // Low
        {10, 16, 26, 36, 48, 64, 72, 88, 110, 130, 150, 176, 198, 216, 240, 280, 308, 338, 364, 416, 442, 476, 504, 560, 588, 644, 700, 728, 784, 812, 868, 924, 980, 1036, 1064, 1120, 1204, 1260, 1316, 1372},                    // Medium
        {13, 22, 36, 52, 72, 96, 108, 132, 160, 192, 224, 260, 288, 320, 360, 408, 448, 504, 546, 600, 644, 690, 750, 810, 870, 952, 1020, 1050, 1140, 1200, 1290, 1350, 1440, 1530, 1590, 1680, 1770, 1860, 1950, 2040},           // Quartile
        {17, 28, 44, 64, 88, 112, 130, 156, 192, 224, 264, 308, 352, 384, 432, 480, 532, 588, 650, 700, 750, 816, 900, 960, 1050, 1110, 1200, 1260, 1350, 1440, 1530, 1620, 1710, 1800, 1890, 1980, 2100, 2220, 2310, 2430},        // High
    };

    if (errorCorrectionLevel > 3) {
        return 0;
    }
    if (version == 0 || version > 40) {
        return 0;
    }

    uint16_t moduleCount  = NUM_RAW_DATA_MODULES[version - 1];
    uint16_t dataCapacity = moduleCount / 8 - NUM_ERROR_CORRECTION_CODEWORDS[errorCorrectionLevel][version - 1];
    return dataCapacity;
}

void encodeDataCodewords(BitBucket *dataCodewords, const uint8_t *text, uint16_t length, uint8_t version) {
    bb_appendBits(dataCodewords, 1 << MODE_NUMERIC, 4);
    bb_appendBits(dataCodewords, length, getModeBits(version, MODE_NUMERIC));

    uint16_t accumData = 0;
    uint8_t accumCount = 0;
    for (uint16_t i = 0; i < length; i++) {
        accumData = accumData * 10 + ((char)(text[i]) - '0');
        accumCount++;
        if (accumCount == 3) {
            bb_appendBits(dataCodewords, accumData, 10);
            accumData  = 0;
            accumCount = 0;
        }
    }

    // 1 or 2 digits remaining
    if (accumCount > 0) {
        bb_appendBits(dataCodewords, accumData, accumCount * 3 + 1);
    }
}

void test_log() {
    QRCode qrcode1;
    uint8_t qrcodeData1[qrcode_getBufferSize(1)];
    qrcode_initText(&qrcode1, qrcodeData1, 2, 0, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
}


void test_formatBits() {
    TEST_ASSERT_EQUAL(0x01, getFormatBits(0));
    TEST_ASSERT_EQUAL(0x00, getFormatBits(1));
    TEST_ASSERT_EQUAL(0x03, getFormatBits(2));
    TEST_ASSERT_EQUAL(0x02, getFormatBits(3));
}

void test_getCapacity() {
    TEST_ASSERT_EQUAL(19, getCapacityInBytes(1, 0));
    TEST_ASSERT_EQUAL(34, getCapacityInBytes(2, 0));
    TEST_ASSERT_EQUAL(55, getCapacityInBytes(3, 0));
    TEST_ASSERT_EQUAL(80, getCapacityInBytes(4, 0));

    TEST_ASSERT_EQUAL(16, getCapacityInBytes(1, 1));
    TEST_ASSERT_EQUAL(28, getCapacityInBytes(2, 1));
    TEST_ASSERT_EQUAL(44, getCapacityInBytes(3, 1));
    TEST_ASSERT_EQUAL(64, getCapacityInBytes(4, 1));

    TEST_ASSERT_EQUAL(0, getCapacityInBytes(0, 0));
    TEST_ASSERT_EQUAL(0, getCapacityInBytes(41, 0));
    TEST_ASSERT_EQUAL(0, getCapacityInBytes(1, 4));
}

void test_encode_orig() {
    QRCode qrcode1;
    uint8_t qrcode1Data[qrcode_getBufferSize(1)];
    qrcode_initText(&qrcode1, qrcode1Data, 1, 0, "9876543210");
}

void test_encodeDataCodewords_numeric() {
    static const uint16_t NUM_RAW_DATA_MODULES[40]              = {208, 359, 567, 807, 1079, 1383, 1568, 1936, 2336, 2768, 3232, 3728, 4256, 4651, 5243, 5867, 6523, 7211, 7931, 8683, 9252, 10068, 10916, 11796, 12708, 13652, 14628, 15371, 16411, 17483, 18587, 19723, 20891, 22091, 23008, 24272, 25568, 26896, 28256, 29648};
    static const uint16_t NUM_ERROR_CORRECTION_CODEWORDS[4][40] = {
        {7, 10, 15, 20, 26, 36, 40, 48, 60, 72, 80, 96, 104, 120, 132, 144, 168, 180, 196, 224, 224, 252, 270, 300, 312, 336, 360, 390, 420, 450, 480, 510, 540, 570, 570, 600, 630, 660, 720, 750},                                // Low
        {10, 16, 26, 36, 48, 64, 72, 88, 110, 130, 150, 176, 198, 216, 240, 280, 308, 338, 364, 416, 442, 476, 504, 560, 588, 644, 700, 728, 784, 812, 868, 924, 980, 1036, 1064, 1120, 1204, 1260, 1316, 1372},                    // Medium
        {13, 22, 36, 52, 72, 96, 108, 132, 160, 192, 224, 260, 288, 320, 360, 408, 448, 504, 546, 600, 644, 690, 750, 810, 870, 952, 1020, 1050, 1140, 1200, 1290, 1350, 1440, 1530, 1590, 1680, 1770, 1860, 1950, 2040},           // Quartile
        {17, 28, 44, 64, 88, 112, 130, 156, 192, 224, 264, 308, 352, 384, 432, 480, 532, 588, 650, 700, 750, 816, 900, 960, 1050, 1110, 1200, 1260, 1350, 1440, 1530, 1620, 1710, 1800, 1890, 1980, 2100, 2220, 2310, 2430},        // High
    };

    uint32_t testVersion{1};
    uint32_t errorCorrectionLevel{0};
    uint8_t testData[10U]{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    BitBucket testOutput;
    uint16_t moduleCount  = NUM_RAW_DATA_MODULES[testVersion - 1];
    uint16_t dataCapacity = moduleCount / 8 - NUM_ERROR_CORRECTION_CODEWORDS[errorCorrectionLevel][testVersion - 1];

    struct BitBucket codewords;
    uint8_t codewordBytes[bb_getBufferSizeBytes(moduleCount)];
    bb_initBuffer(&codewords, codewordBytes, (int32_t)sizeof(codewordBytes));

    encodeDataCodewords(&codewords, testData, 10U, testVersion);
}

void test_Sizes() {
    TEST_ASSERT_EQUAL(0, bb_getGridSizeBytes(0));
    TEST_ASSERT_EQUAL(8, bb_getGridSizeBytes(8));
    TEST_ASSERT_EQUAL(2, bb_getGridSizeBytes(3));

    TEST_ASSERT_EQUAL(0, bb_getBufferSizeBytes(0));
    TEST_ASSERT_EQUAL(1, bb_getBufferSizeBytes(1));
    TEST_ASSERT_EQUAL(1, bb_getBufferSizeBytes(8));
    TEST_ASSERT_EQUAL(2, bb_getBufferSizeBytes(9));
}

void test_bb_init() {
    uint8_t testStorage[8U];
    BitBucket testBucket;
    bb_initGrid(&testBucket, testStorage, 8U);
    TEST_ASSERT_EACH_EQUAL_UINT8(0, testStorage, 8);
}

void test_bb_setGet() {
    uint8_t testStorage[8U];
    BitBucket testBucket;
    bb_initGrid(&testBucket, testStorage, 8U);
    bb_setBit(&testBucket, 0, 0, true);
    TEST_ASSERT_TRUE(bb_getBit(&testBucket, 0, 0));
    bb_setBit(&testBucket, 0, 0, false);
    TEST_ASSERT_FALSE(bb_getBit(&testBucket, 0, 0));
    bb_setBit(&testBucket, 3, 3, true);
    TEST_ASSERT_TRUE(bb_getBit(&testBucket, 3, 3));
    bb_setBit(&testBucket, 3, 3, false);
    TEST_ASSERT_FALSE(bb_getBit(&testBucket, 3, 3));

    bb_invertBit(&testBucket, 5, 5, true);
    TEST_ASSERT_TRUE(bb_getBit(&testBucket, 5, 5));
    bb_invertBit(&testBucket, 5, 5, true);
    TEST_ASSERT_FALSE(bb_getBit(&testBucket, 5, 5));

    bb_invertBit(&testBucket, 7, 7, false);
    TEST_ASSERT_FALSE(bb_getBit(&testBucket, 7, 7));
    bb_invertBit(&testBucket, 7, 7, false);
    TEST_ASSERT_FALSE(bb_getBit(&testBucket, 7, 7));
}

void test_bb_applyMask() {
    uint8_t testStorage1[8U];
    BitBucket testBucket1;
    bb_initGrid(&testBucket1, testStorage1, 8U);
    uint8_t testStorage2[8U];
    BitBucket testBucket2;
    bb_initGrid(&testBucket2, testStorage2, 8U);
    applyMask(&testBucket1, &testBucket2, 0);

    // TEST_ASSERT_EQUAL_MEMORY(testStorage1, testStorage2, 8);
    // applyMask(&testBucket1, &testBucket2, 1);
    // TEST_ASSERT_EQUAL_MEMORY(testStorage1, testStorage2, 8);
    // applyMask(&testBucket1, &testBucket2, 2);
    // TEST_ASSERT_EQUAL_MEMORY(testStorage1, testStorage2, 8);
    // applyMask(&testBucket1, &testBucket2, 3);
    // TEST_ASSERT_EQUAL_MEMORY(testStorage1, testStorage2, 8);
}

//   Not sure what this is doing ??  void appendBits(uint32_t val, uint8_t length);
//   Not sure what this is doing ??  void initBuffer(uint8_t *data, int32_t capacityBytes);

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
    RUN_TEST(test_log);

    // RUN_TEST(test_encode_orig);
    // RUN_TEST(test_formatBits);
    // RUN_TEST(test_getCapacity);
    // RUN_TEST(test_encodeDataCodewords_numeric);
    // RUN_TEST(test_Sizes);

    // RUN_TEST(test_bb_init);
    // RUN_TEST(test_bb_setGet);
    // RUN_TEST(test_bb_applyMask);

    // RUN_TEST(test_max);
    // RUN_TEST(test_getAlphanumeric);
    // RUN_TEST(test_isAlphanumeric);
    // RUN_TEST(test_isNumeric);
    // RUN_TEST(test_getModeBits);

    // RUN_TEST(test_bufferSize);
    // RUN_TEST(test_qrCodeSize);
    // RUN_TEST(test_endresult);
    UNITY_END();
}