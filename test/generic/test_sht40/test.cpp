#include <unity.h>
#include <sht40.hpp>
#include <cstring>
#include <sensordevicecollection.hpp>

extern uint8_t mockSHT40Registers[6];
extern bool mockSHT40Present;

void setUp(void) {
    uint8_t testData[6]{0x80, 0x00, 0xA2, 0x80, 0x00, 0xA2};
    memcpy(mockSHT40Registers, testData, 6);
}

void tearDown(void) {        // after each test
}


void test_isPresent() {
    mockSHT40Present = false;
    TEST_ASSERT_FALSE(sht40::isPresent());
    mockSHT40Present = true;
    TEST_ASSERT_TRUE(sht40::isPresent());
}

void test_initialize() {
    TEST_ASSERT_EQUAL(sensorDeviceState::unknown, sht40::getState());
    sht40::initialize();
    TEST_ASSERT_EQUAL(sensorDeviceState::sleeping, sht40::getState());
}

void test_sampling() {
    sht40::startSampling();
    if (sht40::samplingIsReady()) {
        sht40::readSample();
    } else {
        TEST_FAIL_MESSAGE("Sampling is not ready ??");
    }
    TEST_ASSERT_EQUAL(0x8000, sht40::rawDataTemperature);
    TEST_ASSERT_EQUAL(0x8000, sht40::rawDataRelativeHumidity);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 42.50F, sht40::calculateTemperature());
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 56.50F, sht40::calculateRelativeHumidity());
}

void test_calculateTemperature() {
    sht40::rawDataTemperature = 0x6666;
    TEST_ASSERT_EQUAL(25.0F, sht40::calculateTemperature());
}

void test_calculateRelativeHumidity() {
    sht40::rawDataRelativeHumidity = 0x72B0;
    TEST_ASSERT_EQUAL(50.0F, sht40::calculateRelativeHumidity());

    sht40::rawDataRelativeHumidity = 0x0;
    TEST_ASSERT_EQUAL(0.0F, sht40::calculateRelativeHumidity());

    sht40::rawDataRelativeHumidity = 0xFFFF;
    TEST_ASSERT_EQUAL(100.0F, sht40::calculateRelativeHumidity());
}

void test_run() {
    sht40::initialize();
    sht40::channels[sht40::temperature].set(1, 1);
    sht40::channels[sht40::relativeHumidity].set(1, 1);
    sensorDeviceCollection::discover();
    sensorDeviceCollection::updateCounters(static_cast<uint32_t>(sensorDeviceType::tsl2591));
    sht40::startSampling();
    sht40::run();
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_isPresent);
    RUN_TEST(test_initialize);
    RUN_TEST(test_sampling);
    RUN_TEST(test_calculateTemperature);
    RUN_TEST(test_calculateRelativeHumidity);
    RUN_TEST(test_run);
    UNITY_END();
}
