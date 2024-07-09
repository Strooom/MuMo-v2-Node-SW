#include <unity.h>
#include <sensordevicetype.hpp>

void setUp(void) {        // before tests
}

void tearDown(void) {        // after each test
}

void test_toString() {
    TEST_ASSERT_EQUAL_STRING("MCU", toString(sensorDeviceType::mcu));
    TEST_ASSERT_EQUAL_STRING("Battery", toString(sensorDeviceType::battery));
    TEST_ASSERT_EQUAL_STRING("BME680", toString(sensorDeviceType::bme680));
    TEST_ASSERT_EQUAL_STRING("TSL2591", toString(sensorDeviceType::tsl2591));
    TEST_ASSERT_EQUAL_STRING("SHT40", toString(sensorDeviceType::sht40));
    TEST_ASSERT_EQUAL_STRING("LIS3DH", toString(sensorDeviceType::lis3dh));
    TEST_ASSERT_EQUAL_STRING("STHS34", toString(sensorDeviceType::sths34));
    TEST_ASSERT_EQUAL_STRING("SCD40", toString(sensorDeviceType::scd40));
    TEST_ASSERT_EQUAL_STRING("SPS30", toString(sensorDeviceType::sps30));
    TEST_ASSERT_EQUAL_STRING("XA1110", toString(sensorDeviceType::xa1110));

    TEST_ASSERT_EQUAL_STRING("unknown", toString(sensorDeviceType::nmbrOfKnownDevices));
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_toString);
    UNITY_END();
}
