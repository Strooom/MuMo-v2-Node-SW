// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include <gf256.hpp>
#include <cstring>
#include <cstdio>

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_exp_table() {
    uint8_t gf256expCalculated[256];
    uint32_t number{1};

    for (unsigned index = 0; index < 255; index++) {
        gf256expCalculated[index] = static_cast<uint8_t>(number);

        number = (number << 1);

        if (number > 255) {
            number = number ^ 0x11D;
        }
    }
    gf256expCalculated[255] = 0;
    TEST_ASSERT_EQUAL_UINT8_ARRAY(gf256::gf256exp, gf256expCalculated, 256);
}

void test_log_table() {
    uint8_t gf256logCalculated[256];
    uint32_t number{1};

    for (unsigned index = 0; index < 255; index++) {
        gf256logCalculated[static_cast<uint8_t>(number)] = static_cast<uint8_t>(index);

        number = (number << 1);
        if (number > 255) {
            number = number ^ 0x11D;
        }
    }
    gf256logCalculated[0] = 0xFF;
    TEST_ASSERT_EQUAL_UINT8_ARRAY(gf256::gf256log, gf256logCalculated, 256);
}

void test_add() {
    TEST_ASSERT_EQUAL(0xAA, gf256::add(0xAA, 0x00));
    TEST_ASSERT_EQUAL(0xAA, gf256::add(0x00, 0xAA));
    TEST_ASSERT_EQUAL(0x00, gf256::add(0xAA, 0xAA));
    TEST_ASSERT_EQUAL(0xFF, gf256::add(0x55, 0xAA));
}

void test_log_exp() {
    for (unsigned index = 0; index < 256; index++) {
        uint8_t a = static_cast<uint8_t>(index);
        TEST_ASSERT_EQUAL(a, gf256::e(gf256::log(a)));
        TEST_ASSERT_EQUAL(a, gf256::log(gf256::e(a)));
    }
}

void test_inverse() {
    for (unsigned index = 1; index < 256; index++) {
        TEST_ASSERT_EQUAL(1, gf256::mul1(index, gf256::inv(index)));
    }
}

void test_multiply() {
    for (unsigned a = 0; a < 256; ++a) {
        for (unsigned b = 0; b < 256; ++b) {
            TEST_ASSERT_EQUAL(gf256::mul1(a, b), gf256::mul2(a, b));
            TEST_ASSERT_EQUAL(gf256::mul2(a, b), gf256::mul3(a, b));
        }
    }
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_exp_table);
    RUN_TEST(test_log_table);
    RUN_TEST(test_inverse);
    RUN_TEST(test_add);
    RUN_TEST(test_log_exp);
    RUN_TEST(test_multiply);
    UNITY_END();
}
