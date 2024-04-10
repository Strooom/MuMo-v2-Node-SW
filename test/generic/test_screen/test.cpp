// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include "screen.hpp"
#include "graphics.hpp"
#include "display.hpp"
#include "font.hpp"
#include <cstring>                           // strncmp, strncpy

extern uint8_t mockBME680Registers[256];
extern uint8_t mockTSL2591Registers[256];

void setUp(void) {}
void tearDown(void) {}        // after test


void test_initialize() {
    screen::initialize();
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
    RUN_TEST(test_buildBigTextString);
    RUN_TEST(test_buildSmallTextString);
    UNITY_END();
}