#include <unity.h>
#include "power.hpp"
#include "sensordevicecollection.hpp"
#include "bme680.hpp"
#include "tsl2591.hpp"

uint8_t mockBME680Registers[256];
uint8_t mockTSL2591Registers[256];

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
    TEST_ASSERT_EQUAL_UINT32(3U, sensorDeviceCollection::actualNumberOfDevices);
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initalize);
    RUN_TEST(test_discover);
    UNITY_END();
}
