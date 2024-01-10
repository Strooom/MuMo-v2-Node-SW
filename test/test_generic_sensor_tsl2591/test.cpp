#include <unity.h>
#include "tsl2591.hpp"

uint8_t mockTSL2591Registers[256];

void setUp(void) {
    mockTSL2591Registers[static_cast<uint8_t>(tsl2591::registers::id)]      = tsl2591::chipIdValue;
}

void tearDown(void) {}        // after test

void test_isPresent() {
    TEST_ASSERT_TRUE(tsl2591::isPresent());
}

void test_initialize() {
    TEST_ASSERT_EQUAL(sensorDeviceState::unknown, tsl2591::state);
    tsl2591::initialize();
    TEST_ASSERT_EQUAL(sensorDeviceState::sleeping, tsl2591::state);
}

void test_sample() {
    tsl2591::startSampling();
    TEST_ASSERT_EQUAL_UINT32(0, tsl2591::rawChannel0);
    TEST_ASSERT_EQUAL_UINT32(0, tsl2591::rawChannel1);

}

void test_gainFactor() {
    tsl2591::setGain(tsl2591::gains::gain1x);
    TEST_ASSERT_EQUAL_FLOAT(1.0F, tsl2591::gainFactor());
    tsl2591::setGain(tsl2591::gains::gain25x);
    TEST_ASSERT_EQUAL_FLOAT(25.0F, tsl2591::gainFactor());
    tsl2591::setGain(tsl2591::gains::gain428x);
    TEST_ASSERT_EQUAL_FLOAT(428.0F, tsl2591::gainFactor());
    tsl2591::setGain(tsl2591::gains::gain9876x);
    TEST_ASSERT_EQUAL_FLOAT(9876.0F, tsl2591::gainFactor());
}

void test_integrationTimeFactor() {
    tsl2591::setIntegrationTime(tsl2591::integrationTimes::integrationTime100ms);
    TEST_ASSERT_EQUAL_FLOAT(100.0F, tsl2591::integrationTimeFactor());
    tsl2591::setIntegrationTime(tsl2591::integrationTimes::integrationTime200ms);
    TEST_ASSERT_EQUAL_FLOAT(200.0F, tsl2591::integrationTimeFactor());
    tsl2591::setIntegrationTime(tsl2591::integrationTimes::integrationTime300ms);
    TEST_ASSERT_EQUAL_FLOAT(300.0F, tsl2591::integrationTimeFactor());
    tsl2591::setIntegrationTime(tsl2591::integrationTimes::integrationTime400ms);
    TEST_ASSERT_EQUAL_FLOAT(400.0F, tsl2591::integrationTimeFactor());
    tsl2591::setIntegrationTime(tsl2591::integrationTimes::integrationTime500ms);
    TEST_ASSERT_EQUAL_FLOAT(500.0F, tsl2591::integrationTimeFactor());
    tsl2591::setIntegrationTime(tsl2591::integrationTimes::integrationTime600ms);
    TEST_ASSERT_EQUAL_FLOAT(600.0F, tsl2591::integrationTimeFactor());
}


// void test_measurements() {
//     bme680::sample();
//     TEST_ASSERT_EQUAL_FLOAT(0.0F, bme680::getTemperature());
//     TEST_ASSERT_EQUAL_FLOAT(0.0F, bme680::getRelativeHumidity());
//     TEST_ASSERT_EQUAL_FLOAT(0.0F, bme680::getBarometricPressure());
// }

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_isPresent);
    RUN_TEST(test_initialize);
    RUN_TEST(test_sample);
    RUN_TEST(test_gainFactor);
    RUN_TEST(test_integrationTimeFactor);
    //RUN_TEST(test_measurements);
    UNITY_END();
}
