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
extern bool mockBME680Present;
extern bool mockTSL2591Present;
extern bool mockSHT40Present;

void setUp(void) {
    mockBME680Registers[static_cast<uint8_t>(bme680::registers::chipId)] = bme680::chipIdValue;
    mockTSL2591Registers[static_cast<uint8_t>(tsl2591::registers::id)]   = tsl2591::chipIdValue;
}

void tearDown(void) {        // after each test
}

void test_initalize() {
    for (auto index = 0U; index < static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices); index++) {
        TEST_ASSERT_FALSE(sensorDeviceCollection::isPresent[index]);
    }
}

void test_discover() {
    mockBME680Present  = false;
    mockTSL2591Present = false;
    mockSHT40Present   = false;
    sensorDeviceCollection::discover();
    for (auto sensorDeviceIndex = 0U; sensorDeviceIndex < static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices); sensorDeviceIndex++) {
        TEST_ASSERT_FALSE(sensorDeviceCollection::isPresent[sensorDeviceIndex]);
        TEST_ASSERT_FALSE(sensorDeviceCollection::isValid(sensorDeviceIndex));
    }
    mockBME680Present  = true;
    mockTSL2591Present = true;
    mockSHT40Present   = true;
    sensorDeviceCollection::discover();
    for (auto sensorDeviceIndex = static_cast<uint32_t>(sensorDeviceType::bme680); sensorDeviceIndex <= static_cast<uint32_t>(sensorDeviceType::sht40); sensorDeviceIndex++) {
        TEST_ASSERT_TRUE(sensorDeviceCollection::isPresent[sensorDeviceIndex]);
        TEST_ASSERT_TRUE(sensorDeviceCollection::isValid(sensorDeviceIndex));
    }
}

void test_isValidChannel() {
    mockBME680Present  = false;
    mockTSL2591Present = false;
    mockSHT40Present   = false;
    sensorDeviceCollection::discover();
    for (uint32_t channelIndex = 0; channelIndex < sensorDeviceCollection::nmbrOfChannels(static_cast<uint32_t>(sensorDeviceType::battery)); channelIndex++) {
        TEST_ASSERT_TRUE(sensorDeviceCollection::isValid(static_cast<uint32_t>(sensorDeviceType::battery), channelIndex));
    }

    mockBME680Present  = true;
    mockTSL2591Present = true;
    mockSHT40Present   = true;
    sensorDeviceCollection::discover();

    TEST_ASSERT_TRUE(sensorDeviceCollection::isValid(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::temperature));
    TEST_ASSERT_TRUE(sensorDeviceCollection::isValid(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::relativeHumidity));
    TEST_ASSERT_TRUE(sensorDeviceCollection::isValid(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::barometricPressure));
    TEST_ASSERT_FALSE(sensorDeviceCollection::isValid(static_cast<uint32_t>(sensorDeviceType::bme680), 3));

    TEST_ASSERT_TRUE(sensorDeviceCollection::isValid(static_cast<uint32_t>(sensorDeviceType::tsl2591), tsl2591::visibleLight));
    TEST_ASSERT_FALSE(sensorDeviceCollection::isValid(static_cast<uint32_t>(sensorDeviceType::tsl2591), 1));

    TEST_ASSERT_TRUE(sensorDeviceCollection::isValid(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::temperature));
    TEST_ASSERT_TRUE(sensorDeviceCollection::isValid(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::relativeHumidity));
    TEST_ASSERT_FALSE(sensorDeviceCollection::isValid(static_cast<uint32_t>(sensorDeviceType::sht40), 2));
}

void test_name() {
    //    TEST_ASSERT_EQUAL_STRING("LiFePO4 700mAh", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::battery)));

    TEST_ASSERT_EQUAL_STRING("BME680", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::bme680)));
    TEST_ASSERT_EQUAL_STRING("TSL2591", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::tsl2591)));
    TEST_ASSERT_EQUAL_STRING("SHT40", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::sht40)));
    TEST_ASSERT_EQUAL_STRING("invalid deviceIndex", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices)));
}

