#include <unity.h>
#include <nvs.hpp>
#include <measurementcollection.hpp>
#include <realtimeclock.hpp>
#include <float.hpp>
#include <ctime>

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

extern uint8_t mockEepromMemory[nonVolatileStorage::totalSize];
extern time_t mockRealTimeClock;

void test_addressFromOffset() {
    static constexpr uint32_t blockSize{128};
    uint32_t nmbrOfBlocks = nonVolatileStorage::measurementsSize / blockSize;
    for (uint32_t blockIndex = 0; blockIndex < nmbrOfBlocks; blockIndex++) {
        TEST_ASSERT_EQUAL(4096 + (blockIndex * 128), measurementCollection::offsetToAddress(blockIndex * blockSize));
    }

    TEST_ASSERT_EQUAL(4096, measurementCollection::offsetToAddress(0));
    TEST_ASSERT_EQUAL(4196, measurementCollection::offsetToAddress(100));
    TEST_ASSERT_EQUAL(4096, measurementCollection::offsetToAddress(60 * 1024));        // TODO : these are all for 64K eeprom and need to be adapted for 128K eeprom
    TEST_ASSERT_EQUAL(5120, measurementCollection::offsetToAddress(61 * 1024));
    TEST_ASSERT_EQUAL(8192, measurementCollection::offsetToAddress(64 * 1024));
}

void test_findDataByteForwardAndBackward() {
    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress, 0xFF, nonVolatileStorage::measurementsSize);
    TEST_ASSERT_EQUAL(-1, measurementCollection::findDataByteForward());
    TEST_ASSERT_EQUAL(-1, measurementCollection::findDataByteBackward());

    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress, 0xFF, nonVolatileStorage::measurementsSize);
    mockEepromMemory[nonVolatileStorage::measurementsStartAddress] = 0x00;
    TEST_ASSERT_EQUAL(0, measurementCollection::findDataByteForward());
    TEST_ASSERT_EQUAL(0, measurementCollection::findDataByteBackward());

    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress, 0xFF, nonVolatileStorage::measurementsSize);
    mockEepromMemory[nonVolatileStorage::measurementsStartAddress + 123] = 0x00;
    TEST_ASSERT_EQUAL(123, measurementCollection::findDataByteForward());
    TEST_ASSERT_EQUAL(123, measurementCollection::findDataByteBackward());

    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress, 0xFF, nonVolatileStorage::measurementsSize);
    mockEepromMemory[nonVolatileStorage::measurementsStartAddress + nonVolatileStorage::measurementsSize - 1] = 0x00;
    TEST_ASSERT_EQUAL(nonVolatileStorage::measurementsSize - 1, measurementCollection::findDataByteForward());
    TEST_ASSERT_EQUAL(nonVolatileStorage::measurementsSize - 1, measurementCollection::findDataByteBackward());
}

void test_gapForwardAndBackward() {
    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress, 0x00, nonVolatileStorage::measurementsSize);
    TEST_ASSERT_EQUAL(-1, measurementCollection::findGapForward(0));
    TEST_ASSERT_EQUAL(-1, measurementCollection::findGapBackward(nonVolatileStorage::measurementsSize - 1));

    static constexpr uint32_t offset{8};
    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress, 0x00, nonVolatileStorage::measurementsSize);
    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress + offset, 0xFF, measurementCollection::measurementsGap);

    TEST_ASSERT_FALSE(measurementCollection::isGapForward(offset - 1));
    TEST_ASSERT_TRUE(measurementCollection::isGapForward(offset));
    TEST_ASSERT_FALSE(measurementCollection::isGapForward(offset + 1));
    TEST_ASSERT_FALSE(measurementCollection::isGapBackward(offset + measurementCollection::measurementsGap));
    TEST_ASSERT_TRUE(measurementCollection::isGapBackward(offset + measurementCollection::measurementsGap - 1));
    TEST_ASSERT_FALSE(measurementCollection::isGapBackward(offset + measurementCollection::measurementsGap - 2));

    TEST_ASSERT_EQUAL(offset, measurementCollection::findGapForward(0));
    TEST_ASSERT_EQUAL(offset + measurementCollection::measurementsGap - 1, measurementCollection::findGapBackward(nonVolatileStorage::measurementsSize - 1));

    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress, 0x00, nonVolatileStorage::measurementsSize);
    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress, 0xFF, measurementCollection::measurementsGap / 2);
    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress + nonVolatileStorage::measurementsSize - (measurementCollection::measurementsGap / 2), 0xFF, measurementCollection::measurementsGap / 2);

    TEST_ASSERT_FALSE(measurementCollection::isGapForward(nonVolatileStorage::measurementsSize - (measurementCollection::measurementsGap / 2) - 1));
    TEST_ASSERT_TRUE(measurementCollection::isGapForward(nonVolatileStorage::measurementsSize - (measurementCollection::measurementsGap / 2)));
    TEST_ASSERT_FALSE(measurementCollection::isGapForward(nonVolatileStorage::measurementsSize - (measurementCollection::measurementsGap / 2) + 1));

    TEST_ASSERT_FALSE(measurementCollection::isGapBackward((measurementCollection::measurementsGap / 2)));
    TEST_ASSERT_TRUE(measurementCollection::isGapBackward((measurementCollection::measurementsGap / 2) - 1));
    TEST_ASSERT_FALSE(measurementCollection::isGapBackward((measurementCollection::measurementsGap / 2) - 2));

    TEST_ASSERT_EQUAL(nonVolatileStorage::measurementsSize - (measurementCollection::measurementsGap / 2), measurementCollection::findGapForward(0));
    TEST_ASSERT_EQUAL((measurementCollection::measurementsGap / 2) - 1, measurementCollection::findGapBackward(nonVolatileStorage::measurementsSize - 1));
}

