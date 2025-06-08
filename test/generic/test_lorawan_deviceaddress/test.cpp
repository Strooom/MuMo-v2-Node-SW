#include <unity.h>
#include <deviceaddress.hpp>

void setUp(void) {}
void tearDown(void) {}

void test_initialize() {
    deviceAddress testAddress;
    uint8_t expectedBytes[4]{0};
    uint32_t expectedWord{0};
    const char* expectedHexString = "";

    for (int i = 0; i < 4; ++i) {
        TEST_ASSERT_EQUAL_UINT8(expectedBytes[i], testAddress.getAsByte(i));
    }
    TEST_ASSERT_EQUAL_UINT32(expectedWord, testAddress.getAsWord());
    TEST_ASSERT_EQUAL_STRING(expectedHexString, testAddress.getAsHexString());
}

void test_setFromByteArray() {
    deviceAddress testAddress;
    uint8_t testBytes[4]{0x11, 0x22, 0x33, 0x44};
    uint32_t expectedWord{0x44332211};
    const char* expectedHexString = "11223344";
    testAddress.setFromByteArray(testBytes);

    for (int i = 0; i < 4; ++i) {
        TEST_ASSERT_EQUAL_UINT8(testBytes[i], testAddress.getAsByte(i));
    }
    TEST_ASSERT_EQUAL_UINT32(expectedWord, testAddress.getAsWord());
    TEST_ASSERT_EQUAL_STRING(expectedHexString, testAddress.getAsHexString());
}

void test_setFromWord() {
    deviceAddress testAddress;
    uint32_t testWord{0x44332211};
    uint8_t expectedBytes[4]{0x11, 0x22, 0x33, 0x44};
    const char* expectedHexString = "11223344";
    testAddress.setFromWord(testWord);

    for (int i = 0; i < 4; ++i) {
        TEST_ASSERT_EQUAL_UINT8(expectedBytes[i], testAddress.getAsByte(i));
    }
    TEST_ASSERT_EQUAL_UINT32(testWord, testAddress.getAsWord());
    TEST_ASSERT_EQUAL_STRING(expectedHexString, testAddress.getAsHexString());
}

void test_setFromHexString() {
    deviceAddress testAddress;
    const char* testHexString = "11223344";
    uint8_t expectedBytes[4]{0x11, 0x22, 0x33, 0x44};
    uint32_t expectedWord{0x44332211};
    testAddress.setFromHexString(testHexString);

    for (int i = 0; i < 4; ++i) {
        TEST_ASSERT_EQUAL_UINT8(expectedBytes[i], testAddress.getAsByte(i));
    }
    TEST_ASSERT_EQUAL_UINT32(expectedWord, testAddress.getAsWord());
    TEST_ASSERT_EQUAL_STRING(testHexString, testAddress.getAsHexString());
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_setFromByteArray);
    RUN_TEST(test_setFromWord);
    RUN_TEST(test_setFromHexString);
    UNITY_END();
}