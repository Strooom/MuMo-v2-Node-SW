// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <unity.h>
#include <hexascii.hpp>

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_toUpperCase() {
    TEST_ASSERT_EQUAL('A', hexAscii::toUpperCase('a'));
    TEST_ASSERT_EQUAL('Z', hexAscii::toUpperCase('z'));
    TEST_ASSERT_EQUAL('0', hexAscii::toUpperCase('0'));
    TEST_ASSERT_EQUAL('9', hexAscii::toUpperCase('9'));
}

void test_isHexCharacter() {
    TEST_ASSERT_TRUE(hexAscii::isHexCharacter('0'));
    TEST_ASSERT_TRUE(hexAscii::isHexCharacter('9'));
    TEST_ASSERT_TRUE(hexAscii::isHexCharacter('A'));
    TEST_ASSERT_TRUE(hexAscii::isHexCharacter('F'));
    TEST_ASSERT_TRUE(hexAscii::isHexCharacter('a'));
    TEST_ASSERT_TRUE(hexAscii::isHexCharacter('f'));
    TEST_ASSERT_FALSE(hexAscii::isHexCharacter('G'));
    TEST_ASSERT_FALSE(hexAscii::isHexCharacter('Z'));
    TEST_ASSERT_FALSE(hexAscii::isHexCharacter('g'));
    TEST_ASSERT_FALSE(hexAscii::isHexCharacter('z'));
}

void test_valueFromHexCharacter() {
    TEST_ASSERT_EQUAL(0x00, hexAscii::valueFromHexCharacter('0'));
    TEST_ASSERT_EQUAL(0x09, hexAscii::valueFromHexCharacter('9'));
    TEST_ASSERT_EQUAL(0x0A, hexAscii::valueFromHexCharacter('A'));
    TEST_ASSERT_EQUAL(0x0F, hexAscii::valueFromHexCharacter('F'));
    TEST_ASSERT_EQUAL(0x0A, hexAscii::valueFromHexCharacter('a'));
    TEST_ASSERT_EQUAL(0x0F, hexAscii::valueFromHexCharacter('f'));
    // Edge cases
    TEST_ASSERT_EQUAL(0x00, hexAscii::valueFromHexCharacter(' '));
}

void test_hexCharacterFromValue() {
    TEST_ASSERT_EQUAL('0', hexAscii::hexCharacterFromValue(0x00));
    TEST_ASSERT_EQUAL('9', hexAscii::hexCharacterFromValue(0x09));
    TEST_ASSERT_EQUAL('A', hexAscii::hexCharacterFromValue(0x0A));
    TEST_ASSERT_EQUAL('F', hexAscii::hexCharacterFromValue(0x0F));
    // Edge cases
    TEST_ASSERT_EQUAL('?', hexAscii::hexCharacterFromValue(0x10));
}

void test_hexStringToByteArray() {
    const char input[17] = "0123456789ABCDEF";
    uint8_t output[8];
    uint8_t expectedOutput[8] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    hexAscii::hexStringToByteArray(output, input, 16U);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedOutput, output, 8);
}

void test_byteArrayToHexString() {
    uint8_t input[8] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    char output[17];        // 1 extra for terminating zero
    const char expectedOutput[17] = "0123456789ABCDEF";
    hexAscii::byteArrayToHexString(output, input, 8);
    TEST_ASSERT_EQUAL_STRING(expectedOutput, output);
    TEST_ASSERT_EQUAL(0x00, output[16]);
}

void test_uint65ToHexString() {
    const char expectedOutput1[17] = "0123456789ABCDEF";
    const char expectedOutput2[17] = "FEDCBA9876543210";
    char output[17];
    hexAscii::uint64ToHexString(output, 0x0123456789ABCDEF);
    TEST_ASSERT_EQUAL_STRING(expectedOutput1, output);
    hexAscii::uint64ToHexString(output, 0xFEDCBA9876543210);
    TEST_ASSERT_EQUAL_STRING(expectedOutput2, output);
    TEST_ASSERT_EQUAL(0x00, output[16]);
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_toUpperCase);
    RUN_TEST(test_isHexCharacter);
    RUN_TEST(test_valueFromHexCharacter);
    RUN_TEST(test_hexCharacterFromValue);
    RUN_TEST(test_hexStringToByteArray);
    RUN_TEST(test_byteArrayToHexString);
    RUN_TEST(test_uint65ToHexString);
    UNITY_END();
}