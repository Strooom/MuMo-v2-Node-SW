#include <unity.h>
#include <power.hpp>
#include "applicationevent.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_dummy_for_coverage() {
    TEST_ASSERT_EQUAL_STRING("none", toString(applicationEvent::none));
    TEST_ASSERT_EQUAL_STRING("usb Connected", toString(applicationEvent::usbConnected));
    TEST_ASSERT_EQUAL_STRING("usb Removed", toString(applicationEvent::usbRemoved));
    TEST_ASSERT_EQUAL_STRING("realTimeClock Tick", toString(applicationEvent::realTimeClockTick));
    TEST_ASSERT_EQUAL_STRING("lowPower Timer Expired", toString(applicationEvent::lowPowerTimerExpired));
    TEST_ASSERT_EQUAL_STRING("downlink Application Payload Received", toString(applicationEvent::downlinkApplicationPayloadReceived));
    TEST_ASSERT_EQUAL_STRING("downlink MAC command Received", toString(applicationEvent::downlinkMacCommandReceived));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_dummy_for_coverage);
    UNITY_END();
}
