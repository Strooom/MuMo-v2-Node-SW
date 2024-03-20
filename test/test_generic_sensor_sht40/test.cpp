#include <unity.h>
//#include <sht40.hpp>
#include <cstring>



void setUp(void) {
}

void tearDown(void) {}        // after test

void test_isPresent() {
    TEST_ASSERT_TRUE(sht40::isPresent());
}

void test_initialize() {
    TEST_ASSERT_EQUAL(sensorDeviceState::unknown, sht40::state);
    sht40::initialize();
    TEST_ASSERT_EQUAL(sensorDeviceState::sleeping, sht40::state);
}

// void test_sample() {
//     sht40::sample();
//     TEST_ASSERT_EQUAL_UINT32(0, sht40::rawDataTemperature);
//     TEST_ASSERT_EQUAL_UINT32(0, sht40::rawDataRelativeHumidity);
//     TEST_ASSERT_EQUAL_UINT32(0, sht40::rawDataBarometricPressure);
// }

// void test_measurements() {
//     sht40::sample();
//     TEST_IGNORE_MESSAGE("TODO: test calculating measurements from raw data");
//     TEST_ASSERT_EQUAL_FLOAT(0.0F, sht40::getTemperature());
//     TEST_ASSERT_EQUAL_FLOAT(0.0F, sht40::getRelativeHumidity());
//     TEST_ASSERT_EQUAL_FLOAT(0.0F, sht40::getBarometricPressure());
// }

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_isPresent);
    RUN_TEST(test_initialize);
    // RUN_TEST(test_sample);
    // RUN_TEST(test_measurements);
    UNITY_END();
}
