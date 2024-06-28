// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include <realtimeclock.hpp>
#include <timer.hpp>
#include <ctime>

extern time_t mockRealTimeClock;

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_initialize() {
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    UNITY_END();
}
