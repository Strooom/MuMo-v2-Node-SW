// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include "datarate.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_dataRate() {
    dataRates testDataRates;
    TEST_ASSERT_EQUAL(spreadingFactor::SF12, testDataRates.theDataRates[0].theSpreadingFactor);
    TEST_ASSERT_EQUAL(spreadingFactor::SF7, testDataRates.theDataRates[5].theSpreadingFactor);

    TEST_ASSERT_EQUAL_UINT32(0, testDataRates.getDownlinkDataRateIndex(0, 0));
    TEST_ASSERT_EQUAL_UINT32(5, testDataRates.getDownlinkDataRateIndex(5, 0));
    TEST_ASSERT_EQUAL_UINT32(0, testDataRates.getDownlinkDataRateIndex(0, 0));
    TEST_ASSERT_EQUAL_UINT32(0, testDataRates.getDownlinkDataRateIndex(5, 5));
    TEST_ASSERT_EQUAL_UINT32(0, testDataRates.getDownlinkDataRateIndex(3, 5));
    
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_dataRate);
    UNITY_END();
}

