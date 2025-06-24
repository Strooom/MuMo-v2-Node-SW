#include <unity.h>
#include "power.hpp"
#include "sensordevicecollection.hpp"
#include <battery.hpp>
#include <bme680.hpp>
#include <tsl2591.hpp>
#include <sht40.hpp>
#include <realtimeclock.hpp>

extern uint8_t mockBME680Registers[256];
extern uint8_t mockTSL2591Registers[256];

extern bool mockBME680Present;
extern bool mockTSL2591Present;
extern bool mockSHT40Present;

void setUp(void) {
    mockBME680Registers[static_cast<uint8_t>(bme680::registers::chipId)] = bme680::chipIdValue;
    mockTSL2591Registers[static_cast<uint8_t>(tsl2591::registers::id)]   = tsl2591::chipIdValue;
}

void tearDown(void) {}

void test_initialize() {
    for (auto index = 0U; index < static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices); index++) {
        TEST_ASSERT_FALSE(sensorDeviceCollection::isPresent(index));
    }
}

void test_reset() {
    for (auto index = 0U; index < static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices); index++) {
        sensorDeviceCollection::present[index] = true;
    }
    sensorDeviceCollection::reset();
    for (auto index = 0U; index < static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices); index++) {
        TEST_ASSERT_FALSE(sensorDeviceCollection::isPresent(index));
    }
}

void test_set() {
    sensorDeviceCollection::reset();
    sensorDeviceCollection::set(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage, 5U, 7U);        // as long as not present, this has no effect
    TEST_ASSERT_EQUAL(0, battery::channels[battery::voltage].getOversampling());
    TEST_ASSERT_EQUAL(0, battery::channels[battery::voltage].getPrescaler());

    sensorDeviceCollection::present[static_cast<uint32_t>(sensorDeviceType::battery)] = true;
    sensorDeviceCollection::set(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage, 5U, 7U);        // as long as not present, this has no effect
    TEST_ASSERT_EQUAL(5U, battery::channels[battery::voltage].getOversampling());
    TEST_ASSERT_EQUAL(7U, battery::channels[battery::voltage].getPrescaler());
}

void test_discover() {
    sensorDeviceCollection::reset();
    mockBME680Present  = false;
    mockTSL2591Present = false;
    mockSHT40Present   = false;
    sensorDeviceCollection::discover();
    for (auto sensorDeviceIndex = 0U; sensorDeviceIndex < static_cast<uint32_t>(sensorDeviceType::bme680); sensorDeviceIndex++) {
        TEST_ASSERT_TRUE(sensorDeviceCollection::isPresent(sensorDeviceIndex));
        TEST_ASSERT_TRUE(sensorDeviceCollection::isValid(sensorDeviceIndex));
    }

    for (auto sensorDeviceIndex = static_cast<uint32_t>(sensorDeviceType::bme680); sensorDeviceIndex < static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices); sensorDeviceIndex++) {
        TEST_ASSERT_FALSE(sensorDeviceCollection::isPresent(sensorDeviceIndex));
        TEST_ASSERT_FALSE(sensorDeviceCollection::isValid(sensorDeviceIndex));
    }

    mockBME680Present  = true;
    mockTSL2591Present = true;
    mockSHT40Present   = true;
    sensorDeviceCollection::discover();
    for (auto sensorDeviceIndex = static_cast<uint32_t>(sensorDeviceType::bme680); sensorDeviceIndex <= static_cast<uint32_t>(sensorDeviceType::sht40); sensorDeviceIndex++) {
        TEST_ASSERT_TRUE(sensorDeviceCollection::isPresent(sensorDeviceIndex));
        TEST_ASSERT_TRUE(sensorDeviceCollection::isValid(sensorDeviceIndex));
    }
}

void test_isValidChannel() {
    sensorDeviceCollection::reset();
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
    battery::setType(0);
    TEST_ASSERT_EQUAL_STRING("LiFePO4 700mAh", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::battery)));
    TEST_ASSERT_EQUAL_STRING("BME680", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::bme680)));
    TEST_ASSERT_EQUAL_STRING("TSL2591", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::tsl2591)));
    TEST_ASSERT_EQUAL_STRING("SHT40", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::sht40)));
    TEST_ASSERT_EQUAL_STRING("invalid deviceIndex", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices)));
}

