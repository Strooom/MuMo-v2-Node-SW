#include <unity.h>
#include <measurementcollection.hpp>

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_initialize() {
    TEST_ASSERT_EQUAL(0, measurementCollection::head);
    TEST_ASSERT_EQUAL(0, measurementCollection::level);
    TEST_ASSERT_EQUAL(0, measurementCollection::measurementWriteIndex);
    TEST_ASSERT_EQUAL(0, measurementCollection::oldestUnsentMeasurementIndex);
    TEST_ASSERT_EQUAL(0, measurementCollection::oldestUnconfirmedMeasurementindex);
}

void test_save_Read_measurement() {
    measurement dataIn;
    dataIn.timestamp.asDoubleWord = 0x12345678;
    dataIn.value.asDoubleWord     = 0xAA55FF00;
    dataIn.sensorDeviceId         = 0;
    dataIn.channelAndFlags        = 0;
    measurementCollection::write(0, dataIn);
    measurement dataOut;
    measurementCollection::read(0, dataOut);
    TEST_ASSERT_EQUAL(dataIn.timestamp.asDoubleWord, dataOut.timestamp.asDoubleWord);
    TEST_ASSERT_EQUAL(dataIn.value.asDoubleWord, dataOut.value.asDoubleWord);
    // TEST_ASSERT_EQUAL(dataIn.type, dataOut.type);
    // TEST_ASSERT_EQUAL(dataIn.flags, dataOut.flags);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_save_Read_measurement);
    UNITY_END();
}