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

uint8_t mockBME680Registers[256];
uint8_t mockTSL2591Registers[256];

void setUp(void) {
}

void tearDown(void) {}        // after test

void test_initialize() {
    screen::show();
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    UNITY_END();
}