#include <unity.h>
#include "aeskey.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test


void test_initialize() {
    aesKey aKey;
    uint8_t expected[16]{};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, aKey.asBinary(), aesKey::binaryKeyLength);
    TEST_ASSERT_EQUAL_STRING("00000000000000000000000000000000", aKey.asASCII());
}


void test_setFromBinary() {
    aesKey aKey;
    uint8_t toBeValue[16]{0x00, 0x01, 0x02, 0x03,0x04,0x05, 0x06, 0x07,0x08,0x09, 0x0A, 0x0B,0x0C,0x0D, 0x0E, 0x0F};
    aKey.setFromBinary(toBeValue);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(toBeValue, aKey.asBinary(), aesKey::binaryKeyLength);
    TEST_ASSERT_EQUAL_STRING("000102030405060708090A0B0C0D0E0F", aKey.asASCII());
}

void test_setFromHexAscii() {
    aesKey aKey;
    char asciiValue[33] = "FFFEFDFCFBFAF9F8F7F6F5F4F3F2F1F0";
    uint8_t binaryValue[16]{0x0FF, 0xFE, 0xFD, 0xFC,0xFB,0xFA, 0xF9, 0xF8,0xF7,0xF6, 0xF5, 0xF4,0xF3,0xF2, 0xF1, 0xF0};
    aKey.setFromASCII(asciiValue);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(binaryValue, aKey.asBinary(), aesKey::binaryKeyLength);
    TEST_ASSERT_EQUAL_STRING(asciiValue, aKey.asASCII());
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_setFromBinary);
    RUN_TEST(test_setFromHexAscii);
    UNITY_END();
}