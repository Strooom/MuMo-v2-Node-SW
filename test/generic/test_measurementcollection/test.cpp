#include <unity.h>
#include <nvs.hpp>
#include <measurementcollection.hpp>
#include <realtimeclock.hpp>
#include <float.hpp>
#include <ctime>

void setUp(void) {}
void tearDown(void) {}

extern uint8_t mockEepromMemory[nonVolatileStorage::totalSize];
extern time_t mockRealTimeClock;

void test_addressFromOffset() {
    static constexpr uint32_t blockSize{128};
    uint32_t nmbrOfBlocks = nonVolatileStorage::measurementsSize / blockSize;
    for (uint32_t blockIndex = 0; blockIndex < nmbrOfBlocks; blockIndex++) {
        TEST_ASSERT_EQUAL(4096 + (blockIndex * 128), measurementCollection::addressFromOffset(blockIndex * blockSize));
    }
}

void test_initialize() {
    measurementCollection::initialize();
    TEST_ASSERT_EQUAL(0, measurementCollection::oldestMeasurementOffset);
    TEST_ASSERT_EQUAL(0, measurementCollection::newMeasurementsOffset);
    TEST_ASSERT_EQUAL(0, measurementCollection::uplinkHistoryIndex);
    TEST_ASSERT_EQUAL(0, measurementCollection::uplinkHistory[0].frameCount);
    TEST_ASSERT_EQUAL(0, measurementCollection::uplinkHistory[0].startOffset);
    TEST_ASSERT_EQUAL(0, measurementCollection::uplinkHistory[measurementCollection::uplinkHistoryLength - 1].frameCount);
    TEST_ASSERT_EQUAL(0, measurementCollection::uplinkHistory[measurementCollection::uplinkHistoryLength - 1].startOffset);
    TEST_ASSERT_EQUAL(0, measurementCollection::newMeasurements.getLevel());
}

void test_erase() {
    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress, 0xAA, nonVolatileStorage::measurementsSize);
    TEST_ASSERT_EACH_EQUAL_UINT8(0xAA, mockEepromMemory + nonVolatileStorage::measurementsStartAddress, nonVolatileStorage::measurementsSize);
    measurementCollection::erase();
    TEST_ASSERT_EACH_EQUAL_UINT8(0xFF, mockEepromMemory + nonVolatileStorage::measurementsStartAddress, nonVolatileStorage::measurementsSize);
}

