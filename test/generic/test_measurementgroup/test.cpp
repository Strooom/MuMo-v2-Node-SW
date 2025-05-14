#include <unity.h>
#include <nvs.hpp>
#include <measurementgroup.hpp>

void setUp(void) {}
void tearDown(void) {}

void test_initialize() {
    measurementGroup testGroup;
    TEST_ASSERT_EQUAL(0, testGroup.nmbrOfMeasurements);
    TEST_ASSERT_EQUAL(measurementGroup::oldestPossibleTimestamp, testGroup.timestamp);
    TEST_ASSERT_TRUE(testGroup.isValid());
}

void test_isValid() {
    measurementGroup testGroup;
    TEST_ASSERT_TRUE(testGroup.isValid());
    testGroup.checkSum = 0x00;
    TEST_ASSERT_FALSE(testGroup.isValid());
    testGroup.checkSum  = measurementGroup::defaultChecksum;
    testGroup.timestamp = measurementGroup::oldestPossibleTimestamp - 1;
    TEST_ASSERT_FALSE(testGroup.isValid());
}

void test_lengthInBytes() {
    measurementGroup testGroup;
    TEST_ASSERT_EQUAL(6, testGroup.lengthInBytes(0));
    TEST_ASSERT_EQUAL(11, testGroup.lengthInBytes(1));
    TEST_ASSERT_EQUAL(16, testGroup.lengthInBytes(2));
    TEST_ASSERT_EQUAL(86, testGroup.lengthInBytes(measurementGroup::maxNmbrOfMeasurements));
}

void test_addMeasurement() {
    measurementGroup testGroup;
    testGroup.addMeasurement(0, 0, 3.0F);
    TEST_ASSERT_EQUAL(1, testGroup.nmbrOfMeasurements);
    TEST_ASSERT_EQUAL(0, testGroup.measurements[0].deviceIndex);
    TEST_ASSERT_EQUAL(0, testGroup.measurements[0].channelIndex);
    TEST_ASSERT_EQUAL(3.0F, testGroup.measurements[0].value);        // hex value is 0x40400000 so it has no effect on the checksum
    uint8_t expectedChecksum = 0xB4 ^ 0x01;
    TEST_ASSERT_EQUAL(expectedChecksum, testGroup.calculateChecksum());

    testGroup.addMeasurement(1, 2, 12.0F);        // 12.0F in hex is 0x41400000, device and channel is 0b00001'010
    TEST_ASSERT_EQUAL(2, testGroup.nmbrOfMeasurements);
    expectedChecksum = expectedChecksum ^ 0x01 ^ 0x02 ^ 0b00001010 ^ 0x41 ^ 0x40;
    TEST_ASSERT_EQUAL(expectedChecksum, testGroup.calculateChecksum());
}

void test_addMeasurement2() {
    measurementGroup testGroup;
    for (uint32_t index = 0; index < measurementGroup::maxNmbrOfMeasurements; index++) {
        testGroup.addMeasurement(index, index, static_cast<float>(index));
    }
    TEST_ASSERT_EQUAL(measurementGroup::maxNmbrOfMeasurements, testGroup.nmbrOfMeasurements);

    testGroup.addMeasurement(0, 0, 0.0F);        // try to add a measurement when the group is full
    TEST_ASSERT_EQUAL(measurementGroup::maxNmbrOfMeasurements, testGroup.nmbrOfMeasurements);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_initialize);
    RUN_TEST(test_isValid);
    RUN_TEST(test_lengthInBytes);
    RUN_TEST(test_addMeasurement);
    RUN_TEST(test_addMeasurement2);

    UNITY_END();
}