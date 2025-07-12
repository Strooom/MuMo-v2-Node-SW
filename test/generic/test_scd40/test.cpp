#include <unity.h>
#include <scd40.hpp>
#include <cstring>



void setUp(void) {}
void tearDown(void) {}

void test_initialize() {
    TEST_ASSERT_EQUAL(sensorDeviceState::unknown, scd40::getState());
    scd40::initialize();
    TEST_ASSERT_EQUAL(sensorDeviceState::sleeping, scd40::getState());
}

void test_calculateTemperature() {
    TEST_ASSERT_FLOAT_WITHIN(0.1F, 25.0F, scd40::calculateTemperature(0x6667));
}

void test_calculateRelativeHumidity() {
    TEST_ASSERT_FLOAT_WITHIN(0.1F, 37.0F, scd40::calculateRelativeHumidity(0x5eb9));
}
void test_calculateCO2() {
    TEST_ASSERT_FLOAT_WITHIN(0.1F, 500.0F, scd40::calculateCO2(0x01f4));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_calculateTemperature);
    RUN_TEST(test_calculateRelativeHumidity);
    RUN_TEST(test_calculateCO2);
    UNITY_END();
}
