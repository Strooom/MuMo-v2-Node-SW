#include <unity.h>
#include <power.hpp>
#include <sensordevicetype.hpp>

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_dummy_for_coverage() {
    TEST_ASSERT_EQUAL_STRING("battery", toString(sensorDeviceType::battery));
    TEST_ASSERT_EQUAL_STRING("mcu", toString(sensorDeviceType::mcu));
    TEST_ASSERT_EQUAL_STRING("bme680", toString(sensorDeviceType::bme680));
    TEST_ASSERT_EQUAL_STRING("bme688", toString(sensorDeviceType::bme688));
    TEST_ASSERT_EQUAL_STRING("tsl2591", toString(sensorDeviceType::tsl2591));
    TEST_ASSERT_EQUAL_STRING("sht40", toString(sensorDeviceType::sht40));
    TEST_ASSERT_EQUAL_STRING("lis3dh", toString(sensorDeviceType::lis3dh));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_dummy_for_coverage);
    UNITY_END();
}
