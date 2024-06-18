#include <unity.h>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_toString() {
    // for test coverage only
    TEST_ASSERT_EQUAL_STRING("none", toString(applicationEvent::none));
    TEST_ASSERT_EQUAL_STRING("usbConnected", toString(applicationEvent::usbConnected));
    TEST_ASSERT_EQUAL_STRING("usbRemoved", toString(applicationEvent::usbRemoved));
    TEST_ASSERT_EQUAL_STRING("realTimeClockTick", toString(applicationEvent::realTimeClockTick));
    TEST_ASSERT_EQUAL_STRING("lowPowerTimerExpired", toString(applicationEvent::lowPowerTimerExpired));
    TEST_ASSERT_EQUAL_STRING("downlinkApplicationPayloadReceived", toString(applicationEvent::downlinkApplicationPayloadReceived));
    TEST_ASSERT_EQUAL_STRING("downlinkMACReceived", toString(applicationEvent::downlinkMacCommandReceived));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_toString);
    UNITY_END();
}