#include <unity.h>
#include <nvs.hpp>
#include <settingscollection.hpp>
#include <measurementgroupcollection.hpp>
#include <realtimeclock.hpp>

void setUp(void) {}
void tearDown(void) {}

void test_isValid() {
    measurementGroupCollection::oldestMeasurementOffset = 0xFFFFFFFF;
    measurementGroupCollection::newMeasurementsOffset   = 0xFFFFFFFF;
    TEST_ASSERT_FALSE(measurementGroupCollection::isValid());

    measurementGroupCollection::newMeasurementsOffset   = 0xFFFFFFFF;
    measurementGroupCollection::oldestMeasurementOffset = 0;
    TEST_ASSERT_FALSE(measurementGroupCollection::isValid());

    measurementGroupCollection::newMeasurementsOffset   = 0;
    measurementGroupCollection::oldestMeasurementOffset = 0xFFFFFFFF;
    TEST_ASSERT_FALSE(measurementGroupCollection::isValid());

    measurementGroupCollection::oldestMeasurementOffset = 0;
    measurementGroupCollection::newMeasurementsOffset   = 0;
    TEST_ASSERT_TRUE(measurementGroupCollection::isValid());
}

void test_initialize(void) {
    TEST_ASSERT_EQUAL_UINT32(0, measurementGroupCollection::getOldestMeasurementOffset());
    TEST_ASSERT_EQUAL_UINT32(0, measurementGroupCollection::getNewMeasurementsOffset());

    settingsCollection::save(123, settingsCollection::settingIndex::oldestMeasurementOffset);
    settingsCollection::save(456, settingsCollection::settingIndex::newMeasurementsOffset);
    measurementGroupCollection::initialize();

    TEST_ASSERT_EQUAL_UINT32(123, measurementGroupCollection::getOldestMeasurementOffset());
    TEST_ASSERT_EQUAL_UINT32(456, measurementGroupCollection::getNewMeasurementsOffset());
}

void test_reset(void) {
    measurementGroupCollection::reset();
    TEST_ASSERT_EQUAL_UINT32(0, measurementGroupCollection::getOldestMeasurementOffset());
    TEST_ASSERT_EQUAL_UINT32(0, measurementGroupCollection::getNewMeasurementsOffset());
}

void test_getFreeSpace(void) {
    nonVolatileStorage::mockEepromNmbr64KPages = 2;
    nonVolatileStorage::detectNmbr64KBanks();

    // No measurements stored -> free space should equal the size of the measurements area
    measurementGroupCollection::oldestMeasurementOffset = 0;
    measurementGroupCollection::newMeasurementsOffset   = 0;
    TEST_ASSERT_EQUAL_UINT32(nonVolatileStorage::getMeasurementsAreaSize(), measurementGroupCollection::getFreeSpace());

    // Simulate some measurements stored, at the first half of the memory
    measurementGroupCollection::oldestMeasurementOffset = 0;
    measurementGroupCollection::newMeasurementsOffset   = nonVolatileStorage::getMeasurementsAreaSize() / 2;
    TEST_ASSERT_EQUAL_UINT32(nonVolatileStorage::getMeasurementsAreaSize() / 2, measurementGroupCollection::getFreeSpace());

    // Now assume the free area is wrapping around the end of the memory, measurements stored at the last quarter and the first quarter of the memory
    measurementGroupCollection::oldestMeasurementOffset = nonVolatileStorage::getMeasurementsAreaSize() * 3 / 4;
    measurementGroupCollection::newMeasurementsOffset   = (nonVolatileStorage::getMeasurementsAreaSize()) / 4;
    TEST_ASSERT_EQUAL_UINT32(nonVolatileStorage::getMeasurementsAreaSize() / 2, measurementGroupCollection::getFreeSpace());
}

void test_getAddressFromOffset() {
    TEST_ASSERT_EQUAL_UINT32(nonVolatileStorage::measurementsStartAddress, measurementGroupCollection::getAddressFromOffset(0));
    TEST_ASSERT_EQUAL_UINT32(nonVolatileStorage::measurementsStartAddress + 128, measurementGroupCollection::getAddressFromOffset(128));
}