void test_findStartEndOffsets() {
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

void test_add() {
    measurementCollection::erase();
    measurementCollection::initialize();
    measurementCollection::findStartEndOffsets();
    TEST_ASSERT_EQUAL(0, measurementCollection::newMeasurementsOffset);
    TEST_ASSERT_EQUAL(0, measurementCollection::oldestMeasurementOffset);

    union {
        uint32_t asUint32;
        uint8_t asBytes[4];
    } now;
    now.asUint32 = time(nullptr);

    union {
        float asFloat;
        uint8_t asBytes[4];
    } value;

    value.asFloat = 1.0;

    measurementCollection::addMeasurement(0, 0, value.asFloat);
    TEST_ASSERT_EQUAL(5, measurementCollection::newMeasurements.getLevel());
    TEST_ASSERT_EQUAL(0, measurementCollection::newMeasurements[0]);
    TEST_ASSERT_EQUAL_UINT8(value.asBytes[0], measurementCollection::newMeasurements[1]);
    TEST_ASSERT_EQUAL_UINT8(value.asBytes[1], measurementCollection::newMeasurements[2]);
    TEST_ASSERT_EQUAL_UINT8(value.asBytes[2], measurementCollection::newMeasurements[3]);
    TEST_ASSERT_EQUAL_UINT8(value.asBytes[3], measurementCollection::newMeasurements[4]);

    value.asFloat = 1.0;
    measurementCollection::addMeasurement(1, 1, value.asFloat);
    TEST_ASSERT_EQUAL(10, measurementCollection::newMeasurements.getLevel());
    TEST_ASSERT_EQUAL(0b00001001, measurementCollection::newMeasurements[5]);
    TEST_ASSERT_EQUAL_UINT8(value.asBytes[0], measurementCollection::newMeasurements[6]);
    TEST_ASSERT_EQUAL_UINT8(value.asBytes[1], measurementCollection::newMeasurements[7]);
    TEST_ASSERT_EQUAL_UINT8(value.asBytes[2], measurementCollection::newMeasurements[8]);
    TEST_ASSERT_EQUAL_UINT8(value.asBytes[3], measurementCollection::newMeasurements[9]);
}

void test_save() {
    measurementCollection::erase();
    measurementCollection::initialize();
    measurementCollection::addMeasurement(1, 1, 10.0F);
    measurementCollection::addMeasurement(2, 2, 20.0F);
    measurementCollection::saveNewMeasurementsToEeprom();

    TEST_ASSERT_EQUAL(2, mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 0]);
    time_t now = realTimeClock::get();

    TEST_ASSERT_EQUAL_UINT8(realTimeClock::time_tToBytes(now)[0], mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 1]);
    TEST_ASSERT_EQUAL_UINT8(realTimeClock::time_tToBytes(now)[1], mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 2]);
    TEST_ASSERT_EQUAL_UINT8(realTimeClock::time_tToBytes(now)[2], mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 3]);
    TEST_ASSERT_EQUAL_UINT8(realTimeClock::time_tToBytes(now)[3], mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 4]);

    TEST_ASSERT_EQUAL_UINT8(0b00001001, mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 5]);

    TEST_ASSERT_EQUAL_UINT8(floatToBytes(10.0F)[0], mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 6]);
    TEST_ASSERT_EQUAL_UINT8(floatToBytes(10.0F)[1], mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 7]);
    TEST_ASSERT_EQUAL_UINT8(floatToBytes(10.0F)[2], mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 8]);
    TEST_ASSERT_EQUAL_UINT8(floatToBytes(10.0F)[3], mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 9]);

    TEST_ASSERT_EQUAL_UINT8(0b00010010, mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 10]);

    TEST_ASSERT_EQUAL_UINT8(floatToBytes(20.0F)[0], mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 11]);
    TEST_ASSERT_EQUAL_UINT8(floatToBytes(20.0F)[1], mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 12]);
    TEST_ASSERT_EQUAL_UINT8(floatToBytes(20.0F)[2], mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 13]);
    TEST_ASSERT_EQUAL_UINT8(floatToBytes(20.0F)[3], mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 14]);
}

void test_nmbrOfBytesToTransmit() {
    measurementCollection::erase();
    measurementCollection::initialize();
    measurementCollection::findStartEndOffsets();

    union {
        uint32_t asUint32;
        uint8_t asBytes[4];
    } now;
    now.asUint32 = time(nullptr);

    union {
        float asFloat;
        uint8_t asBytes[4];
    } value;
    value.asFloat = 1.0;

    TEST_ASSERT_EQUAL(0, measurementCollection::nmbrOfBytesToTransmit());
    measurementCollection::addMeasurement(0, 0, value.asFloat);
    measurementCollection::addMeasurement(1, 1, value.asFloat);
    TEST_ASSERT_EQUAL(0, measurementCollection::nmbrOfBytesToTransmit());
    measurementCollection::saveNewMeasurementsToEeprom();
    TEST_ASSERT_EQUAL(15, measurementCollection::nmbrOfBytesToTransmit());

    static constexpr uint32_t uplinkFrameCount{123};
    measurementCollection::setTransmitted(uplinkFrameCount, measurementCollection::nmbrOfBytesToTransmit());
    TEST_ASSERT_EQUAL(1, measurementCollection::uplinkHistoryIndex);
    TEST_ASSERT_EQUAL(0, measurementCollection::nmbrOfBytesToTransmit());

    measurementCollection::initialize();
    measurementCollection::findStartEndOffsets();
    TEST_ASSERT_EQUAL(15, measurementCollection::newMeasurementsOffset);
    TEST_ASSERT_EQUAL(0, measurementCollection::oldestMeasurementOffset);
}

void test_dump() {
    measurementCollection::erase();
    measurementCollection::initialize();
    measurementCollection::findStartEndOffsets();
    measurementCollection::addMeasurement(0, 0, 0.0F);
    measurementCollection::addMeasurement(1, 1, 1.0F);
    measurementCollection::saveNewMeasurementsToEeprom();
    measurementCollection::dumpMeasurementGroup(0);
    measurementCollection::dumpAll();
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_addressFromOffset);
    RUN_TEST(test_initialize);
    RUN_TEST(test_erase);
    RUN_TEST(test_add);
    RUN_TEST(test_findStartEndOffsets);
    RUN_TEST(test_save);
    RUN_TEST(test_nmbrOfBytesToTransmit);
    RUN_TEST(test_dump);
    UNITY_END();
}