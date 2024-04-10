#include <unity.h>
#include <measurementcollection.hpp>
#include <nvs.hpp>

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

extern uint8_t mockEepromMemory[nonVolatileStorage::totalSize];

void test_initialize() {
    TEST_ASSERT_EQUAL(0, measurementCollection::writeAddress);
}


void test_findStartEndAddress() {
    nonVolatileStorage::erase();
    measurementCollection::findStartEndAddress();
    TEST_ASSERT_EQUAL(nonVolatileStorage::measurementsStartAddress, measurementCollection::endAddress);
    TEST_ASSERT_EQUAL(nonVolatileStorage::measurementsStartAddress, measurementCollection::startAddress);

    static constexpr uint32_t offset{123};
    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress, 0x00, nonVolatileStorage::measurementsSize);
    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress + offset, 0xFF, 8);
    measurementCollection::findStartEndAddress();
    TEST_ASSERT_EQUAL(nonVolatileStorage::measurementsStartAddress + offset, measurementCollection::endAddress);
    TEST_ASSERT_EQUAL(nonVolatileStorage::measurementsStartAddress, measurementCollection::startAddress);

    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress, 0x00, nonVolatileStorage::measurementsSize);
    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress + offset, 0xFF, 4);
    measurementCollection::findStartEndAddress();
    TEST_ASSERT_EQUAL(nonVolatileStorage::measurementsStartAddress + offset, measurementCollection::endAddress);
    TEST_ASSERT_EQUAL(nonVolatileStorage::measurementsStartAddress + offset + 4, measurementCollection::startAddress);

    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress, 0x00, nonVolatileStorage::measurementsSize);
    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress + offset, 0xFF, 3);
    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress + (2 * offset), 0xFF, 8);
    measurementCollection::findStartEndAddress();
    TEST_ASSERT_EQUAL(nonVolatileStorage::measurementsStartAddress + (2 * offset), measurementCollection::endAddress);
    TEST_ASSERT_EQUAL(nonVolatileStorage::measurementsStartAddress, measurementCollection::startAddress);

    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress, 0x00, nonVolatileStorage::measurementsSize);
    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress + offset, 0xFF, 3);
    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress + (2 * offset), 0xFF, 4);
    measurementCollection::findStartEndAddress();
    TEST_ASSERT_EQUAL(nonVolatileStorage::measurementsStartAddress + (2 * offset), measurementCollection::endAddress);
    TEST_ASSERT_EQUAL(nonVolatileStorage::measurementsStartAddress + (2 * offset) + 4, measurementCollection::startAddress);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_findStartEndAddress);
    UNITY_END();
}