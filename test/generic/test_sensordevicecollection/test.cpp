#include <unity.h>
#include "power.hpp"
#include "sensordevicecollection.hpp"
#include <battery.hpp>
#include <bme680.hpp>
#include <tsl2591.hpp>
#include <sht40.hpp>

extern uint8_t mockBME680Registers[256];
extern uint8_t mockTSL2591Registers[256];
extern float mockBatteryVoltage;

void setUp(void) {
    mockBME680Registers[static_cast<uint8_t>(bme680::registers::chipId)] = bme680::chipIdValue;
    mockTSL2591Registers[static_cast<uint8_t>(tsl2591::registers::id)]   = tsl2591::chipIdValue;
}
void tearDown(void) {}        // after test

void test_initalize() {
    for (auto index = 0U; index < static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices); index++) {
        TEST_ASSERT_FALSE(sensorDeviceCollection::isPresent[index]);
    }
    TEST_ASSERT_EQUAL_UINT32(0U, sensorDeviceCollection::actualNumberOfDevices);
}

void test_discover() {
    sensorDeviceCollection::discover();
    TEST_ASSERT_TRUE(sensorDeviceCollection::isPresent[static_cast<uint32_t>(sensorDeviceType::battery)]);
    TEST_ASSERT_TRUE(sensorDeviceCollection::isPresent[static_cast<uint32_t>(sensorDeviceType::bme680)]);
    TEST_ASSERT_TRUE(sensorDeviceCollection::isPresent[static_cast<uint32_t>(sensorDeviceType::tsl2591)]);
    TEST_ASSERT_TRUE(sensorDeviceCollection::isPresent[static_cast<uint32_t>(sensorDeviceType::sht40)]);
    TEST_ASSERT_EQUAL_UINT32(4U, sensorDeviceCollection::actualNumberOfDevices);
}

void test_name() {
    TEST_ASSERT_EQUAL_STRING("battery", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::battery)));
    TEST_ASSERT_EQUAL_STRING("BME680", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::bme680)));
    TEST_ASSERT_EQUAL_STRING("TSL2591", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::tsl2591)));
    TEST_ASSERT_EQUAL_STRING("SHT40", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::sht40)));
    TEST_ASSERT_EQUAL_STRING("invalid deviceIndex", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices)));
}

void test_nmbrOfChannels() {
    TEST_ASSERT_EQUAL(2, sensorDeviceCollection::nmbrOfChannels(static_cast<uint32_t>(sensorDeviceType::battery)));
    TEST_ASSERT_EQUAL(3, sensorDeviceCollection::nmbrOfChannels(static_cast<uint32_t>(sensorDeviceType::bme680)));
    TEST_ASSERT_EQUAL(1, sensorDeviceCollection::nmbrOfChannels(static_cast<uint32_t>(sensorDeviceType::tsl2591)));
    TEST_ASSERT_EQUAL(2, sensorDeviceCollection::nmbrOfChannels(static_cast<uint32_t>(sensorDeviceType::sht40)));
    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::nmbrOfChannels(static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices)));
}

void test_getChannel() {
    for (uint32_t channelIndex = 0; channelIndex < sensorDeviceCollection::nmbrOfChannels(static_cast<uint32_t>(sensorDeviceType::battery)); channelIndex++) {
        TEST_ASSERT_TRUE(&battery::channels[channelIndex] == &sensorDeviceCollection::channel(static_cast<uint32_t>(sensorDeviceType::battery), channelIndex));
    }
    for (uint32_t channelIndex = 0; channelIndex < sensorDeviceCollection::nmbrOfChannels(static_cast<uint32_t>(sensorDeviceType::bme680)); channelIndex++) {
        TEST_ASSERT_TRUE(&bme680::channels[channelIndex] == &sensorDeviceCollection::channel(static_cast<uint32_t>(sensorDeviceType::bme680), channelIndex));
    }
    for (uint32_t channelIndex = 0; channelIndex < sensorDeviceCollection::nmbrOfChannels(static_cast<uint32_t>(sensorDeviceType::tsl2591)); channelIndex++) {
        TEST_ASSERT_TRUE(&tsl2591::channels[channelIndex] == &sensorDeviceCollection::channel(static_cast<uint32_t>(sensorDeviceType::tsl2591), channelIndex));
    }
    for (uint32_t channelIndex = 0; channelIndex < sensorDeviceCollection::nmbrOfChannels(static_cast<uint32_t>(sensorDeviceType::sht40)); channelIndex++) {
        TEST_ASSERT_TRUE(&sht40::channels[channelIndex] == &sensorDeviceCollection::channel(static_cast<uint32_t>(sensorDeviceType::sht40), channelIndex));
    }
    TEST_ASSERT_TRUE(&sensorDeviceCollection::dummy == &sensorDeviceCollection::channel(static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices), 0));
}

