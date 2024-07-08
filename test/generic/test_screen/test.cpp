// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include "screen.hpp"
#include "graphics.hpp"
#include "display.hpp"
#include "font.hpp"
#include <cstring>        // strncmp, strncpy


void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_showLogo() {
    screen::setType(screenType::logo);
    screen::update();
}

void test_showConsole() {
    screen::setType(screenType::version);
    screen::setText(0, "Hello, world!");
    screen::setText(1, "Line 2");
    screen::setText(2, "Line 3");
    screen::setText(3, "Line 4");
    screen::setText(4, "Line 5");
    screen::setText(5, "Line 6");
    screen::setText(6, "Line 7");
    screen::setText(7, "Line 8");
    screen::setText(8, "Line 9");
    screen::setText(9, "Line 10");
    screen::update();
}

void test_showUid() {
    screen::setType(screenType::uid);
    screen::update();
}

void test_showMeasurements() {
    screen::setType(screenType::measurements);
    screen::update();
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_showLogo);
    RUN_TEST(test_showConsole);
    RUN_TEST(test_showUid);
    RUN_TEST(test_showMeasurements);
    UNITY_END();
}