void test_nmbrOfChannels() {
    mockBME680Present  = false;
    mockTSL2591Present = false;
    mockSHT40Present   = false;
    sensorDeviceCollection::discover();

    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::nmbrOfChannels(static_cast<uint32_t>(sensorDeviceType::bme680)));
    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::nmbrOfChannels(static_cast<uint32_t>(sensorDeviceType::tsl2591)));
    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::nmbrOfChannels(static_cast<uint32_t>(sensorDeviceType::sht40)));
    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::nmbrOfChannels(static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices)));
    mockBME680Present  = true;
    mockTSL2591Present = true;
    mockSHT40Present   = true;
    sensorDeviceCollection::discover();
    TEST_ASSERT_EQUAL(3, sensorDeviceCollection::nmbrOfChannels(static_cast<uint32_t>(sensorDeviceType::bme680)));
    TEST_ASSERT_EQUAL(1, sensorDeviceCollection::nmbrOfChannels(static_cast<uint32_t>(sensorDeviceType::tsl2591)));
    TEST_ASSERT_EQUAL(2, sensorDeviceCollection::nmbrOfChannels(static_cast<uint32_t>(sensorDeviceType::sht40)));
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
    // TEST_ASSERT_EQUAL(2, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    // TEST_ASSERT_EQUAL(2, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::battery), battery::stateOfCharge));

    TEST_ASSERT_EQUAL(1, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::temperature));
    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::relativeHumidity));
    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::barometricPressure));

    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::tsl2591), tsl2591::visibleLight));

    TEST_ASSERT_EQUAL(1, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::temperature));
    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::relativeHumidity));

    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices), 0));
}

void test_channelName() {
    // TEST_ASSERT_EQUAL_STRING("voltage", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    // TEST_ASSERT_EQUAL_STRING("stateOfCharge", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::battery), battery::stateOfCharge));

    TEST_ASSERT_EQUAL_STRING("temperature", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::temperature));
    TEST_ASSERT_EQUAL_STRING("relativeHumidity", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::relativeHumidity));
    TEST_ASSERT_EQUAL_STRING("barometricPressure", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::barometricPressure));

    TEST_ASSERT_EQUAL_STRING("visibleLight", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::tsl2591), tsl2591::visibleLight));

    TEST_ASSERT_EQUAL_STRING("temperature", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::temperature));
    TEST_ASSERT_EQUAL_STRING("relativeHumidity", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::relativeHumidity));

    TEST_ASSERT_EQUAL_STRING("invalid index", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices), 0));
}

void test_units() {
    // TEST_ASSERT_EQUAL_STRING("V", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    // TEST_ASSERT_EQUAL_STRING("", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::battery), battery::stateOfCharge));

    TEST_ASSERT_EQUAL_STRING("~C", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::temperature));
    TEST_ASSERT_EQUAL_STRING("%RH", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::relativeHumidity));
    TEST_ASSERT_EQUAL_STRING("hPa", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::barometricPressure));

    TEST_ASSERT_EQUAL_STRING("lux", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::tsl2591), tsl2591::visibleLight));

    TEST_ASSERT_EQUAL_STRING("~C", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::temperature));
    TEST_ASSERT_EQUAL_STRING("%RH", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::relativeHumidity));

    TEST_ASSERT_EQUAL_STRING("invalid index", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices), 0));
}

