// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include <bitmatrix.hpp>

void setUp(void) {}
void tearDown(void) {}

void test_initialize() {
    bitMatrix<16U> testMatrix;
    TEST_ASSERT_EQUAL(16U, testMatrix.widthHeightInBits);
    TEST_ASSERT_EQUAL(256U, testMatrix.sizeInBits);
    TEST_ASSERT_EQUAL(32U, testMatrix.sizeInBytes);
    for (uint32_t byteIndex = 0; byteIndex < testMatrix.sizeInBytes; byteIndex++) {
        TEST_ASSERT_EQUAL(0, testMatrix.data[byteIndex]);
    }
}

void test_Offset() {
    bitMatrix<16U> testMatrix;
    TEST_ASSERT_EQUAL(0, testMatrix.byteOffset(0, 0));
    TEST_ASSERT_EQUAL(0, testMatrix.byteOffset(7, 0));
    TEST_ASSERT_EQUAL(1, testMatrix.byteOffset(8, 0));
    TEST_ASSERT_EQUAL(1, testMatrix.byteOffset(15, 0));

    TEST_ASSERT_EQUAL(2, testMatrix.byteOffset(0, 1));
    TEST_ASSERT_EQUAL(2, testMatrix.byteOffset(7, 1));
    TEST_ASSERT_EQUAL(3, testMatrix.byteOffset(8, 1));
    TEST_ASSERT_EQUAL(3, testMatrix.byteOffset(15, 1));

    TEST_ASSERT_EQUAL(31, testMatrix.byteOffset(15, 15));
}

void test_bitMask() {
    bitMatrix<16U> testMatrix;
    TEST_ASSERT_EQUAL(0b10000000, testMatrix.bitMask(0, 0));
    TEST_ASSERT_EQUAL(0b00000001, testMatrix.bitMask(7, 0));
    TEST_ASSERT_EQUAL(0b10000000, testMatrix.bitMask(0, 1));
    TEST_ASSERT_EQUAL(0b00000001, testMatrix.bitMask(7, 1));
    TEST_ASSERT_EQUAL(0b00000001, testMatrix.bitMask(7, 7));
}

void test_set_get() {
    bitMatrix<16U> testMatrix;
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
    bitMatrix<16U> testMatrix;
    testMatrix.setBit(0, 0);
    testMatrix.setBit(3, 3);
    testMatrix.setBit(5, 5);
    testMatrix.clearAllBits();
    for (uint32_t byteIndex = 0; byteIndex < testMatrix.sizeInBytes; byteIndex++) {
        TEST_ASSERT_EQUAL(0, testMatrix.data[byteIndex]);
    }
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_Offset);
    RUN_TEST(test_bitMask);
    RUN_TEST(test_set_get);
    RUN_TEST(test_clearAll);
    UNITY_END();
}