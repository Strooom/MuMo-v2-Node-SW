#include <unity.h>
#include <power.hpp>
#include <sensordevicestate.hpp>

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_dummy_for_coverage() {
    TEST_ASSERT_EQUAL_STRING("unpowered", toString(sensorDeviceState::unpowered));
    TEST_ASSERT_EQUAL_STRING("sleeping", toString(sensorDeviceState::sleeping));
    TEST_ASSERT_EQUAL_STRING("standby", toString(sensorDeviceState::standby));
    TEST_ASSERT_EQUAL_STRING("sampling", toString(sensorDeviceState::sampling));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_dummy_for_coverage);
    UNITY_END();
}
