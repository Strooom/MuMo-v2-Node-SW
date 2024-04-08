// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include <float.hpp>

uint8_t mockBME680Registers[256];
uint8_t mockTSL2591Registers[256];

void setUp(void) {}
void tearDown(void) {}

void test_integerPart() {
    TEST_ASSERT_EQUAL_INT(10, integerPart(10.4, 0));
    TEST_ASSERT_EQUAL_INT(11, integerPart(10.5, 0));
    TEST_ASSERT_EQUAL_INT(10, integerPart(10.4, 1));
    TEST_ASSERT_EQUAL_INT(10, integerPart(10.5, 1));
    TEST_ASSERT_EQUAL_INT(11, integerPart(10.95, 1));
}

void test_fractionalPart() {
    TEST_ASSERT_EQUAL_INT(0, fractionalPart(10.49, 0));
    TEST_ASSERT_EQUAL_INT(0, fractionalPart(10.51, 0));
    TEST_ASSERT_EQUAL_INT(4, fractionalPart(10.44, 1));
    TEST_ASSERT_EQUAL_INT(5, fractionalPart(10.45, 1));
    TEST_ASSERT_EQUAL_INT(44, fractionalPart(10.44, 2));
    TEST_ASSERT_EQUAL_INT(45, fractionalPart(10.45, 2));
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


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_integerPart);
    RUN_TEST(test_fractionalPart);
    RUN_TEST(test_factorFloat);
    RUN_TEST(test_factorInt);
    UNITY_END();
}