void test_nmbrOfChannels() {
    sensorDeviceCollection::reset();
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

void test_channelProperties() {
    sensorDeviceCollection::reset();
    sensorDeviceCollection::discover();        // Will detect battery
    TEST_ASSERT_EQUAL(2, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    TEST_ASSERT_EQUAL(2, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::battery), battery::stateOfCharge));
    TEST_ASSERT_EQUAL_STRING("voltage", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    TEST_ASSERT_EQUAL_STRING("stateOfCharge", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::battery), battery::stateOfCharge));
    TEST_ASSERT_EQUAL_STRING("V", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    TEST_ASSERT_EQUAL_STRING("", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::battery), battery::stateOfCharge));

    mockBME680Present = true;
    sensorDeviceCollection::discover();        // Will detect battery
    TEST_ASSERT_EQUAL(1, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::temperature));
    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::relativeHumidity));
    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::barometricPressure));
    TEST_ASSERT_EQUAL_STRING("temperature", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::temperature));
    TEST_ASSERT_EQUAL_STRING("relativeHumidity", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::relativeHumidity));
    TEST_ASSERT_EQUAL_STRING("barometricPressure", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::barometricPressure));
    TEST_ASSERT_EQUAL_STRING("~C", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::temperature));
    TEST_ASSERT_EQUAL_STRING("%RH", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::relativeHumidity));
    TEST_ASSERT_EQUAL_STRING("hPa", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::barometricPressure));

    // mockTSL2591Present = true;
    // mockSHT40Present   = true;
    // TEST_ASSERT_EQUAL(0, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::tsl2591), tsl2591::visibleLight));
    // TEST_ASSERT_EQUAL(1, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::temperature));
    // TEST_ASSERT_EQUAL(0, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::relativeHumidity));
    // TEST_ASSERT_EQUAL(0, sensorDeviceCollection::decimals(static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices), 0));
}

void test_channelName() {
    TEST_ASSERT_EQUAL_STRING("visibleLight", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::tsl2591), tsl2591::visibleLight));
    TEST_ASSERT_EQUAL_STRING("temperature", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::temperature));
    TEST_ASSERT_EQUAL_STRING("relativeHumidity", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::relativeHumidity));
    TEST_ASSERT_EQUAL_STRING("invalid index", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices), 0));
}

void test_units() {
    TEST_ASSERT_EQUAL_STRING("lux", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::tsl2591), tsl2591::visibleLight));
    TEST_ASSERT_EQUAL_STRING("~C", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::temperature));
    TEST_ASSERT_EQUAL_STRING("%RH", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::sht40), sht40::relativeHumidity));
    TEST_ASSERT_EQUAL_STRING("invalid index", sensorDeviceCollection::units(static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices), 0));
}

void test_needsSampling() {
    sensorDeviceCollection::reset();
    mockBME680Present  = false;
    mockTSL2591Present = false;
    mockSHT40Present   = false;
    sensorDeviceCollection::discover();

    TEST_ASSERT_FALSE(sensorDeviceCollection::needsSampling());
    TEST_ASSERT_FALSE(sensorDeviceCollection::needsSampling(static_cast<uint32_t>(sensorDeviceType::battery)));

    battery::channels[battery::voltage].set(0, 1);
    TEST_ASSERT_TRUE(sensorDeviceCollection::needsSampling());
    TEST_ASSERT_TRUE(sensorDeviceCollection::needsSampling(static_cast<uint32_t>(sensorDeviceType::battery)));
}