void test_decimals() {
    TEST_ASSERT_EQUAL(2, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::battery), battery::percentCharged));

    TEST_ASSERT_EQUAL(1, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::temperature));
    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::relativeHumidity));
    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::barometricPressure));

    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::tsl2591), tsl2591::visibleLight));

    TEST_ASSERT_EQUAL(1, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::temperature));
    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::relativeHumidity));

    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices), 0));
}

void test_channelName() {
    TEST_ASSERT_EQUAL_STRING("voltage", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    TEST_ASSERT_EQUAL_STRING("percentCharged", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::battery), battery::percentCharged));

    TEST_ASSERT_EQUAL_STRING("temperature", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::temperature));
    TEST_ASSERT_EQUAL_STRING("relativeHumidity", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::relativeHumidity));
    TEST_ASSERT_EQUAL_STRING("barometricPressure", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::barometricPressure));

    TEST_ASSERT_EQUAL_STRING("visibleLight", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::tsl2591), tsl2591::visibleLight));

    TEST_ASSERT_EQUAL_STRING("temperature", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::temperature));
    TEST_ASSERT_EQUAL_STRING("relativeHumidity", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::relativeHumidity));

    TEST_ASSERT_EQUAL_STRING("invalid index", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices), 0));
}

void test_units() {
    TEST_ASSERT_EQUAL_STRING("V", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    TEST_ASSERT_EQUAL_STRING("%", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::battery), battery::percentCharged));

    TEST_ASSERT_EQUAL_STRING("~C", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::temperature));
    TEST_ASSERT_EQUAL_STRING("%RH", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::relativeHumidity));
    TEST_ASSERT_EQUAL_STRING("hPa", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::barometricPressure));

    TEST_ASSERT_EQUAL_STRING("lux", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::tsl2591), tsl2591::visibleLight));

    TEST_ASSERT_EQUAL_STRING("~C", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::temperature));
    TEST_ASSERT_EQUAL_STRING("%RH", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::relativeHumidity));

    TEST_ASSERT_EQUAL_STRING("invalid index", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices), 0));
}

void test_hasNewMeasurements() {
    battery::initalize();
    bme680::initialize();
    tsl2591::initialize();
    sht40::initialize();
    TEST_ASSERT_FALSE(sensorDeviceCollection::hasNewMeasurements());
    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::nmbrOfNewMeasurements());

    battery::channels[battery::voltage].hasNewValue = true;
    TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurements());
    TEST_ASSERT_EQUAL(1, sensorDeviceCollection::nmbrOfNewMeasurements());
    sensorDeviceCollection::clearNewMeasurements(static_cast<uint32_t>(sensorDeviceType::battery));

    bme680::channels[bme680::temperature].hasNewValue = true;
    TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurements());
    TEST_ASSERT_EQUAL(1, sensorDeviceCollection::nmbrOfNewMeasurements());
    sensorDeviceCollection::clearNewMeasurements(static_cast<uint32_t>(sensorDeviceType::bme680));

    tsl2591::channels[tsl2591::visibleLight].hasNewValue = true;
    TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurements());
    TEST_ASSERT_EQUAL(1, sensorDeviceCollection::nmbrOfNewMeasurements());
    sensorDeviceCollection::clearNewMeasurements(static_cast<uint32_t>(sensorDeviceType::tsl2591));

    sht40::channels[sht40::temperature].hasNewValue = true;
    TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurements());
    TEST_ASSERT_EQUAL(1, sensorDeviceCollection::nmbrOfNewMeasurements());

    battery::channels[battery::voltage].hasNewValue      = true;
    bme680::channels[bme680::temperature].hasNewValue    = true;
    tsl2591::channels[tsl2591::visibleLight].hasNewValue = true;
    sht40::channels[sht40::temperature].hasNewValue      = true;

    TEST_ASSERT_EQUAL(4, sensorDeviceCollection::nmbrOfNewMeasurements());

    sensorDeviceCollection::clearNewMeasurements();
    TEST_ASSERT_FALSE(sensorDeviceCollection::hasNewMeasurements());
    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::nmbrOfNewMeasurements());
}

