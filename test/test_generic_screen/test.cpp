// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include "screen.hpp"
#include "graphics.hpp"
#include "display.hpp"
#include "font.hpp"
#include <cstring>                           // strncmp, strncpy

uint8_t mockBME680Registers[256];
uint8_t mockTSL2591Registers[256];

void setUp(void) {}
void tearDown(void) {}        // after test


void test_initialize() {
    screen::initialize();
}

void test_integerPart() {
    TEST_ASSERT_EQUAL_INT(10, screen::integerPart(10.4, 0));
    TEST_ASSERT_EQUAL_INT(11, screen::integerPart(10.5, 0));
    TEST_ASSERT_EQUAL_INT(10, screen::integerPart(10.4, 1));
    TEST_ASSERT_EQUAL_INT(10, screen::integerPart(10.5, 1));
    TEST_ASSERT_EQUAL_INT(11, screen::integerPart(10.95, 1));
}

void test_fractionalPart() {
    TEST_ASSERT_EQUAL_INT(0, screen::fractionalPart(10.49, 0));
    TEST_ASSERT_EQUAL_INT(0, screen::fractionalPart(10.51, 0));
    TEST_ASSERT_EQUAL_INT(4, screen::fractionalPart(10.44, 1));
    TEST_ASSERT_EQUAL_INT(5, screen::fractionalPart(10.45, 1));
    TEST_ASSERT_EQUAL_INT(44, screen::fractionalPart(10.44, 2));
    TEST_ASSERT_EQUAL_INT(45, screen::fractionalPart(10.45, 2));
}

void test_factorFloat() {
    TEST_ASSERT_EQUAL_FLOAT(1.0F, screen::factorFloat(0));
    TEST_ASSERT_EQUAL_FLOAT(10.0F, screen::factorFloat(1));
    TEST_ASSERT_EQUAL_FLOAT(100.0F, screen::factorFloat(2));
}

void test_factorInt() {
    TEST_ASSERT_EQUAL_INT(1, screen::factorInt(0));
    TEST_ASSERT_EQUAL_INT(10, screen::factorInt(1));
    TEST_ASSERT_EQUAL_INT(100, screen::factorInt(2));
}

void test_buildBigTextString() {
    strncpy(screen::bigText[0], "", screen::maxTextLength);
    screen::buildBigTextString(10, 0);
    TEST_ASSERT_EQUAL_STRING("10", screen::bigText[0]);
}

void test_buildSmallTextString() {
    strncpy(screen::smallText[0], "", screen::maxTextLength);
    screen::buildSmallTextString(5, 1, "lux", 0);
    TEST_ASSERT_EQUAL_STRING("5 lux", screen::smallText[0]);

    strncpy(screen::smallText[1], "", screen::maxTextLength);
    screen::buildSmallTextString(0, 0, "hPa", 1);
    TEST_ASSERT_EQUAL_STRING("hPa", screen::smallText[1]);

}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_integerPart);
    RUN_TEST(test_fractionalPart);
    RUN_TEST(test_factorFloat);
    RUN_TEST(test_factorInt);
    RUN_TEST(test_buildBigTextString);
    RUN_TEST(test_buildSmallTextString);
    UNITY_END();
}