void test_hasNewMeasurements() {
    mockBME680Present  = false;
    mockTSL2591Present = false;
    mockSHT40Present   = false;
    sensorDeviceCollection::discover();

    TEST_ASSERT_FALSE(sensorDeviceCollection::hasNewMeasurements());
    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::nmbrOfNewMeasurements());
    TEST_ASSERT_FALSE(sensorDeviceCollection::needsSampling());
    TEST_ASSERT_FALSE(sensorDeviceCollection::needsSampling(static_cast<uint32_t>(sensorDeviceType::battery)));

    // battery::mockBatteryVoltage = 3.2F;
    // battery::channels[battery::voltage].set(0, 1);
    // TEST_ASSERT_TRUE(sensorDeviceCollection::needsSampling());
    // TEST_ASSERT_TRUE(sensorDeviceCollection::needsSampling(static_cast<uint32_t>(sensorDeviceType::battery)));

    // sensorDeviceCollection::startSampling();
    // sensorDeviceCollection::run();
    // TEST_ASSERT_TRUE(sensorDeviceCollection::isSamplingReady());
    // TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurements());
    // TEST_ASSERT_EQUAL(1, sensorDeviceCollection::nmbrOfNewMeasurements());
    // TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurements(static_cast<uint32_t>(sensorDeviceType::battery)));
    // TEST_ASSERT_EQUAL(1, sensorDeviceCollection::nmbrOfNewMeasurements(static_cast<uint32_t>(sensorDeviceType::battery)));
    // TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurement(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));

    // TEST_ASSERT_EQUAL_FLOAT(battery::mockBatteryVoltage, sensorDeviceCollection::value(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    // sensorDeviceCollection::clearNewMeasurements();
    // TEST_ASSERT_FALSE(sensorDeviceCollection::hasNewMeasurements());
}

void test_hasNewMeasurements2() {
    // mockBME680Present  = false;
    // mockTSL2591Present = false;
    // mockSHT40Present   = false;
    // sensorDeviceCollection::discover();
    // battery::mockBatteryVoltage = 3.2F;
    // // TEST_ASSERT_FALSE(sensorDeviceCollection::channel(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage).isActive());
    // // TEST_ASSERT_FALSE(sensorDeviceCollection::channel(static_cast<uint32_t>(sensorDeviceType::battery), battery::stateOfCharge).isActive());
    // battery::channels[battery::voltage].set(1, 1);
    // TEST_ASSERT_TRUE(sensorDeviceCollection::channel(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage).isActive());
    // TEST_ASSERT_FALSE(sensorDeviceCollection::channel(static_cast<uint32_t>(sensorDeviceType::battery), battery::stateOfCharge).isActive());
    // TEST_ASSERT_EQUAL_FLOAT(battery::mockBatteryVoltage, sensorDeviceCollection::value(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    // TEST_ASSERT_TRUE(sensorDeviceCollection::needsSampling());
    // sensorDeviceCollection::startSampling();
    // sensorDeviceCollection::run();
    // TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurement(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    // TEST_ASSERT_EQUAL_FLOAT(battery::mockBatteryVoltage, sensorDeviceCollection::value(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));

    // sensorDeviceCollection::clearNewMeasurements();
    // sensorDeviceCollection::updateCounters();

    // TEST_ASSERT_TRUE(sensorDeviceCollection::needsSampling());
    // sensorDeviceCollection::startSampling();
    // sensorDeviceCollection::run();
    // TEST_ASSERT_FALSE(sensorDeviceCollection::hasNewMeasurement(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    // TEST_ASSERT_EQUAL_FLOAT(battery::mockBatteryVoltage, sensorDeviceCollection::value(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));

    // sensorDeviceCollection::clearNewMeasurements();
    // sensorDeviceCollection::updateCounters();

    // TEST_ASSERT_TRUE(sensorDeviceCollection::needsSampling());
    // sensorDeviceCollection::startSampling();
    // sensorDeviceCollection::run();
    // TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurement(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    // TEST_ASSERT_EQUAL_FLOAT(battery::mockBatteryVoltage, sensorDeviceCollection::value(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
}

void test_log() {
    sensorDeviceCollection::log();
    TEST_IGNORE_MESSAGE("For Coverage Only");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initalize);
    RUN_TEST(test_discover);
    RUN_TEST(test_isValidChannel);
    RUN_TEST(test_name);
    RUN_TEST(test_nmbrOfChannels);
    RUN_TEST(test_getChannel);
    RUN_TEST(test_channelName);
    RUN_TEST(test_decimals);
    RUN_TEST(test_units);
    RUN_TEST(test_hasNewMeasurements);
    RUN_TEST(test_hasNewMeasurements2);
    RUN_TEST(test_log);
    UNITY_END();
}
