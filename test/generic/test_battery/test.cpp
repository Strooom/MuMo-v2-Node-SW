#include <unity.h>
#include <settingscollection.hpp>
#include <battery.hpp>
#include <sensordevicecollection.hpp>

extern uint32_t mockBatteryRawADC;
extern float mockBatteryVoltage;

void setUp(void) {}
void tearDown(void) {}

void test_initialization() {
    TEST_ASSERT_EQUAL(sensorDeviceState::unknown, battery::getState());
    TEST_ASSERT_EQUAL(batteryType::liFePO4_700mAh, battery::type);
    batteryType testType = batteryType::saft_ls_14250;
    settingsCollection::save<batteryType>(testType, settingsCollection::settingIndex::batteryType);
    battery::initalize();
    TEST_ASSERT_EQUAL(sensorDeviceState::sleeping, battery::getState());
    TEST_ASSERT_EQUAL(batteryType::saft_ls_14250, battery::type);
}

void test_sampling() {
    battery::initalize();
    TEST_ASSERT_EQUAL(sensorDeviceState::sleeping, battery::getState());
    battery::startSampling();
    TEST_ASSERT_EQUAL(sensorDeviceState::sampling, battery::getState());
    TEST_ASSERT_TRUE(battery::samplingIsReady());
    mockBatteryRawADC = 1234;
    TEST_ASSERT_EQUAL(mockBatteryRawADC, battery::readSample());
    TEST_ASSERT_EQUAL(0, battery::voltageFromRaw(0));
}

void test_run() {
    battery::initalize();
    battery::channels[battery::voltage].set(1, 1, 1, 1, 3.2F);
    battery::channels[battery::percentCharged].set(1, 1, 1, 1, 0.5F);
    sensorDeviceCollection::discover();
    sensorDeviceCollection::updateCounters(static_cast<uint32_t>(sensorDeviceType::battery));
    battery::startSampling();
    battery::run();
    sensorDeviceCollection::updateCounters(static_cast<uint32_t>(sensorDeviceType::battery));
    battery::startSampling();
    battery::run();
    sensorDeviceCollection::updateCounters(static_cast<uint32_t>(sensorDeviceType::battery));
    battery::startSampling();
    battery::run();
    sensorDeviceCollection::updateCounters(static_cast<uint32_t>(sensorDeviceType::battery));
    battery::startSampling();
    battery::run();
}

void test_toString() {
    toString(batteryType::liFePO4_700mAh);
    toString(batteryType::liFePO4_1500mAh);
    toString(batteryType::alkaline_1200mAh);
    toString(batteryType::saft_ls_14250);
    toString(batteryType::saft_ls_14500);
    toString(static_cast<batteryType>(999));
    TEST_IGNORE_MESSAGE("Dummy test for coverage");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialization);
    RUN_TEST(test_toString);
    RUN_TEST(test_sampling);
    RUN_TEST(test_run);
    UNITY_END();
}
