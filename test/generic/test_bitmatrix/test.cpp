// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include <bitmatrix.hpp>

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_neededLengthForExtBuffer() {
    TEST_ASSERT_EQUAL(0, bitMatrix::neededLengthInBytes(0));
    TEST_ASSERT_EQUAL(2, bitMatrix::neededLengthInBytes(3));
    TEST_ASSERT_EQUAL(2, bitMatrix::neededLengthInBytes(4));
    TEST_ASSERT_EQUAL(4, bitMatrix::neededLengthInBytes(5));
    TEST_ASSERT_EQUAL(32, bitMatrix::neededLengthInBytes(16));
}


void test_initialize() {
    uint32_t testLength{40};
    uint8_t storage[bitMatrix::neededLengthInBytes(testLength)];
    bitMatrix testMatrix{storage, testLength};
    TEST_ASSERT_EQUAL(testLength, testMatrix.widthHeight);
    TEST_ASSERT_EQUAL(40, testMatrix.widthHeightInBits());
    for (uint32_t byteIndex = 0; byteIndex < testMatrix.lengthInBytes; byteIndex++) {
        TEST_ASSERT_EQUAL(0, testMatrix.data[byteIndex]);
    }
}

void test_Offset() {
    uint32_t testLength{8};
    uint8_t storage[bitMatrix::neededLengthInBytes(testLength)];
    bitMatrix testMatrix{storage, testLength};
    TEST_ASSERT_EQUAL(0, testMatrix.byteOffset(0, 0));
    TEST_ASSERT_EQUAL(0, testMatrix.byteOffset(7, 0));
    TEST_ASSERT_EQUAL(1, testMatrix.byteOffset(0, 1));
    TEST_ASSERT_EQUAL(1, testMatrix.byteOffset(7, 1));
    TEST_ASSERT_EQUAL(7, testMatrix.byteOffset(7, 7));
}

void test_bitMask() {
    uint32_t testLength{8};
    uint8_t storage[bitMatrix::neededLengthInBytes(testLength)];
    bitMatrix testMatrix{storage, testLength};
    TEST_ASSERT_EQUAL(0b10000000, testMatrix.bitMask(0, 0));
    TEST_ASSERT_EQUAL(0b00000001, testMatrix.bitMask(7, 0));
    TEST_ASSERT_EQUAL(0b10000000, testMatrix.bitMask(0, 1));
    TEST_ASSERT_EQUAL(0b00000001, testMatrix.bitMask(7, 1));
    TEST_ASSERT_EQUAL(0b00000001, testMatrix.bitMask(7, 7));
}



void test_set_get() {
    uint32_t testLength{8};
    uint8_t storage[bitMatrix::neededLengthInBytes(testLength)];
    bitMatrix testMatrix{storage, testLength};
    testMatrix.setBit(0, 0);
    TEST_ASSERT_TRUE(testMatrix.getBit(0, 0));
    testMatrix.clearBit(0, 0);
    TEST_ASSERT_FALSE(testMatrix.getBit(0, 0));

    testMatrix.setBit(3, 3);
    TEST_ASSERT_TRUE(testMatrix.getBit(3, 3));
    testMatrix.clearBit(3, 3);
    TEST_ASSERT_FALSE(testMatrix.getBit(3, 3));

    testMatrix.setBit(5, 5);
    testMatrix.invertBit(5, 5);
    TEST_ASSERT_FALSE(testMatrix.getBit(5, 5));
    testMatrix.invertBit(5, 5);
    TEST_ASSERT_TRUE(testMatrix.getBit(5, 5));
}

void test_clearAll() {
    uint32_t testLength{8};
    uint8_t storage[bitMatrix::neededLengthInBytes(testLength)];
    bitMatrix testMatrix{storage, testLength};
    testMatrix.setBit(0, 0);
    testMatrix.setBit(3, 3);
    testMatrix.setBit(5, 5);
    testMatrix.clearAllBits();
    for (uint32_t byteIndex = 0; byteIndex < testMatrix.lengthInBytes; byteIndex++) {
        TEST_ASSERT_EQUAL(0, testMatrix.data[byteIndex]);
    }
}

// void test_mask() {
//     TEST_ASSERT_TRUE(bitArray<8>::mask(0, 0, 0));
//     TEST_ASSERT_TRUE(bitArray<8>::mask(0, 0, 1));
//     TEST_ASSERT_TRUE(bitArray<8>::mask(0, 0, 2));
//     TEST_ASSERT_TRUE(bitArray<8>::mask(0, 0, 3));
//     TEST_ASSERT_TRUE(bitArray<8>::mask(0, 0, 4));
//     TEST_ASSERT_TRUE(bitArray<8>::mask(0, 0, 5));
//     TEST_ASSERT_TRUE(bitArray<8>::mask(0, 0, 6));
//     TEST_ASSERT_TRUE(bitArray<8>::mask(0, 0, 7));

//     TEST_ASSERT_FALSE(bitArray<8>::mask(1, 0, 0));
//     TEST_ASSERT_TRUE(bitArray<8>::mask(1, 0, 1));
//     TEST_ASSERT_FALSE(bitArray<8>::mask(1, 0, 2));
//     TEST_ASSERT_FALSE(bitArray<8>::mask(1, 0, 3));
//     TEST_ASSERT_TRUE(bitArray<8>::mask(1, 0, 4));
//     TEST_ASSERT_TRUE(bitArray<8>::mask(1, 0, 5));
//     TEST_ASSERT_TRUE(bitArray<8>::mask(1, 0, 6));
//     TEST_ASSERT_FALSE(bitArray<8>::mask(1, 0, 7));

// // TODO : check if more is needed

//     TEST_ASSERT_FALSE(bitArray<8>::mask(0, 0, 100U));

// }

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_neededLengthForExtBuffer);
    RUN_TEST(test_initialize);
    RUN_TEST(test_Offset);
    RUN_TEST(test_bitMask);
    RUN_TEST(test_set_get);
    RUN_TEST(test_clearAll);
    // RUN_TEST(test_mask);
    UNITY_END();
}