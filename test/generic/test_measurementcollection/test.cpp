#include <unity.h>
#include <nvs.hpp>
#include <measurementcollection.hpp>

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

extern uint8_t mockEepromMemory[nonVolatileStorage::totalSize];

void test_initialize() {
    measurementCollection::initialize();
    TEST_ASSERT_EQUAL(0, measurementCollection::oldestMeasurementOffset);
    TEST_ASSERT_EQUAL(0, measurementCollection::newMeasurementsOffset);
    TEST_ASSERT_EQUAL(0, measurementCollection::uplinkMemoryIndex);
    TEST_ASSERT_EQUAL(0, measurementCollection::uplinkMemory[0].uplinkFrameCount);
    TEST_ASSERT_EQUAL(0, measurementCollection::uplinkMemory[0].startOffset);
    TEST_ASSERT_EQUAL(0, measurementCollection::uplinkMemory[measurementCollection::maxNumberOfUplinksMemory - 1].uplinkFrameCount);
    TEST_ASSERT_EQUAL(0, measurementCollection::uplinkMemory[measurementCollection::maxNumberOfUplinksMemory - 1].startOffset);
    TEST_ASSERT_EQUAL(0, measurementCollection::newMeasurements.getLevel());
}

void test_addressFromOffset() {
    TEST_ASSERT_EQUAL(nonVolatileStorage::measurementsStartAddress, measurementCollection::addressFromOffset(0));
    TEST_ASSERT_EQUAL(nonVolatileStorage::measurementsStartAddress + 123, measurementCollection::addressFromOffset(123));
    TEST_ASSERT_EQUAL(nonVolatileStorage::measurementsStartAddress + 123, measurementCollection::addressFromOffset(nonVolatileStorage::measurementsSize + 123));
}

void test_nmbrOfBytesToTransmit() {
    TEST_ASSERT_EQUAL(0, measurementCollection::nmbrOfBytesToTransmit());
    measurementCollection::newMeasurementsOffset = 16;
    TEST_ASSERT_EQUAL(16, measurementCollection::nmbrOfBytesToTransmit());
    measurementCollection::uplinkMemory[0].startOffset = 8;
    TEST_ASSERT_EQUAL(8, measurementCollection::nmbrOfBytesToTransmit());
}

void test_findStartEndOffsets() {
    nonVolatileStorage::erase();
    measurementCollection::findStartEndOffsets();
    TEST_ASSERT_EQUAL(0, measurementCollection::newMeasurementsOffset);
    TEST_ASSERT_EQUAL(0, measurementCollection::oldestMeasurementOffset);

    static constexpr uint32_t offset{123};
    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress, 0x00, nonVolatileStorage::measurementsSize);
    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress + offset, 0xFF, 8);
    measurementCollection::findStartEndOffsets();
    TEST_ASSERT_EQUAL(offset, measurementCollection::newMeasurementsOffset);
    TEST_ASSERT_EQUAL(0, measurementCollection::oldestMeasurementOffset);

    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress, 0x00, nonVolatileStorage::measurementsSize);
    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress + offset, 0xFF, 4);
    measurementCollection::findStartEndOffsets();
    TEST_ASSERT_EQUAL(offset, measurementCollection::newMeasurementsOffset);
    TEST_ASSERT_EQUAL(offset + 4, measurementCollection::oldestMeasurementOffset);

    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress, 0x00, nonVolatileStorage::measurementsSize);
    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress + offset, 0xFF, 3);
    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress + (2 * offset), 0xFF, 8);
    measurementCollection::findStartEndOffsets();
    TEST_ASSERT_EQUAL((2 * offset), measurementCollection::newMeasurementsOffset);
    TEST_ASSERT_EQUAL(0, measurementCollection::oldestMeasurementOffset);

    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress, 0x00, nonVolatileStorage::measurementsSize);
    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress + offset, 0xFF, 3);
    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress + (2 * offset), 0xFF, 4);
    measurementCollection::findStartEndOffsets();
    TEST_ASSERT_EQUAL((2 * offset), measurementCollection::newMeasurementsOffset);
    TEST_ASSERT_EQUAL((2 * offset) + 4, measurementCollection::oldestMeasurementOffset);
}

void test_save() {
    nonVolatileStorage::erase();
    measurementCollection::initialize();
    TEST_ASSERT_EQUAL(0, measurementCollection::nmbrOfBytesToTransmit());
    uint8_t data[16]{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    measurementCollection::newMeasurements.append(data, 16);
    measurementCollection::save();
    TEST_ASSERT_EQUAL(16, measurementCollection::nmbrOfBytesToTransmit());
    TEST_ASSERT_EQUAL_UINT8_ARRAY(data, mockEepromMemory + nonVolatileStorage::measurementsStartAddress, 16);
    TEST_ASSERT_EACH_EQUAL_UINT8(0xFF, mockEepromMemory + nonVolatileStorage::measurementsStartAddress + 16, 4);

    measurementCollection::newMeasurements.append(data, 8);
    measurementCollection::save();
    TEST_ASSERT_EQUAL(24, measurementCollection::nmbrOfBytesToTransmit());
    TEST_ASSERT_EQUAL_UINT8_ARRAY(data, mockEepromMemory + nonVolatileStorage::measurementsStartAddress + 16, 8);
    TEST_ASSERT_EACH_EQUAL_UINT8(0xFF, mockEepromMemory + nonVolatileStorage::measurementsStartAddress + 24, 4);
}

void test_setTransmitted() {
    static constexpr uint32_t uplinkFrameCount{123};
    uint32_t bytesToTransmit = measurementCollection::nmbrOfBytesToTransmit();
    TEST_ASSERT_EQUAL(24, bytesToTransmit);
    TEST_ASSERT_EQUAL(0, measurementCollection::uplinkMemoryIndex);
    measurementCollection::setTransmitted(uplinkFrameCount, bytesToTransmit);
    TEST_ASSERT_EQUAL(1, measurementCollection::uplinkMemoryIndex);
    TEST_ASSERT_EQUAL(0, measurementCollection::nmbrOfBytesToTransmit());
    TEST_ASSERT_EQUAL(0, measurementCollection::newMeasurements.getLevel());
    TEST_ASSERT_EQUAL(123, measurementCollection::uplinkMemory[0].uplinkFrameCount);

// TODO : test overflow of the uplinkMemoryIndex


}

void test_addMeasurement() {

}

void test_addMeasurement() {

}



int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_addressFromOffset);
    RUN_TEST(test_findStartEndOffsets);
    RUN_TEST(test_addMeasurement);
    RUN_TEST(test_save);
    RUN_TEST(test_nmbrOfBytesToTransmit);
    RUN_TEST(test_setTransmitted);
    UNITY_END();
}