#include <unity.h>
#include "power.hpp"
#include "sensordevicecollection.hpp"
#include <battery.hpp>
#include <bme680.hpp>
#include <tsl2591.hpp>

extern uint8_t mockBME680Registers[256];
extern uint8_t mockTSL2591Registers[256];

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
    TEST_ASSERT_EQUAL_STRING("unknown", sensorDeviceCollection::name(static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices)));
}

void test_valueAsFloat() {
    TEST_IGNORE_MESSAGE("Implement me!");
}

void test_channelDecimals() {
    TEST_ASSERT_EQUAL(2, sensorDeviceCollection::channelDecimals(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::channelDecimals(static_cast<uint32_t>(sensorDeviceType::battery), battery::percentCharged));
    TEST_ASSERT_EQUAL(1, sensorDeviceCollection::channelDecimals(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::temperature));
    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::channelDecimals(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::relativeHumidity));
    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::channelDecimals(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::barometricPressure));
    TEST_ASSERT_EQUAL(0, sensorDeviceCollection::channelDecimals(static_cast<uint32_t>(sensorDeviceType::tsl2591), tsl2591::visibleLight));
}

void test_channelName() {
    TEST_ASSERT_EQUAL_STRING("voltage", sensorDeviceCollection::channelName(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    TEST_ASSERT_EQUAL_STRING("percentCharged", sensorDeviceCollection::channelName(static_cast<uint32_t>(sensorDeviceType::battery), battery::percentCharged));
    TEST_ASSERT_EQUAL_STRING("temperature", sensorDeviceCollection::channelName(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::temperature));
    TEST_ASSERT_EQUAL_STRING("relativeHumidity", sensorDeviceCollection::channelName(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::relativeHumidity));
    TEST_ASSERT_EQUAL_STRING("barometricPressure", sensorDeviceCollection::channelName(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::barometricPressure));
    TEST_ASSERT_EQUAL_STRING("visibleLight", sensorDeviceCollection::channelName(static_cast<uint32_t>(sensorDeviceType::tsl2591), tsl2591::visibleLight));
}

void test_channelUnits() {
    TEST_ASSERT_EQUAL_STRING("V", sensorDeviceCollection::channelUnits(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage));
    TEST_ASSERT_EQUAL_STRING("%", sensorDeviceCollection::channelUnits(static_cast<uint32_t>(sensorDeviceType::battery), battery::percentCharged));
    TEST_ASSERT_EQUAL_STRING("~C", sensorDeviceCollection::channelUnits(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::temperature));
    TEST_ASSERT_EQUAL_STRING("%RH", sensorDeviceCollection::channelUnits(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::relativeHumidity));
    TEST_ASSERT_EQUAL_STRING("hPa", sensorDeviceCollection::channelUnits(static_cast<uint32_t>(sensorDeviceType::bme680), bme680::barometricPressure));
    TEST_ASSERT_EQUAL_STRING("lux", sensorDeviceCollection::channelUnits(static_cast<uint32_t>(sensorDeviceType::tsl2591), tsl2591::visibleLight));
}

void test_tickAndRun() {
    sensorDeviceCollection::discover();
    bme680::channels[bme680::temperature].set(0, 1, 0, 0);
    TEST_ASSERT_TRUE(sensorDeviceCollection::isSleeping());
    sensorDeviceCollection::tick();
    TEST_ASSERT_FALSE(sensorDeviceCollection::isSleeping());
    TEST_ASSERT_EQUAL(sensorDeviceState::sampling, bme680::state);
    sensorDeviceCollection::run();
    TEST_ASSERT_TRUE(sensorDeviceCollection::hasNewMeasurements());
    TEST_ASSERT_EQUAL(4, sensorDeviceCollection::nmbrOfNewMeasurements());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initalize);
    RUN_TEST(test_discover);
    RUN_TEST(test_name);
    RUN_TEST(test_valueAsFloat);
    RUN_TEST(test_channelDecimals);
    RUN_TEST(test_channelName);
    RUN_TEST(test_channelUnits);
    RUN_TEST(test_tickAndRun);
    UNITY_END();
}