void test_findMeasurementsInEeprom() {
    // blank EEPROM section case
    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress, 0xFF, nonVolatileStorage::measurementsSize);
    measurementCollection::findMeasurementsInEeprom();
    TEST_ASSERT_EQUAL(0, measurementCollection::newMeasurementsOffset);
    TEST_ASSERT_EQUAL(0, measurementCollection::oldestMeasurementOffset);

    // Some data in the EEPROM, in different locations, to test the proper wraparound of the findEnd() function
    static constexpr uint32_t nmbrNonEmptyBytes{4};

    for (uint32_t testRun = 0; testRun <= ((measurementCollection::measurementsGap * 4) + nmbrNonEmptyBytes); testRun++) {
        memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress, 0xFF, nonVolatileStorage::measurementsSize);
        uint32_t start = testRun + nonVolatileStorage::measurementsSize - ((2 * measurementCollection::measurementsGap) + nmbrNonEmptyBytes);
        uint32_t end   = start + nmbrNonEmptyBytes;

        if ((start < nonVolatileStorage::measurementsSize) && (end > nonVolatileStorage::measurementsSize)) {
            uint32_t length1 = nonVolatileStorage::measurementsSize - start;
            memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress + start, 0x00, length1);
            end              = end % nonVolatileStorage::measurementsSize;
            uint32_t length2 = end;
            memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress, 0x00, length2);
        } else {
            if (start >= nonVolatileStorage::measurementsSize) {
                start = start % nonVolatileStorage::measurementsSize;
            }
            if (end >= nonVolatileStorage::measurementsSize) {
                end = end % nonVolatileStorage::measurementsSize;
            }
            memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress + start, 0x00, nmbrNonEmptyBytes);
        }

        measurementCollection::findMeasurementsInEeprom();
        uint32_t expectedStart = start;
        TEST_ASSERT_EQUAL(expectedStart, measurementCollection::oldestMeasurementOffset);
        uint32_t expectedEnd = end;
        TEST_ASSERT_EQUAL(expectedEnd, measurementCollection::newMeasurementsOffset);
    }

    // Opposite scenario, with all eeprom full of data and only the minimyum gap somewhere

    for (uint32_t testRun = 0; testRun <= ((measurementCollection::measurementsGap * 4) + nmbrNonEmptyBytes); testRun++) {
        memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress, 0x00, nonVolatileStorage::measurementsSize);
        uint32_t start = testRun + nonVolatileStorage::measurementsSize - ((2 * measurementCollection::measurementsGap) + measurementCollection::measurementsGap);
        uint32_t end   = start + measurementCollection::measurementsGap;

        if ((start < nonVolatileStorage::measurementsSize) && (end > nonVolatileStorage::measurementsSize)) {
            uint32_t length1 = nonVolatileStorage::measurementsSize - start;
            memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress + start, 0xFF, length1);
            end              = end % nonVolatileStorage::measurementsSize;
            uint32_t length2 = end;
            memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress, 0xFF, length2);
        } else {
            if (start >= nonVolatileStorage::measurementsSize) {
                start = start % nonVolatileStorage::measurementsSize;
            }
            if (end >= nonVolatileStorage::measurementsSize) {
                end = end % nonVolatileStorage::measurementsSize;
            }
            memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress + start, 0xFF, measurementCollection::measurementsGap);
        }

        measurementCollection::findMeasurementsInEeprom();
        uint32_t expectedStart = start;
        uint32_t expectedEnd = end;
        TEST_ASSERT_EQUAL(expectedEnd, measurementCollection::oldestMeasurementOffset);
        TEST_ASSERT_EQUAL(expectedStart, measurementCollection::newMeasurementsOffset);
    }
}

void test_initialize() {
    measurementCollection::initialize();
    TEST_ASSERT_EQUAL(0, measurementCollection::oldestMeasurementOffset);
    TEST_ASSERT_EQUAL(0, measurementCollection::newMeasurementsOffset);
    TEST_ASSERT_EQUAL(0, measurementCollection::uplinkHistoryIndex);
    TEST_ASSERT_EQUAL(0, measurementCollection::uplinkHistory[0].frameCount);
    TEST_ASSERT_EQUAL(0, measurementCollection::uplinkHistory[0].offset);
    TEST_ASSERT_EQUAL(0, measurementCollection::uplinkHistory[measurementCollection::uplinkHistoryLength - 1].frameCount);
    TEST_ASSERT_EQUAL(0, measurementCollection::uplinkHistory[measurementCollection::uplinkHistoryLength - 1].offset);
    TEST_ASSERT_EQUAL(0, measurementCollection::newMeasurements.getLevel());
}

