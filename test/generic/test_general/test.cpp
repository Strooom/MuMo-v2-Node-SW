#include <unity.h>
#include <swaplittleandbigendian.hpp>
#include <strl.hpp>

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

void test_strlcpy() {
    uint32_t result;
    static constexpr uint32_t inputLength{10};
    const char* input = "0123456789";
    static constexpr uint32_t outputLength{32};
    char output[outputLength]{};
    memset(output, 0xAA, outputLength);
    result = strlcpy(output, input, 5);
    TEST_ASSERT_EQUAL(5, result);
    TEST_ASSERT_EQUAL_UINT8(0, output[4]);
    TEST_ASSERT_EQUAL_STRING("0123", output);
    TEST_ASSERT_EQUAL_UINT8(0xAA, output[5]);

    memset(output, 0xAA, outputLength);
    result = strlcpy(output, input, 16);
    TEST_ASSERT_EQUAL(10, result);
    TEST_ASSERT_EQUAL_UINT8(0, output[inputLength]);
    TEST_ASSERT_EQUAL_STRING("0123456789", output);
    TEST_ASSERT_EQUAL_UINT8(0xAA, output[inputLength + 1]);
}

void test_strlcat() {
    uint32_t result;
    static constexpr uint32_t inputLength{10};
    const char* input = "0123456789";
    static constexpr uint32_t outputLength{32};
    char output[outputLength]{};
    memset(output, 0xAA, outputLength);
    output[0] = 0;
    result    = strlcat(output, input, 5);
    TEST_ASSERT_EQUAL(5, result);
    TEST_ASSERT_EQUAL_UINT8(0, output[4]);
    TEST_ASSERT_EQUAL_STRING("0123", output);
    TEST_ASSERT_EQUAL_UINT8(0xAA, output[5]);

    result = strlcat(output, input, 5);
    TEST_ASSERT_EQUAL(9, result);
    TEST_ASSERT_EQUAL_UINT8(0, output[8]);
    TEST_ASSERT_EQUAL_STRING("01230123", output);
    TEST_ASSERT_EQUAL_UINT8(0xAA, output[9]);

    result = strlcat(output, input, 32);
    TEST_ASSERT_EQUAL(18, result);
    TEST_ASSERT_EQUAL_UINT8(0, output[18]);
    TEST_ASSERT_EQUAL_STRING("012301230123456789", output);
    TEST_ASSERT_EQUAL_UINT8(0xAA, output[19]);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_swapLittleBigEndian);
    RUN_TEST(test_bytesToBigEndianWord);
    RUN_TEST(test_bigEndianWordToByte0);
    RUN_TEST(test_bigEndianWordToByte1);
    RUN_TEST(test_bigEndianWordToByte2);
    RUN_TEST(test_bigEndianWordToByte3);
    RUN_TEST(test_strlcpy);
    RUN_TEST(test_strlcat);
    UNITY_END();
}