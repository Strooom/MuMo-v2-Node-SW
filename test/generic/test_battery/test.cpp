#include <unity.h>
#include <settingscollection.hpp>
#include <battery.hpp>

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

    testType = static_cast<batteryType>(1000);
    settingsCollection::save<batteryType>(testType, settingsCollection::settingIndex::batteryType);
    battery::initalize();
    TEST_ASSERT_EQUAL(batteryType::liFePO4_700mAh, battery::type);
}

void test_tickAndRun() {
    batteryType testType = batteryType::saft_ls_14250;
    settingsCollection::save<batteryType>(testType, settingsCollection::settingIndex::batteryType);
    battery::initalize();

    battery::channels[battery::voltage].set(0, 1, 0, 0);
    battery::channels[battery::percentCharged].set(0, 0, 0, 0);

    TEST_ASSERT_TRUE(battery::anyChannelNeedsSampling());
    TEST_ASSERT_TRUE(battery::channels[battery::voltage].needsSampling());
    TEST_ASSERT_FALSE(battery::channels[battery::percentCharged].needsSampling());
    battery::tick();
    TEST_ASSERT_EQUAL(sensorDeviceState::sampling, battery::getState());
    battery::run();
    TEST_ASSERT_EQUAL(sensorDeviceState::sleeping, battery::getState());

    battery::channels[battery::percentCharged].set(0, 1, 0, 0);
    TEST_ASSERT_TRUE(battery::channels[battery::percentCharged].needsSampling());
    battery::tick();
    battery::run();
    battery::clearNewMeasurement(battery::voltage);
    battery::clearAllNewMeasurements();
}

void test_findNewMeasurements() {
    batteryType testType = batteryType::saft_ls_14250;
    settingsCollection::save<batteryType>(testType, settingsCollection::settingIndex::batteryType);
    battery::initalize();
    battery::channels[battery::voltage].set(0, 1, 0, 0);
    battery::channels[battery::percentCharged].set(0, 0, 0, 0);
    battery::tick();
    battery::run();
    TEST_ASSERT_EQUAL(0, battery::nextNewMeasurementChannel(0));
    TEST_ASSERT_EQUAL(battery::notFound, battery::nextNewMeasurementChannel(1));

    battery::initalize();
    battery::channels[battery::voltage].set(0, 0, 0, 0);
    battery::channels[battery::percentCharged].set(0, 1, 0, 0);
    battery::tick();
    battery::run();
    TEST_ASSERT_EQUAL(1, battery::nextNewMeasurementChannel(0));
    TEST_ASSERT_EQUAL(1, battery::nextNewMeasurementChannel(1));
}

void test_toString() {
    toString(batteryType::liFePO4_700mAh);
    toString(batteryType::liFePO4_1500mAh);
    toString(batteryType::alkaline_1200mAh);
    toString(batteryType::saft_ls_14250);
    toString(batteryType::saft_ls_14500);
    TEST_IGNORE_MESSAGE("Dummy test for coverage");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialization);
    RUN_TEST(test_tickAndRun);
    RUN_TEST(test_findNewMeasurements);
    RUN_TEST(test_toString);
    UNITY_END();
}
