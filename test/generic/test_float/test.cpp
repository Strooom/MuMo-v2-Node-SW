// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include <float.hpp>

extern uint8_t mockBME680Registers[256];
extern uint8_t mockTSL2591Registers[256];

void setUp(void) {} 
void tearDown(void) {}


void test_integerPart() {
    TEST_ASSERT_EQUAL_INT(10, integerPart(10.4F, 0));
    TEST_ASSERT_EQUAL_INT(11, integerPart(10.5F, 0));
    TEST_ASSERT_EQUAL_INT(10, integerPart(10.4F, 1));
    TEST_ASSERT_EQUAL_INT(10, integerPart(10.5F, 1));
    TEST_ASSERT_EQUAL_INT(11, integerPart(10.95F, 1));


    TEST_ASSERT_EQUAL_INT(-10, integerPart(-10.4F, 0));
    TEST_ASSERT_EQUAL_INT(-11, integerPart(-10.5F, 0));
    TEST_ASSERT_EQUAL_INT(-10, integerPart(-10.4F, 1));
    TEST_ASSERT_EQUAL_INT(-10, integerPart(-10.5F, 1));
    TEST_ASSERT_EQUAL_INT(-11, integerPart(-10.95F, 1));
}

void test_fractionalPart() {
    TEST_ASSERT_EQUAL_INT(0, fractionalPart(10.49F, 0));
    TEST_ASSERT_EQUAL_INT(0, fractionalPart(10.51F, 0));
    TEST_ASSERT_EQUAL_INT(4, fractionalPart(10.44F, 1));
    TEST_ASSERT_EQUAL_INT(5, fractionalPart(10.45F, 1));
    TEST_ASSERT_EQUAL_INT(44, fractionalPart(10.44F, 2));
    TEST_ASSERT_EQUAL_INT(45, fractionalPart(10.45F, 2));

    TEST_ASSERT_EQUAL_INT(0, fractionalPart(-10.49F, 0));
    TEST_ASSERT_EQUAL_INT(0, fractionalPart(-10.51F, 0));
    TEST_ASSERT_EQUAL_INT(4, fractionalPart(-10.44F, 1));
    TEST_ASSERT_EQUAL_INT(5, fractionalPart(-10.45F, 1));
    TEST_ASSERT_EQUAL_INT(44, fractionalPart(-10.44F, 2));
    TEST_ASSERT_EQUAL_INT(45, fractionalPart(-10.45F, 2));
}

void test_factorFloat() {
    TEST_ASSERT_EQUAL_FLOAT(1.0F, factorFloat(0));
    TEST_ASSERT_EQUAL_FLOAT(10.0F, factorFloat(1));
    TEST_ASSERT_EQUAL_FLOAT(100.0F, factorFloat(2));
}

void test_factorInt() {
    TEST_ASSERT_EQUAL_INT(1, factorInt(0));
    TEST_ASSERT_EQUAL_INT(10, factorInt(1));
    TEST_ASSERT_EQUAL_INT(100, factorInt(2));
}


void test_floatFromBytes() {
    // https://www.h-schmidt.net/FloatConverter/IEEE754.html
    uint8_t testBytes[4]{0x00, 0x00, 0xf7, 0x42};
    TEST_ASSERT_EQUAL_FLOAT(123.5F, bytesToFloat(testBytes));

    uint8_t testBytes2[4]{0x4E, 0x61, 0x3C, 0x4B};
    TEST_ASSERT_EQUAL_FLOAT(12345678.0F, bytesToFloat(testBytes2));
}

void test_bytesFromFloat() {
    TEST_ASSERT_EQUAL_FLOAT(0x00, floatToBytes(123.5F)[0]);
    TEST_ASSERT_EQUAL_FLOAT(0x00, floatToBytes(123.5F)[1]);
    TEST_ASSERT_EQUAL_FLOAT(0xf7, floatToBytes(123.5F)[2]);
    TEST_ASSERT_EQUAL_FLOAT(0x42, floatToBytes(123.5F)[3]);

    TEST_ASSERT_EQUAL_FLOAT(0x4E, floatToBytes(12345678.0F)[0]);
    TEST_ASSERT_EQUAL_FLOAT(0x61, floatToBytes(12345678.0F)[1]);
    TEST_ASSERT_EQUAL_FLOAT(0x3C, floatToBytes(12345678.0F)[2]);
    TEST_ASSERT_EQUAL_FLOAT(0x4B, floatToBytes(12345678.0F)[3]);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_integerPart);
    RUN_TEST(test_fractionalPart);
    RUN_TEST(test_factorFloat);
    RUN_TEST(test_factorInt);
    RUN_TEST(test_floatFromBytes);
    RUN_TEST(test_bytesFromFloat);
    UNITY_END();
}