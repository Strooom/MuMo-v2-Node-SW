#include <unity.h>
//#include <scd40.hpp>
#include <cstring>



void setUp(void) {
}

void tearDown(void) {}        // after test

void test_isPresent() {
//    TEST_ASSERT_TRUE(scd40::isPresent());
}

void test_signature() {
    // TEST_ASSERT_EQUAL(sensorDeviceState::unknown, scd40::state);
    // scd40::initialize();
    // TEST_ASSERT_EQUAL(sensorDeviceState::sleeping, scd40::state);
}

// void test_sample() {
//     scd40::sample();
//     TEST_ASSERT_EQUAL_UINT32(0, scd40::rawDataTemperature);
//     TEST_ASSERT_EQUAL_UINT32(0, scd40::rawDataRelativeHumidity);
//     TEST_ASSERT_EQUAL_UINT32(0, scd40::rawDataBarometricPressure);
// }

// void test_measurements() {
//     scd40::sample();
//     TEST_IGNORE_MESSAGE("TODO: test calculating measurements from raw data");
//     TEST_ASSERT_EQUAL_FLOAT(0.0F, scd40::getTemperature());
//     TEST_ASSERT_EQUAL_FLOAT(0.0F, scd40::getRelativeHumidity());
//     TEST_ASSERT_EQUAL_FLOAT(0.0F, scd40::getBarometricPressure());
// }

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_isPresent);
    RUN_TEST(test_signature);
    // RUN_TEST(test_sample);
    // RUN_TEST(test_measurements);
    UNITY_END();
}
