#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <i2c.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>
#include <nvs.hpp>
#include <measurementgroupcollection.hpp>
#include <applicationevent.hpp>
#include <circularbuffer.hpp>
circularBuffer<applicationEvent, 16U> applicationEventBuffer;

static constexpr uint32_t nmbrOfEepromChipsOnHw{2};
static constexpr uint32_t nmbrOf64KBanksPerChip{1};

void setUp(void) {}
void tearDown(void) {}

void test_isPresent() {
    uint32_t expected{nmbrOfEepromChipsOnHw * nmbrOf64KBanksPerChip};
    uint32_t actual{nonVolatileStorage::detectNmbr64KBanks()};

    TEST_ASSERT_EQUAL(2U, nonVolatileStorage::nmbr64KBanks);
    TEST_ASSERT_EQUAL(expected, actual);
    TEST_ASSERT_EQUAL(2U, actual);
}

void test_totalSize() {
    TEST_ASSERT_EQUAL_UINT32(nmbrOfEepromChipsOnHw * nmbrOf64KBanksPerChip * (64 * 1024), nonVolatileStorage::totalSize());
    TEST_ASSERT_EQUAL_UINT32(131072U, nonVolatileStorage::totalSize());
}

void test_otherSize() {
    TEST_ASSERT_EQUAL_UINT32(4096U, nonVolatileStorage::settingsSize);
    TEST_ASSERT_EQUAL_UINT32(126976U, nonVolatileStorage::getMeasurementsAreaSize());
}

void test_free_size() {
    measurementGroupCollection::newMeasurementsOffset   = 0;
    measurementGroupCollection::oldestMeasurementOffset = 0;
    TEST_ASSERT_EQUAL_UINT32(126976U, measurementGroupCollection::getFreeSpace());

    measurementGroupCollection::newMeasurementsOffset   = 1000;
    measurementGroupCollection::oldestMeasurementOffset = 0;
    TEST_ASSERT_EQUAL_UINT32(125976U, measurementGroupCollection::getFreeSpace());

    measurementGroupCollection::newMeasurementsOffset   = 125976U;
    measurementGroupCollection::oldestMeasurementOffset = 0;
    TEST_ASSERT_EQUAL_UINT32(1000U, measurementGroupCollection::getFreeSpace());

    measurementGroupCollection::newMeasurementsOffset   = 125976U;
    measurementGroupCollection::oldestMeasurementOffset = 1000;
    TEST_ASSERT_EQUAL_UINT32(2000U, measurementGroupCollection::getFreeSpace());

    measurementGroupCollection::initialize();
    TEST_ASSERT_EQUAL_UINT32(0, measurementGroupCollection::getOldestMeasurementOffset());
    TEST_ASSERT_EQUAL_UINT32(0, measurementGroupCollection::getNewMeasurementsOffset());
}

void test_add() {
    measurementGroupCollection::initialize();
    measurementGroup tmpGroup;
    tmpGroup.addMeasurement(0, 0, 1.0f);
    tmpGroup.addMeasurement(0, 1, 2.0f);

    measurementGroupCollection::addNew(tmpGroup);
    TEST_ASSERT_EQUAL(16, measurementGroupCollection::getNewMeasurementsOffset());
    TEST_ASSERT_EQUAL(0, measurementGroupCollection::getOldestMeasurementOffset());
    TEST_ASSERT_EQUAL_UINT32(126960U, measurementGroupCollection::getFreeSpace());
}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(3000);
    SystemClock_Config();
    gpio::initialize();
    i2c::wakeUp();

    UNITY_BEGIN();
    RUN_TEST(test_isPresent);
    RUN_TEST(test_totalSize);
    RUN_TEST(test_otherSize);
    RUN_TEST(test_free_size);
    RUN_TEST(test_add);
    UNITY_END();
}
