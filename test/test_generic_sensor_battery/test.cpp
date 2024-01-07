#include <unity.h>
#include <settingscollection.hpp>
#include <battery.hpp>

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_initilization() {
    TEST_ASSERT_EQUAL(0, settingsCollection::read<uint8_t>(settingsCollection::settingIndex::batteryVersion));
    battery::initalize();
    TEST_ASSERT_EQUAL(batteryType::liFePO4_700mAh, battery::type);
    batteryType testType = batteryType::saft;
    settingsCollection::save<batteryType>(settingsCollection::settingIndex::batteryVersion, testType);
    battery::initalize();
    TEST_ASSERT_EQUAL(batteryType::saft, battery::type);
}

void test_needsSampling() {
    battery::channels[battery::voltage].set(0, 0, 0, 0);
    TEST_ASSERT_FALSE(battery::anyChannelNeedsSampling());
    battery::channels[battery::voltage].set(1, 1, 1, 1);
    TEST_ASSERT_TRUE(battery::anyChannelNeedsSampling());
    battery::channels[battery::percentCharged].set(1, 1, 1, 1);
    TEST_ASSERT_TRUE(battery::anyChannelNeedsSampling());
}

void test_tick() {
    battery::initalize();
    battery::channels[battery::voltage].set(0, 0, 0, 0);
    battery::channels[battery::percentCharged].set(0, 0, 0, 0);

    TEST_ASSERT_EQUAL(sensorDeviceState::sleeping, battery::state);
    battery::tick();
    TEST_ASSERT_EQUAL(sensorDeviceState::sleeping, battery::state);

    battery::channels[battery::voltage].set(1, 1, 1, 1);
    battery::tick();
    TEST_ASSERT_EQUAL(sensorDeviceState::sampling, battery::state);
}

void test_run() {
    battery::initalize();
    TEST_ASSERT_EQUAL(sensorDeviceState::sleeping, battery::state);
    battery::run();
    TEST_ASSERT_EQUAL(sensorDeviceState::sleeping, battery::state);

    battery::tick();
    TEST_ASSERT_EQUAL(sensorDeviceState::sampling, battery::state);
    TEST_ASSERT_TRUE(battery::samplingIsReady());
    battery::run();
    TEST_ASSERT_EQUAL(sensorDeviceState::sleeping, battery::state);
}

void test_measurements() {
    //TEST_MESSAGE("hello");
    // TEST_ASSERT_TRUE(battery::isPresent());
    // battery::initalize();
    // battery::sample();

    // TEST_ASSERT_EQUAL_FLOAT(3.2F, battery::getVoltage());
    // TEST_ASSERT_EQUAL_FLOAT(128.0F, battery::getChargeLevel());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initilization);
    RUN_TEST(test_needsSampling);
    RUN_TEST(test_tick);
    RUN_TEST(test_run);
    RUN_TEST(test_measurements);
    UNITY_END();
}
