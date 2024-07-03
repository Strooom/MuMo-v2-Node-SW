// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include <qrcode.hpp>

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_initialize() {
}

void test_getAlphanumeric() {
    TEST_ASSERT_EQUAL(0, qrCode::getAlphanumeric('0'));
    TEST_ASSERT_EQUAL(1, qrCode::getAlphanumeric('1'));
    TEST_ASSERT_EQUAL(8, qrCode::getAlphanumeric('8'));
    TEST_ASSERT_EQUAL(9, qrCode::getAlphanumeric('9'));
    TEST_ASSERT_EQUAL(10, qrCode::getAlphanumeric('A'));
    TEST_ASSERT_EQUAL(11, qrCode::getAlphanumeric('B'));
    TEST_ASSERT_EQUAL(34, qrCode::getAlphanumeric('Y'));
    TEST_ASSERT_EQUAL(35, qrCode::getAlphanumeric('Z'));
    TEST_ASSERT_EQUAL(36, qrCode::getAlphanumeric(' '));
    TEST_ASSERT_EQUAL(37, qrCode::getAlphanumeric('$'));
    TEST_ASSERT_EQUAL(38, qrCode::getAlphanumeric('%'));
    TEST_ASSERT_EQUAL(39, qrCode::getAlphanumeric('*'));
    TEST_ASSERT_EQUAL(40, qrCode::getAlphanumeric('+'));
    TEST_ASSERT_EQUAL(41, qrCode::getAlphanumeric('-'));
    TEST_ASSERT_EQUAL(42, qrCode::getAlphanumeric('.'));
    TEST_ASSERT_EQUAL(43, qrCode::getAlphanumeric('/'));
    TEST_ASSERT_EQUAL(44, qrCode::getAlphanumeric(':'));
    TEST_ASSERT_EQUAL(-1, qrCode::getAlphanumeric('?'));
    TEST_ASSERT_EQUAL(-1, qrCode::getAlphanumeric('a'));
}

void test_isAlphanumeric() {
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric("0", 1));
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric("9", 1));
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric("A", 1));
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric("Z", 1));
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric(" ", 1));
    TEST_ASSERT_FALSE(qrCode::isAlphanumeric("?", 1));
    TEST_ASSERT_FALSE(qrCode::isAlphanumeric("?", 1));
    TEST_ASSERT_FALSE(qrCode::isAlphanumeric("\n", 1));

    TEST_ASSERT_TRUE(qrCode::isAlphanumeric("HELLO WORLD", 11));
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric("1234567890", 10));
    TEST_ASSERT_FALSE(qrCode::isAlphanumeric(" ? ", 3));
}

void test_isNumeric() {
    TEST_ASSERT_TRUE(qrCode::isNumeric("1234567890", 10));
    TEST_ASSERT_FALSE(qrCode::isNumeric("?", 1));
    TEST_ASSERT_FALSE(qrCode::isNumeric("\n", 1));
    TEST_ASSERT_FALSE(qrCode::isNumeric("000 000", 7));
}

void test_getGridSizeBytes() {
    TEST_IGNORE_MESSAGE("to be continued");
    // TEST_ASSERT_EQUAL(1, qrCode::bb_getGridSizeBytes(0));
    // TEST_ASSERT_EQUAL(1, qrCode::bb_getGridSizeBytes(8));
}

void test_getBufferSizeBytes() {
    TEST_IGNORE_MESSAGE("to be continued");
    // TEST_ASSERT_EQUAL(1, qrCode::bb_getBufferSizeBytes(0));
    // TEST_ASSERT_EQUAL(1, qrCode::bb_getBufferSizeBytes(8));
}

void test_max() {
    TEST_ASSERT_EQUAL(5, qrCode::max(0, 5));
    TEST_ASSERT_EQUAL(5, qrCode::max(5, 0));
    TEST_ASSERT_EQUAL(5, qrCode::max(5, 5));
    TEST_ASSERT_EQUAL(5, qrCode::max(5, -5));
    TEST_ASSERT_EQUAL(5, qrCode::max(-5, 5));
    TEST_ASSERT_EQUAL(-5, qrCode::max(-5, -5));
}

void test_bufferSize() {
    TEST_ASSERT_EQUAL(56, qrcode_getBufferSize(1));
    TEST_ASSERT_EQUAL(79, qrcode_getBufferSize(2));
    TEST_ASSERT_EQUAL(172, qrcode_getBufferSize(5));
}

void test_qrCodeSize() {
    QRCode qrcode1;
    uint8_t qrcodeData1[qrcode_getBufferSize(1)];
    qrcode_initText(&qrcode1, qrcodeData1, 1, ECC_MEDIUM, "Hello World");
    TEST_ASSERT_EQUAL(21, qrcode1.size);        // The size of the QR code is (4 * version + 17)

    QRCode qrcode2;
    uint8_t qrcodeData2[qrcode_getBufferSize(2)];
    qrcode_initText(&qrcode2, qrcodeData2, 2, ECC_MEDIUM, "Hello World");
    TEST_ASSERT_EQUAL(25, qrcode2.size);

    QRCode qrcode3;
    uint8_t qrcodeData3[qrcode_getBufferSize(5)];
    qrcode_initText(&qrcode3, qrcodeData3, 5, ECC_MEDIUM, "Hello World");
    TEST_ASSERT_EQUAL(37, qrcode3.size);
}

void test_endresult() {
    static constexpr int testSize{2};
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(2)];
    qrcode_initText(&qrcode, qrcodeData, 2, ECC_MEDIUM, "Hello World");

    bool expected[testSize][testSize] = {
        {false, true},
        {true, false}};

    for (uint8_t y = 0; y < qrcode.size; y++) {
        for (uint8_t x = 0; x < qrcode.size; x++) {
            if (qrcode_getModule(&qrcode, x, y)) {
            }
        }
    }
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_getAlphanumeric);
    RUN_TEST(test_isAlphanumeric);
    RUN_TEST(test_isNumeric);
    RUN_TEST(test_getGridSizeBytes);
    RUN_TEST(test_getBufferSizeBytes);
    RUN_TEST(test_max);
    RUN_TEST(test_bufferSize);
    RUN_TEST(test_qrCodeSize);
    // RUN_TEST(test_endresult);
    UNITY_END();
}