void test_hasNewMeasurements() {
    sensorDeviceCollection::reset();
    mockBME680Present  = false;
    mockTSL2591Present = false;
    mockSHT40Present   = false;
    sensorDeviceCollection::discover();

    battery::mockBatteryVoltage = 3.2F;
    battery::channels[battery::voltage].set(0, 1);

    sensorDeviceCollection::startSampling();
    sensorDeviceCollection::run();
    TEST_ASSERT_TRUE(sensorDeviceCollection::isSamplingReady());
    TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurements());
    TEST_ASSERT_EQUAL(1, sensorDeviceCollection::nmbrOfNewMeasurements());
    TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurements(static_cast<uint32_t>(sensorDeviceType::battery)));
    TEST_ASSERT_EQUAL(1, sensorDeviceCollection::nmbrOfNewMeasurements(static_cast<uint32_t>(sensorDeviceType::battery)));
    TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurement(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));

    TEST_ASSERT_EQUAL_FLOAT(battery::mockBatteryVoltage, sensorDeviceCollection::value(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    // sensorDeviceCollection::clearNewMeasurements();
    //  TEST_ASSERT_FALSE(sensorDeviceCollection::hasNewMeasurements());
}

void test_bug() {
    sensorDeviceCollection::reset();
    mockBME680Present  = false;
    mockTSL2591Present = false;
    mockSHT40Present   = false;
    sensorDeviceCollection::discover();
    battery::channels[battery::voltage].set(0, 2);
    battery::channels[battery::stateOfCharge].set(0, 3);
    TEST_ASSERT_TRUE(sensorDeviceCollection::needsSampling(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    TEST_ASSERT_TRUE(sensorDeviceCollection::needsSampling(static_cast<uint32_t>(sensorDeviceType::battery), battery::stateOfCharge));
    TEST_ASSERT_TRUE(sensorDeviceCollection::needsSampling(static_cast<uint32_t>(sensorDeviceType::battery)));
    TEST_ASSERT_TRUE(sensorDeviceCollection::needsSampling());

    TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurements());
    TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurements(static_cast<uint32_t>(sensorDeviceType::battery)));
    TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurement(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurement(static_cast<uint32_t>(sensorDeviceType::battery), battery::stateOfCharge));
    TEST_ASSERT_EQUAL(2, sensorDeviceCollection::nmbrOfNewMeasurements());
    TEST_ASSERT_EQUAL(2, sensorDeviceCollection::nmbrOfNewMeasurements(static_cast<uint32_t>(sensorDeviceType::battery)));

    sensorDeviceCollection::updateCounters();

    TEST_ASSERT_FALSE(sensorDeviceCollection::needsSampling(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    TEST_ASSERT_FALSE(sensorDeviceCollection::needsSampling(static_cast<uint32_t>(sensorDeviceType::battery), battery::stateOfCharge));
    TEST_ASSERT_FALSE(sensorDeviceCollection::needsSampling(static_cast<uint32_t>(sensorDeviceType::battery)));
    TEST_ASSERT_FALSE(sensorDeviceCollection::needsSampling());

    TEST_ASSERT_FALSE(sensorDeviceCollection::hasNewMeasurements());
    TEST_ASSERT_FALSE(sensorDeviceCollection::hasNewMeasurements(static_cast<uint32_t>(sensorDeviceType::battery)));
    TEST_ASSERT_FALSE(sensorDeviceCollection::hasNewMeasurement(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    TEST_ASSERT_FALSE(sensorDeviceCollection::hasNewMeasurement(static_cast<uint32_t>(sensorDeviceType::battery), battery::stateOfCharge));
    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::nmbrOfNewMeasurements());
    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::nmbrOfNewMeasurements(static_cast<uint32_t>(sensorDeviceType::battery)));

    sensorDeviceCollection::updateCounters();

    TEST_ASSERT_TRUE(sensorDeviceCollection::needsSampling(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    TEST_ASSERT_FALSE(sensorDeviceCollection::needsSampling(static_cast<uint32_t>(sensorDeviceType::battery), battery::stateOfCharge));
    TEST_ASSERT_TRUE(sensorDeviceCollection::needsSampling(static_cast<uint32_t>(sensorDeviceType::battery)));
    TEST_ASSERT_TRUE(sensorDeviceCollection::needsSampling());

    TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurements());
    TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurements(static_cast<uint32_t>(sensorDeviceType::battery)));
    TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurement(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    TEST_ASSERT_FALSE(sensorDeviceCollection::hasNewMeasurement(static_cast<uint32_t>(sensorDeviceType::battery), battery::stateOfCharge));
    TEST_ASSERT_EQUAL(1, sensorDeviceCollection::nmbrOfNewMeasurements());
    TEST_ASSERT_EQUAL(1, sensorDeviceCollection::nmbrOfNewMeasurements(static_cast<uint32_t>(sensorDeviceType::battery)));
}

void test_bug2() {
    battery::mockBatteryVoltage      = 3.0F;
    realTimeClock::mockRealTimeClock = measurementGroup::oldestPossibleTimestamp + 400;
    sensorDeviceCollection::reset();
    mockBME680Present  = false;
    mockTSL2591Present = false;
    mockSHT40Present   = false;
    sensorDeviceCollection::discover();
    battery::channels[battery::voltage].set(0, 2);
    battery::channels[battery::stateOfCharge].set(0, 3);
    TEST_ASSERT_TRUE(sensorDeviceCollection::needsSampling(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    TEST_ASSERT_TRUE(sensorDeviceCollection::needsSampling(static_cast<uint32_t>(sensorDeviceType::battery), battery::stateOfCharge));
    TEST_ASSERT_TRUE(sensorDeviceCollection::needsSampling(static_cast<uint32_t>(sensorDeviceType::battery)));
    TEST_ASSERT_TRUE(sensorDeviceCollection::needsSampling());

    TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurements());
    TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurements(static_cast<uint32_t>(sensorDeviceType::battery)));
    TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurement(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurement(static_cast<uint32_t>(sensorDeviceType::battery), battery::stateOfCharge));
    TEST_ASSERT_EQUAL(2, sensorDeviceCollection::nmbrOfNewMeasurements());
    TEST_ASSERT_EQUAL(2, sensorDeviceCollection::nmbrOfNewMeasurements(static_cast<uint32_t>(sensorDeviceType::battery)));

    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::newMeasurements.getNumberOfMeasurements());
    sensorDeviceCollection::collectNewMeasurements();
    TEST_ASSERT_EQUAL(2, sensorDeviceCollection::newMeasurements.getNumberOfMeasurements());

    sensorDeviceCollection::updateCounters();

    TEST_ASSERT_FALSE(sensorDeviceCollection::needsSampling(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    TEST_ASSERT_FALSE(sensorDeviceCollection::needsSampling(static_cast<uint32_t>(sensorDeviceType::battery), battery::stateOfCharge));
    TEST_ASSERT_FALSE(sensorDeviceCollection::needsSampling(static_cast<uint32_t>(sensorDeviceType::battery)));
    TEST_ASSERT_FALSE(sensorDeviceCollection::needsSampling());

    TEST_ASSERT_FALSE(sensorDeviceCollection::hasNewMeasurements());
    TEST_ASSERT_FALSE(sensorDeviceCollection::hasNewMeasurements(static_cast<uint32_t>(sensorDeviceType::battery)));
    TEST_ASSERT_FALSE(sensorDeviceCollection::hasNewMeasurement(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    TEST_ASSERT_FALSE(sensorDeviceCollection::hasNewMeasurement(static_cast<uint32_t>(sensorDeviceType::battery), battery::stateOfCharge));
    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::nmbrOfNewMeasurements());
    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::nmbrOfNewMeasurements(static_cast<uint32_t>(sensorDeviceType::battery)));

    sensorDeviceCollection::collectNewMeasurements();
    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::newMeasurements.getNumberOfMeasurements());

    sensorDeviceCollection::updateCounters();

    TEST_ASSERT_TRUE(sensorDeviceCollection::needsSampling(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    TEST_ASSERT_FALSE(sensorDeviceCollection::needsSampling(static_cast<uint32_t>(sensorDeviceType::battery), battery::stateOfCharge));
    TEST_ASSERT_TRUE(sensorDeviceCollection::needsSampling(static_cast<uint32_t>(sensorDeviceType::battery)));
    TEST_ASSERT_TRUE(sensorDeviceCollection::needsSampling());

    TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurements());
    TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurements(static_cast<uint32_t>(sensorDeviceType::battery)));
    TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurement(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    TEST_ASSERT_FALSE(sensorDeviceCollection::hasNewMeasurement(static_cast<uint32_t>(sensorDeviceType::battery), battery::stateOfCharge));
    TEST_ASSERT_EQUAL(1, sensorDeviceCollection::nmbrOfNewMeasurements());
    TEST_ASSERT_EQUAL(1, sensorDeviceCollection::nmbrOfNewMeasurements(static_cast<uint32_t>(sensorDeviceType::battery)));

    sensorDeviceCollection::collectNewMeasurements();
    TEST_ASSERT_EQUAL(1, sensorDeviceCollection::newMeasurements.getNumberOfMeasurements());
    TEST_ASSERT_EQUAL(measurementGroup::oldestPossibleTimestamp + 400, sensorDeviceCollection::newMeasurements.getTimeStamp());
    TEST_ASSERT_EQUAL(3.0F, sensorDeviceCollection::newMeasurements.getValue(0));
}

void test_log() {
    sensorDeviceCollection::log();
    TEST_IGNORE_MESSAGE("For Coverage Only");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_reset);
    RUN_TEST(test_set);
    RUN_TEST(test_discover);
    RUN_TEST(test_isValidChannel);
    RUN_TEST(test_name);
    RUN_TEST(test_nmbrOfChannels);
    RUN_TEST(test_getChannel);
    RUN_TEST(test_channelProperties);
    RUN_TEST(test_needsSampling);
    RUN_TEST(test_hasNewMeasurements);
    RUN_TEST(test_bug);
    RUN_TEST(test_bug2);
    RUN_TEST(test_log);
    UNITY_END();
}