void test_add(void) {
    measurementGroupCollection::reset();

    measurementGroup testMeasurementGroup;
    testMeasurementGroup.addMeasurement(0, 0, 0.0f);
    testMeasurementGroup.addMeasurement(0, 0, 0.0f);

    uint32_t lengthInBytes = measurementGroup::lengthInBytes(testMeasurementGroup.getNumberOfMeasurements());
    static constexpr uint32_t expectLengthInBytes{16};
    TEST_ASSERT_EQUAL(expectLengthInBytes, lengthInBytes);

    measurementGroupCollection::addNew(testMeasurementGroup);
    TEST_ASSERT_EQUAL_UINT32(lengthInBytes, measurementGroupCollection::getNewMeasurementsOffset());
    TEST_ASSERT_EQUAL_UINT32(0, measurementGroupCollection::getOldestMeasurementOffset());

    uint8_t expectedBytes[expectLengthInBytes];
    testMeasurementGroup.toBytes(expectedBytes, expectLengthInBytes);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBytes, nonVolatileStorage::mockEepromMemory + nonVolatileStorage::measurementsStartAddress, expectLengthInBytes);
}

void test_add_wrap(void) {        // addresses in NVS need to wrap around end of measurements area, not overwriting settings in settings area.
    static constexpr uint32_t expectLengthInBytes{16};
    measurementGroupCollection::reset();
    measurementGroupCollection::newMeasurementsOffset = nonVolatileStorage::getMeasurementsAreaSize() - (expectLengthInBytes / 2);        // 8 from the end, writing 16 bytes, should wrap

    measurementGroup testMeasurementGroup;
    testMeasurementGroup.addMeasurement(0, 0, 0.0f);
    testMeasurementGroup.addMeasurement(0, 0, 0.0f);

    uint32_t lengthInBytes = measurementGroup::lengthInBytes(testMeasurementGroup.getNumberOfMeasurements());
    TEST_ASSERT_EQUAL(expectLengthInBytes, lengthInBytes);

    measurementGroupCollection::addNew(testMeasurementGroup);
    TEST_ASSERT_EQUAL_UINT32(expectLengthInBytes / 2, measurementGroupCollection::getNewMeasurementsOffset());

    uint8_t expectedBytes[expectLengthInBytes];
    testMeasurementGroup.toBytes(expectedBytes, expectLengthInBytes);

    uint8_t actualBytes[expectLengthInBytes];
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBytes, nonVolatileStorage::mockEepromMemory + nonVolatileStorage::measurementsStartAddress + nonVolatileStorage::getMeasurementsAreaSize() - (expectLengthInBytes / 2), expectLengthInBytes / 2);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBytes + (expectLengthInBytes / 2), nonVolatileStorage::mockEepromMemory + nonVolatileStorage::measurementsStartAddress, expectLengthInBytes / 2);
}

void test_add_and_read_back(void) {
    static constexpr uint32_t expectLengthInBytes{16};
    measurementGroupCollection::reset();
    measurementGroup testMeasurementGroupToWrite;
    testMeasurementGroupToWrite.addMeasurement(0, 0, 0.0f);
    testMeasurementGroupToWrite.addMeasurement(0, 0, 0.0f);
    measurementGroupCollection::addNew(testMeasurementGroupToWrite);

    measurementGroup testMeasurementGroupToRead;
    measurementGroupCollection::get(testMeasurementGroupToRead, 0);
    TEST_ASSERT_EQUAL_UINT32(2, testMeasurementGroupToRead.getNumberOfMeasurements());
}

void test_eraseOldest() {
    static constexpr uint32_t expectLengthInBytes{16};
    measurementGroupCollection::reset();
    measurementGroup testMeasurementGroupToWrite;
    testMeasurementGroupToWrite.addMeasurement(0, 0, 0.0f);
    testMeasurementGroupToWrite.addMeasurement(0, 0, 0.0f);
    measurementGroupCollection::addNew(testMeasurementGroupToWrite);

    measurementGroupCollection::eraseOldest();
    TEST_ASSERT_EQUAL_UINT32(expectLengthInBytes, measurementGroupCollection::getOldestMeasurementOffset());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_isValid);
    RUN_TEST(test_initialize);
    RUN_TEST(test_reset);
    RUN_TEST(test_getFreeSpace);
    RUN_TEST(test_getAddressFromOffset);
    RUN_TEST(test_add);
    RUN_TEST(test_add_wrap);
    RUN_TEST(test_add_and_read_back);
    RUN_TEST(test_eraseOldest);
    UNITY_END();
}