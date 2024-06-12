#include <unity.h>
#include <power.hpp>
#include <bme680.hpp>
#include <cstring>
#include <sensordevicecollection.hpp>

extern uint8_t mockBME680Registers[256];

void setUp(void) {}
void tearDown(void) {}

void test_isPresent() {
    mockBME680Registers[static_cast<uint8_t>(bme680::registers::chipId)] = bme680::chipIdValue;
    TEST_ASSERT_TRUE(bme680::isPresent());
}

void test_initialize() {
    TEST_ASSERT_EQUAL(sensorDeviceState::unknown, bme680::state);
    bme680::initialize();
    TEST_ASSERT_EQUAL(sensorDeviceState::sleeping, bme680::state);
}

void test_sampling() {
    TEST_ASSERT_EQUAL(sensorDeviceState::sleeping, bme680::getState());
    bme680::startSampling();
    TEST_ASSERT_EQUAL(sensorDeviceState::sampling, bme680::getState());
    TEST_ASSERT_TRUE(bme680::samplingIsReady());
}

void test_run() {
    bme680::initialize();
    bme680::channels[bme680::temperature].set(1, 1, 1, 1, 25.0F);
    bme680::channels[bme680::relativeHumidity].set(1, 1, 1, 1, 50.0F);
    bme680::channels[bme680::barometricPressure].set(1, 1, 1, 1, 1013.0F);
    sensorDeviceCollection::discover();
    sensorDeviceCollection::updateCounters(static_cast<uint32_t>(sensorDeviceType::bme680));
    bme680::startSampling();
    bme680::run();
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_sampling);
    RUN_TEST(test_run);
    UNITY_END();
}
