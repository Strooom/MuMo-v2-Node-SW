#include <unity.h>
#include <settingscollection.hpp>
#include <battery.hpp>

void setUp(void) {
    battery::initalize();
}
void tearDown(void) {}        // after test

void test_initilization() {
    TEST_ASSERT_EQUAL(batteryType::liFePO4_700mAh, battery::type);
    TEST_ASSERT_EQUAL(sensorDeviceState::sleeping, battery::state);

    batteryType testType = batteryType::saft_ls_14250;
    settingsCollection::save<batteryType>(testType, settingsCollection::settingIndex::batteryType);
    battery::initalize();
    TEST_ASSERT_EQUAL(batteryType::saft_ls_14250, battery::type);
}

void test_needsSampling() {
    battery::channels[battery::voltage].set(0, 0, 0, 0);
    battery::channels[battery::percentCharged].set(0, 0, 0, 0);
    TEST_ASSERT_FALSE(battery::anyChannelNeedsSampling());
    battery::channels[battery::voltage].set(1, 1, 1, 1);
    TEST_ASSERT_TRUE(battery::anyChannelNeedsSampling());
    battery::channels[battery::percentCharged].set(1, 1, 1, 1);
    TEST_ASSERT_TRUE(battery::anyChannelNeedsSampling());
    battery::channels[battery::voltage].set(0, 0, 0, 0);
    TEST_ASSERT_TRUE(battery::anyChannelNeedsSampling());
    battery::channels[battery::percentCharged].set(0, 0, 0, 0);
    TEST_ASSERT_FALSE(battery::anyChannelNeedsSampling());
}

void test_tick() {
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
