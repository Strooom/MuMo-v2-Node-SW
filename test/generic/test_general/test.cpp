#include <unity.h>
#include <swaplittleandbigendian.hpp>

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_swapLittleBigEndian() {
    TEST_ASSERT_EQUAL_UINT32(0x12345678, swapLittleBigEndian(0x78563412));
}

void test_bytesToBigEndianWord() {
    TEST_ASSERT_EQUAL_UINT32(0x12345678, bytesToBigEndianWord(0x78, 0x56, 0x34, 0x12));
}

void test_bigEndianWordToByte0() {
    TEST_ASSERT_EQUAL_UINT8(0x78, bigEndianWordToByte0(0x12345678));
}

void test_bigEndianWordToByte1() {
    TEST_ASSERT_EQUAL_UINT8(0x56, bigEndianWordToByte1(0x12345678));
}

void test_bigEndianWordToByte2() {
    TEST_ASSERT_EQUAL_UINT8(0x34, bigEndianWordToByte2(0x12345678));
}

void test_bigEndianWordToByte3() {
    TEST_ASSERT_EQUAL_UINT8(0x12, bigEndianWordToByte3(0x12345678));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_swapLittleBigEndian);
    RUN_TEST(test_bytesToBigEndianWord);
    RUN_TEST(test_bigEndianWordToByte0);
    RUN_TEST(test_bigEndianWordToByte1);
    RUN_TEST(test_bigEndianWordToByte2);
    RUN_TEST(test_bigEndianWordToByte3);
    UNITY_END();
}