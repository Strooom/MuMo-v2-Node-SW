// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include <bitmap.hpp>

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_bytesPerRow() {
    const uint8_t testPixelData[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    bitmap testBitMap1{8, 8, testPixelData};
    TEST_ASSERT_EQUAL(1, testBitMap1.bytesPerRow);

    bitmap testBitMap2{12, 8, testPixelData};
    TEST_ASSERT_EQUAL(2, testBitMap2.bytesPerRow);

    bitmap testBitMap3{16, 8, testPixelData};
    TEST_ASSERT_EQUAL(2, testBitMap3.bytesPerRow);

    bitmap testBitMap4{20, 8, testPixelData};
    TEST_ASSERT_EQUAL(3, testBitMap4.bytesPerRow);
}

void test_inBounds() {
    const uint8_t testPixelData[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    bitmap testBitMap{8, 8, testPixelData};

    TEST_ASSERT_TRUE(testBitMap.isInBounds(0, 0));
    TEST_ASSERT_TRUE(testBitMap.isInBounds(7, 7));
    TEST_ASSERT_FALSE(testBitMap.isInBounds(8, 0));
    TEST_ASSERT_FALSE(testBitMap.isInBounds(0, 8));
    TEST_ASSERT_FALSE(testBitMap.isInBounds(8, 8));
}

void test_getBitIndex() {
    const uint8_t testPixelData[3] = {0, 0, 0};
    bitmap testBitMap{20, 1, testPixelData};
    TEST_ASSERT_EQUAL(7, testBitMap.getBitIndex(0));
    TEST_ASSERT_EQUAL(0, testBitMap.getBitIndex(7));
    TEST_ASSERT_EQUAL(7, testBitMap.getBitIndex(8));
    TEST_ASSERT_EQUAL(0, testBitMap.getBitIndex(15));
    TEST_ASSERT_EQUAL(7, testBitMap.getBitIndex(16));
    TEST_ASSERT_EQUAL(4, testBitMap.getBitIndex(19));
}

void test_getByteIndex() {
    const uint8_t testPixelData[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    bitmap testBitMap{16, 4, testPixelData};
    TEST_ASSERT_EQUAL(6, testBitMap.getByteIndex(0, 0));
    TEST_ASSERT_EQUAL(6, testBitMap.getByteIndex(7, 0));
    TEST_ASSERT_EQUAL(7, testBitMap.getByteIndex(8, 0));
    TEST_ASSERT_EQUAL(7, testBitMap.getByteIndex(15, 0));
    TEST_ASSERT_EQUAL(0, testBitMap.getByteIndex(0, 3));
    TEST_ASSERT_EQUAL(0, testBitMap.getByteIndex(7, 3));
    TEST_ASSERT_EQUAL(1, testBitMap.getByteIndex(8, 3));
    TEST_ASSERT_EQUAL(1, testBitMap.getByteIndex(15, 3));
}

void test_getPixel() {
    const uint8_t testPixelData[8] = {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55};
    bitmap testBitMap{8, 8, testPixelData};
    for (uint32_t x = 0; x < 8; x++) {
        for (uint32_t y = 0; y < 8; y++) {
            if ((x + y) % 2 == 0) {
                TEST_ASSERT_FALSE(testBitMap.getPixel(x, y));
            } else {
                TEST_ASSERT_TRUE(testBitMap.getPixel(x, y));
            }
        }
    }
}

void test_getPixelOutOfBounds() {
    const uint8_t testPixelData[8] = {0xFF, 0xFF};
    bitmap testBitMap{16, 1, testPixelData};
    TEST_ASSERT_TRUE(testBitMap.getPixel(0, 0));
    TEST_ASSERT_TRUE(testBitMap.getPixel(15, 0));
    TEST_ASSERT_FALSE(testBitMap.getPixel(16, 0));
    TEST_ASSERT_FALSE(testBitMap.getPixel(0, 1));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_bytesPerRow);
    RUN_TEST(test_inBounds);
    RUN_TEST(test_getBitIndex);
    RUN_TEST(test_getByteIndex);
    RUN_TEST(test_getPixel);
    RUN_TEST(test_getPixelOutOfBounds);
    UNITY_END();
}