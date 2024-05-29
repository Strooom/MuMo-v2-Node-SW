#include <unity.h>
#include <nvs.hpp>
#include <measurementcollection.hpp>
#include <ctime>

void setUp(void) {}
void tearDown(void) {}

extern uint8_t mockEepromMemory[nonVolatileStorage::totalSize];

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
        float asUint32;
        uint8_t asBytes[4];
    } now;
    now.asUint32 = time(nullptr);

    union {
        float asFloat;
        uint8_t asBytes[4];
    } value;

    value.asFloat = 1.0;

    measurementCollection::addMeasurement(0, 0, value.asFloat);
    TEST_ASSERT_EQUAL(9, measurementCollection::newMeasurements.getLevel());
    TEST_ASSERT_EQUAL(128, measurementCollection::newMeasurements[0]);
    TEST_ASSERT_EQUAL_UINT8(now.asBytes[0], measurementCollection::newMeasurements[1]);
    TEST_ASSERT_EQUAL_UINT8(now.asBytes[1], measurementCollection::newMeasurements[2]);
    TEST_ASSERT_EQUAL_UINT8(now.asBytes[2], measurementCollection::newMeasurements[3]);
    TEST_ASSERT_EQUAL_UINT8(now.asBytes[3], measurementCollection::newMeasurements[4]);
    TEST_ASSERT_EQUAL_UINT8(value.asBytes[0], measurementCollection::newMeasurements[5]);
    TEST_ASSERT_EQUAL_UINT8(value.asBytes[1], measurementCollection::newMeasurements[6]);
    TEST_ASSERT_EQUAL_UINT8(value.asBytes[2], measurementCollection::newMeasurements[7]);
    TEST_ASSERT_EQUAL_UINT8(value.asBytes[3], measurementCollection::newMeasurements[8]);

    measurementCollection::addMeasurement(1, 1, 2.0);
    TEST_ASSERT_EQUAL(18, measurementCollection::newMeasurements.getLevel());
    TEST_ASSERT_EQUAL(133, measurementCollection::newMeasurements[9]);
}


void test_save() {
    measurementCollection::erase();
    measurementCollection::initialize();
    measurementCollection::findStartEndOffsets();

    union {
        float asUint32;
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
    measurementCollection::saveNewMeasurementsToEeprom();

    TEST_ASSERT_EQUAL(128, mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 0]);
    TEST_ASSERT_EQUAL_UINT8(now.asBytes[0],  mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 1]);
    TEST_ASSERT_EQUAL_UINT8(now.asBytes[1],  mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 2]);
    TEST_ASSERT_EQUAL_UINT8(now.asBytes[2],  mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 3]);
    TEST_ASSERT_EQUAL_UINT8(now.asBytes[3],  mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 4]);

    TEST_ASSERT_EQUAL_UINT8(value.asBytes[0], mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 5]);
    TEST_ASSERT_EQUAL_UINT8(value.asBytes[1], mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 6]);
    TEST_ASSERT_EQUAL_UINT8(value.asBytes[2], mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 7]);
    TEST_ASSERT_EQUAL_UINT8(value.asBytes[3], mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 8]);

    TEST_ASSERT_EQUAL(133, mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 9]);
    TEST_ASSERT_EQUAL_UINT8(now.asBytes[0],  mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 10]);
    TEST_ASSERT_EQUAL_UINT8(now.asBytes[1],  mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 11]);
    TEST_ASSERT_EQUAL_UINT8(now.asBytes[2],  mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 12]);
    TEST_ASSERT_EQUAL_UINT8(now.asBytes[3],  mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 13]);

    TEST_ASSERT_EQUAL_UINT8(value.asBytes[0], mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 14]);
    TEST_ASSERT_EQUAL_UINT8(value.asBytes[1], mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 15]);
    TEST_ASSERT_EQUAL_UINT8(value.asBytes[2], mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 16]);
    TEST_ASSERT_EQUAL_UINT8(value.asBytes[3], mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 17]);

    TEST_ASSERT_EQUAL(18, measurementCollection::nmbrOfBytesToTransmit());
}

void test_nmbrOfBytesToTransmit() {
    measurementCollection::erase();
    measurementCollection::initialize();
    measurementCollection::findStartEndOffsets();

    union {
        float asUint32;
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
    TEST_ASSERT_EQUAL(18, measurementCollection::nmbrOfBytesToTransmit());

    static constexpr uint32_t uplinkFrameCount{123};
    measurementCollection::setTransmitted(uplinkFrameCount, measurementCollection::nmbrOfBytesToTransmit());
    TEST_ASSERT_EQUAL(1, measurementCollection::uplinkHistoryIndex);
    TEST_ASSERT_EQUAL(0, measurementCollection::nmbrOfBytesToTransmit());

    measurementCollection::initialize();
    measurementCollection::findStartEndOffsets();
    TEST_ASSERT_EQUAL(18, measurementCollection::newMeasurementsOffset);
    TEST_ASSERT_EQUAL(0, measurementCollection::oldestMeasurementOffset);
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
    UNITY_END();
}