#include <unity.h>
#include <deviceaddress.hpp>

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

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

void test_setFromWord() {
    deviceAddress testAddress;
    uint32_t testAddressAsWord{0x260B1F80};
    testAddress.setFromWord(testAddressAsWord);

    uint8_t expectedBytes[4]{0x80, 0x1F, 0x0B, 0x26};
    const char* expectedHexString = "260B1F80";
    for (int i = 0; i < 4; ++i) {
        TEST_ASSERT_EQUAL_UINT8(expectedBytes[i], testAddress.getAsByte(i));
    }
    TEST_ASSERT_EQUAL_UINT32(testAddressAsWord, testAddress.getAsWord());
    TEST_ASSERT_EQUAL_STRING(expectedHexString, testAddress.getAsHexString());
}

void test_setFromHexString() {
    deviceAddress testAddress;
    const char* testAddressAsHexString = "260B1F80";
    testAddress.setFromHexString(testAddressAsHexString);
    
    uint8_t expectedBytes[4]{0x80, 0x1F, 0x0B, 0x26};
    uint32_t expectedWord{0x260B1F80};
    for (int i = 0; i < 4; ++i) {
        TEST_ASSERT_EQUAL_UINT8(expectedBytes[i], testAddress.getAsByte(i));
    }
    TEST_ASSERT_EQUAL_UINT32(expectedWord, testAddress.getAsWord());
    TEST_ASSERT_EQUAL_STRING(testAddressAsHexString, testAddress.getAsHexString());
}

void test_setFromByteArray() {
    deviceAddress testAddress;
    uint8_t testAddressAsBytes[4]{0x80, 0x1F, 0x0B, 0x26};
    testAddress.setFromByteArray(testAddressAsBytes);
    uint32_t expectedWord{0x260B1F80};
    const char* expectedHexString = "260B1F80";

    for (int i = 0; i < 4; ++i) {
        TEST_ASSERT_EQUAL_UINT8(testAddressAsBytes[i], testAddress.getAsByte(i));
    }
    TEST_ASSERT_EQUAL_UINT32(expectedWord, testAddress.getAsWord());
    TEST_ASSERT_EQUAL_STRING(expectedHexString, testAddress.getAsHexString());
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_setFromWord);
    RUN_TEST(test_setFromHexString);
    RUN_TEST(test_setFromByteArray);
    UNITY_END();
}