#include <unity.h>
#include <nvs.hpp>
#include <measurementgroup.hpp>

void setUp(void) {}
void tearDown(void) {}

void test_initialize() {
    measurementGroup testGroup;
    TEST_ASSERT_EQUAL(0, testGroup.nmbrOfMeasurements);
    TEST_ASSERT_EQUAL(measurementGroup::oldestPossibleTimestamp, testGroup.timestamp);
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

    testGroup.addMeasurement(1, 2, 12.0F);        // 12.0F in hex is 0x41400000, device and channel is 0b00001'010
    TEST_ASSERT_EQUAL(2, testGroup.nmbrOfMeasurements);
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

void test_toBytes() {
    measurementGroup testGroup;
    time_t testTimestamp{1577836800};        // Jan 01 2020 00:00:00 GMT+0000
    uint32_t testDeviceIndex{1};
    uint32_t testChannelIndex{2};
    float testValue{123.5F};        // https://www.h-schmidt.net/FloatConverter/IEEE754.html
    testGroup.setTimestamp(testTimestamp);
    testGroup.addMeasurement(testDeviceIndex, testChannelIndex, testValue);
    testGroup.addMeasurement(testDeviceIndex, testChannelIndex, testValue);

    // For group with 2 measurements, the length in bytes is 16
    uint8_t testBytes[16];
    testGroup.toBytes(testBytes, sizeof(testBytes));
    uint8_t expectedBytes[16] = {
        0x02,                          // nmbrOfMeasurements
        0x00, 0xE1, 0x0B, 0x5E,        // timestamp as bytes (Jan 01 2020) 0x5E0BE100, checksum is 0xB4
        0b00001010,                    // deviceIndex 01, channelIndex 02, compressed to 0b00001010
        0x00, 0x00, 0xf7, 0x42,        // value
        0b00001010,                    // deviceIndex 01, channelIndex 02, compressed to 0b00001010
        0x00, 0x00, 0xf7, 0x42,        // value
        0xB6                           // checksum = 0xB4 ^ 0x02
    };
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expectedBytes, testBytes, sizeof(testBytes));
}

void test_fromBytes() {
    measurementGroup testGroup;
    uint8_t testBytes[16] = {
        0x02,                          // nmbrOfMeasurements
        0x00, 0xE1, 0x0B, 0x5E,        // timestamp as bytes (Jan 01 2020) 0x5E0BE100, checksum is 0xB4
        0b00001010,                    // deviceIndex 01, channelIndex 02, compressed to 0b00001010
        0x00, 0x00, 0xf7, 0x42,        // value
        0b00001010,                    // deviceIndex 01, channelIndex 02, compressed to 0b00001010
        0x00, 0x00, 0xf7, 0x42,        // value
        0xB6                           // checksum = 0xB4 ^ 0x02
    };
    testGroup.fromBytes(testBytes);
    TEST_ASSERT_EQUAL(2, testGroup.nmbrOfMeasurements);
    TEST_ASSERT_EQUAL(1577836800, testGroup.timestamp);
    TEST_ASSERT_EQUAL(1, testGroup.measurements[0].deviceIndex);
    TEST_ASSERT_EQUAL(2, testGroup.measurements[0].channelIndex);
    TEST_ASSERT_EQUAL(123.5F, testGroup.measurements[0].value);
    TEST_ASSERT_EQUAL(1, testGroup.measurements[1].deviceIndex);
    TEST_ASSERT_EQUAL(2, testGroup.measurements[1].channelIndex);
    TEST_ASSERT_EQUAL(123.5F, testGroup.measurements[1].value);
}

void test_toBytes_edge() {
    measurementGroup testGroup;
    uint8_t testBytes[5]{};        // too short to convert group with 0 measurements to bytes, so bytes should stay zero
    testGroup.toBytes(testBytes, sizeof(testBytes));
    uint8_t expectedBytes[5] = {0, 0, 0, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expectedBytes, testBytes, sizeof(testBytes));
}

void test_fromBytes_edge() {
    measurementGroup testGroup;
    uint8_t testBytes[6] = {
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x01        // wrong checksum
    };
    testGroup.fromBytes(testBytes);
    TEST_ASSERT_EQUAL(0, testGroup.nmbrOfMeasurements);
    TEST_ASSERT_EQUAL(1577836800, testGroup.timestamp);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_initialize);
    RUN_TEST(test_lengthInBytes);
    RUN_TEST(test_addMeasurement);
    RUN_TEST(test_addMeasurement2);
    RUN_TEST(test_toBytes);
    RUN_TEST(test_fromBytes);
    RUN_TEST(test_toBytes_edge);
    RUN_TEST(test_fromBytes_edge);

    UNITY_END();
}