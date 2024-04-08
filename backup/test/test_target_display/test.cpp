// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include "display.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_reset() {
    // put a scope on the reset pin (PA0) and monitor the 10 ms low pulse, every half second
    for (auto nmbrLoops = 0; nmbrLoops < 100; nmbrLoops++) {
        display::reset();
        HAL_Delay(500);
    }
    TEST_IGNORE_MESSAGE("results to be validated on HW pin");
}

void test_setDataOrCommand() {
    // put a scope on the C/D pin (PB14) and monitor the signal high for 100ms, low for 300ms
    for (auto nmbrLoops = 0; nmbrLoops < 100; nmbrLoops++) {
        display::setDataOrCommand(true);
        HAL_Delay(100);
        display::setDataOrCommand(false);
        HAL_Delay(300);
    }
    TEST_IGNORE_MESSAGE("results to be validated on HW pin");
}

void test_selectChip() {
    // put a scope on the CS (PB5) pin and monitor the signal high for 100ms, low for 300ms
    for (auto nmbrLoops = 0; nmbrLoops < 100; nmbrLoops++) {
        display::selectChip(true);
        HAL_Delay(100);
        display::selectChip(false);
        HAL_Delay(300);
    }
    TEST_IGNORE_MESSAGE("results to be validated on HW pin");
}

void test_isBusy() {
    // put a jumper between CS (PB5) and displayBusy (PB10) to create some kind of loopback for testing the input
    for (auto nmbrLoops = 0; nmbrLoops < 100; nmbrLoops++) {
        display::selectChip(true);
        TEST_ASSERT_FALSE(display::isBusy());
        display::selectChip(false);
        TEST_ASSERT_TRUE(display::isBusy());
    }
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_reset);
    RUN_TEST(test_setDataOrCommand);
    RUN_TEST(test_selectChip);
    RUN_TEST(test_isBusy);
    UNITY_END();
}