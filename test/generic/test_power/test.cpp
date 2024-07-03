// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include "power.hpp"

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_initialize() {
    TEST_ASSERT_FALSE(power::usbPower);
}

void test_connect_and_removal_usb() {
    power::mockUsbPower = false;
    TEST_ASSERT_FALSE(power::hasUsbPower());
    TEST_ASSERT_FALSE(power::isUsbConnected());
    TEST_ASSERT_FALSE(power::isUsbRemoved());
    TEST_ASSERT_FALSE(power::isUsbConnected());
    TEST_ASSERT_FALSE(power::isUsbRemoved());

    power::mockUsbPower = true;
    TEST_ASSERT_TRUE(power::hasUsbPower());
    TEST_ASSERT_TRUE(power::isUsbConnected());
    TEST_ASSERT_FALSE(power::isUsbRemoved());
    TEST_ASSERT_FALSE(power::isUsbConnected());

    power::mockUsbPower = false;
    TEST_ASSERT_FALSE(power::hasUsbPower());
    TEST_ASSERT_FALSE(power::isUsbConnected());
    TEST_ASSERT_TRUE(power::isUsbRemoved());
    TEST_ASSERT_FALSE(power::isUsbRemoved());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_connect_and_removal_usb);
    UNITY_END();
}