void test_erase() {
    memset(mockEepromMemory + nonVolatileStorage::measurementsStartAddress, 0xAA, nonVolatileStorage::measurementsSize);
    TEST_ASSERT_EACH_EQUAL_UINT8(0xAA, mockEepromMemory + nonVolatileStorage::measurementsStartAddress, nonVolatileStorage::measurementsSize);
    measurementCollection::eraseAll();
    TEST_ASSERT_EACH_EQUAL_UINT8(0xFF, mockEepromMemory + nonVolatileStorage::measurementsStartAddress, nonVolatileStorage::measurementsSize);
}

void test_add() {
    measurementCollection::eraseAll();
    measurementCollection::initialize();
    // measurementCollection::findStartEndOffsets();
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
    mockRealTimeClock = 100;
    measurementCollection::eraseAll();
    measurementCollection::initialize();
    measurementCollection::addMeasurement(1, 1, 10.0F);
    measurementCollection::addMeasurement(2, 2, 20.0F);

    TEST_ASSERT_EQUAL_UINT32(0, measurementCollection::nmbrOfMeasurementBytes());
    time_t now = realTimeClock::get();
    measurementCollection::saveNewMeasurementsToEeprom();
    TEST_ASSERT_EQUAL_UINT32(15, measurementCollection::nmbrOfMeasurementBytes());
    TEST_ASSERT_EQUAL(2, measurementCollection::nmbrOfMeasurementsInGroup(0));
    TEST_ASSERT_EQUAL(now, measurementCollection::timestampOfMeasurementsGroup(0));
    TEST_ASSERT_EQUAL(0b00001001, measurementCollection::measurementHeader(0 + 5));
    TEST_ASSERT_EQUAL(1, measurementCollection::measurementDeviceIndex(0 + 5));
    TEST_ASSERT_EQUAL(1, measurementCollection::measurementChannelIndex(0 + 5));
    TEST_ASSERT_EQUAL(10.0F, measurementCollection::measurementValue(0 + 5));
    TEST_ASSERT_EQUAL(0b00010010, measurementCollection::measurementHeader(0 + 10));
    TEST_ASSERT_EQUAL(2, measurementCollection::measurementDeviceIndex(0 + 10));
    TEST_ASSERT_EQUAL(2, measurementCollection::measurementChannelIndex(0 + 10));
    TEST_ASSERT_EQUAL(20.0F, measurementCollection::measurementValue(0 + 10));
}

void test_bytesConsumed() {
    measurementCollection::eraseAll();
    measurementCollection::initialize();
    measurementCollection::addMeasurement(1, 1, 10.0F);
    measurementCollection::addMeasurement(2, 2, 20.0F);
    measurementCollection::saveNewMeasurementsToEeprom();

    TEST_ASSERT_EQUAL_UINT32(15, measurementCollection::dumpMeasurementGroup(0));
}

void test_nmbrOfBytesToTransmit() {
    measurementCollection::eraseAll();
    measurementCollection::initialize();

    TEST_ASSERT_EQUAL(0, measurementCollection::nmbrOfBytesToTransmit());
    measurementCollection::addMeasurement(1, 1, 10.0F);
    measurementCollection::addMeasurement(2, 2, 20.0F);
    TEST_ASSERT_EQUAL(0, measurementCollection::nmbrOfBytesToTransmit());
    measurementCollection::saveNewMeasurementsToEeprom();
    TEST_ASSERT_EQUAL(15, measurementCollection::nmbrOfBytesToTransmit());

    static constexpr uint32_t uplinkFrameCount{123};
    measurementCollection::setTransmitted(uplinkFrameCount, measurementCollection::nmbrOfBytesToTransmit());
    TEST_ASSERT_EQUAL(1, measurementCollection::uplinkHistoryIndex);
    TEST_ASSERT_EQUAL(0, measurementCollection::nmbrOfBytesToTransmit());
}

void test_dump() {
    measurementCollection::eraseAll();
    measurementCollection::initialize();
    // measurementCollection::findStartEndOffsets();
    measurementCollection::addMeasurement(0, 0, 0.0F);
    measurementCollection::addMeasurement(1, 1, 1.0F);
    measurementCollection::saveNewMeasurementsToEeprom();
    measurementCollection::dumpMeasurementGroup(0);
    measurementCollection::dumpAll();
    measurementCollection::dumpRaw(0, 32);        // For coverage only
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_addressFromOffset);
    RUN_TEST(test_findDataByteForwardAndBackward);
    RUN_TEST(test_gapForwardAndBackward);
    RUN_TEST(test_findMeasurementsInEeprom);
    RUN_TEST(test_initialize);
    RUN_TEST(test_erase);
    RUN_TEST(test_add);
    RUN_TEST(test_save);
    RUN_TEST(test_bytesConsumed);
    RUN_TEST(test_nmbrOfBytesToTransmit);
    RUN_TEST(test_dump);
    UNITY_END();
}