void test_validDeviceAndChannelIndex() {
    TEST_ASSERT_TRUE(sensorDeviceCollection::isValidDeviceIndex(static_cast<uint32_t>(sensorDeviceType::battery)));
    TEST_ASSERT_TRUE(sensorDeviceCollection::isValidDeviceIndex(static_cast<uint32_t>(sensorDeviceType::bme680)));
    TEST_ASSERT_TRUE(sensorDeviceCollection::isValidDeviceIndex(static_cast<uint32_t>(sensorDeviceType::tsl2591)));
    TEST_ASSERT_TRUE(sensorDeviceCollection::isValidDeviceIndex(static_cast<uint32_t>(sensorDeviceType::sht40)));
    TEST_ASSERT_FALSE(sensorDeviceCollection::isValidDeviceIndex(static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices)));

    TEST_ASSERT_TRUE(sensorDeviceCollection::isValidChannelIndex(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    TEST_ASSERT_TRUE(sensorDeviceCollection::isValidChannelIndex(static_cast<uint32_t>(sensorDeviceType::battery), battery::percentCharged));
    TEST_ASSERT_FALSE(sensorDeviceCollection::isValidChannelIndex(static_cast<uint32_t>(sensorDeviceType::battery), 2));

    TEST_ASSERT_TRUE(sensorDeviceCollection::isValidChannelIndex(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::temperature));
    TEST_ASSERT_TRUE(sensorDeviceCollection::isValidChannelIndex(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::relativeHumidity));
    TEST_ASSERT_TRUE(sensorDeviceCollection::isValidChannelIndex(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::barometricPressure));
    TEST_ASSERT_FALSE(sensorDeviceCollection::isValidChannelIndex(static_cast<uint32_t>(sensorDeviceType::bme680), 3));

    TEST_ASSERT_TRUE(sensorDeviceCollection::isValidChannelIndex(static_cast<uint32_t>(sensorDeviceType::tsl2591), tsl2591::visibleLight));
    TEST_ASSERT_FALSE(sensorDeviceCollection::isValidChannelIndex(static_cast<uint32_t>(sensorDeviceType::tsl2591), 1));

    TEST_ASSERT_TRUE(sensorDeviceCollection::isValidChannelIndex(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::temperature));
    TEST_ASSERT_TRUE(sensorDeviceCollection::isValidChannelIndex(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::relativeHumidity));
    TEST_ASSERT_FALSE(sensorDeviceCollection::isValidChannelIndex(static_cast<uint32_t>(sensorDeviceType::sht40), 2));
}

void test_tickAndRun() {
    sensorDeviceCollection::discover();
    battery::initalize();
    battery::channels[battery::voltage].set(1, 1, 1, 1, 1.0F);
    battery::channels[battery::percentCharged].set(1, 1, 1, 1, 1.0F);
    bme680::initialize();
    bme680::channels[bme680::temperature].set(1, 1, 1, 1, 1.0F);
    bme680::channels[bme680::relativeHumidity].set(1, 1, 1, 1, 1.0F);
    bme680::channels[bme680::barometricPressure].set(1, 1, 1, 1, 1.0F);
    tsl2591::initialize();
    tsl2591::channels[tsl2591::visibleLight].set(1, 1, 1, 1, 1.0F);
    sht40::initialize();
    sht40::channels[sht40::temperature].set(1, 1, 1, 1, 1.0F);
    sht40::channels[sht40::relativeHumidity].set(1, 1, 1, 1, 1.0F);

    sensorDeviceCollection::updateCounters();
    sensorDeviceCollection::startSampling();
    sensorDeviceCollection::run();
    sensorDeviceCollection::isSleeping();
}

void test_value() {
    sensorDeviceCollection::discover();
    mockBatteryVoltage = 3.2F;
    TEST_ASSERT_EQUAL_FLOAT(mockBatteryVoltage, sensorDeviceCollection::value(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
}

void test_log() {
    sensorDeviceCollection::log();
    TEST_IGNORE_MESSAGE("For Coverage Only");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initalize);
    RUN_TEST(test_discover);
    RUN_TEST(test_name);
    RUN_TEST(test_nmbrOfChannels);
    RUN_TEST(test_getChannel);
    RUN_TEST(test_value);
    RUN_TEST(test_decimals);
    RUN_TEST(test_name);
    RUN_TEST(test_units);
    RUN_TEST(test_hasNewMeasurements);
    RUN_TEST(test_validDeviceAndChannelIndex);
    RUN_TEST(test_tickAndRun);
    RUN_TEST(test_log);
    UNITY_END();
}
