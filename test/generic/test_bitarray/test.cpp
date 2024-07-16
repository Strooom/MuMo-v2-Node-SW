// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include <bitarray.hpp>

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_Sizes() {
    bitArray<8> testArray;
    TEST_ASSERT_EQUAL(8, testArray.widthHeightInBits);
    TEST_ASSERT_EQUAL(64, testArray.sizeInBits);
    TEST_ASSERT_EQUAL(8, testArray.sizeInBytes);
}

void test_initialize() {
    bitArray<8> testArray;
    for (uint32_t byteIndex = 0; byteIndex < testArray.sizeInBytes; byteIndex++) {
        TEST_ASSERT_EQUAL(0, testArray.data[byteIndex]);
    }
}

void test_Offset() {
    bitArray<8> testArray;
    TEST_ASSERT_EQUAL(0, testArray.byteOffset(0, 0));
    TEST_ASSERT_EQUAL(0, testArray.byteOffset(7, 0));
    TEST_ASSERT_EQUAL(1, testArray.byteOffset(0, 1));
    TEST_ASSERT_EQUAL(1, testArray.byteOffset(7, 1));
    TEST_ASSERT_EQUAL(7, testArray.byteOffset(7, 7));

}

void test_bitMask() {
    bitArray<8> testArray;
    TEST_ASSERT_EQUAL(0b10000000, testArray.bitMask(0, 0));
    TEST_ASSERT_EQUAL(0b00000001, testArray.bitMask(7, 0));
    TEST_ASSERT_EQUAL(0b10000000, testArray.bitMask(0, 1));
    TEST_ASSERT_EQUAL(0b00000001, testArray.bitMask(7, 1));
    TEST_ASSERT_EQUAL(0b00000001, testArray.bitMask(7, 7));

}

void test_set_get() {
    bitArray<8> testArray;
    testArray.setBit(0, 0);
    TEST_ASSERT_TRUE(testArray.getBit(0, 0));
    testArray.clearBit(0, 0);
    TEST_ASSERT_FALSE(testArray.getBit(0, 0));

    testArray.setBit(3, 3);
    TEST_ASSERT_TRUE(testArray.getBit(3, 3));
    testArray.clearBit(3, 3);
    TEST_ASSERT_FALSE(testArray.getBit(3, 3));

    testArray.setBit(5, 5);
    testArray.invertBit(5, 5);
    TEST_ASSERT_FALSE(testArray.getBit(5, 5));
    testArray.invertBit(5, 5);
    TEST_ASSERT_TRUE(testArray.getBit(5, 5));
}

void test_clearAll() {
    bitArray<8> testArray;
    testArray.setBit(0, 0);
    testArray.setBit(3, 3);
    testArray.setBit(5, 5);
    testArray.clearAllBits();
    for (uint32_t byteIndex = 0; byteIndex < testArray.sizeInBytes; byteIndex++) {
        TEST_ASSERT_EQUAL(0, testArray.data[byteIndex]);
    }

}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_Sizes);
    RUN_TEST(test_initialize);
    RUN_TEST(test_Offset);
    RUN_TEST(test_bitMask);
    RUN_TEST(test_set_get);
    RUN_TEST(test_clearAll);
    UNITY_END();
}