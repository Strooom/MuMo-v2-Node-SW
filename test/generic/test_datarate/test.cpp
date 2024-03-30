#include <unity.h>
#include <datarate.hpp>
void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_getDownlinkDataRateIndex() {
    for (int index = 0; index < dataRates::nmbrUsedDataRates; index++) {
        for (int delta = 0; delta < dataRates::nmbrUsedDataRates; delta++) {
            int minimum = index - delta;
            if (minimum < 0) {
                minimum = 0;
            }
            TEST_ASSERT_EQUAL(minimum, dataRates::getDownlinkDataRateIndex(index, delta));
        }
    }
}
    int main(int argc, char **argv) {
        UNITY_BEGIN();
        RUN_TEST(test_getDownlinkDataRateIndex);
        UNITY_END();
    }
