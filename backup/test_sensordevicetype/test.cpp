#include <unity.h>
#include <power.hpp>
#include <sensordevicetype.hpp>

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_dummy_for_coverage() {
    TEST_ASSERT_EQUAL_STRING("battery", toString(sensorDeviceType::battery));
    TEST_ASSERT_EQUAL_STRING("STM32WLE5", toString(sensorDeviceType::mcu));
    TEST_ASSERT_EQUAL_STRING("BME680", toString(sensorDeviceType::bme680));
    TEST_ASSERT_EQUAL_STRING("TSL2591", toString(sensorDeviceType::tsl2591));
    TEST_ASSERT_EQUAL_STRING("SHT40", toString(sensorDeviceType::sht40));
    TEST_ASSERT_EQUAL_STRING("LIS3DH", toString(sensorDeviceType::lis3dh));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_dummy_for_coverage);
    UNITY_END();
}
