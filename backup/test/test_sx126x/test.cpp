#include <unity.h>
#include "sx126x.h"


void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_calculateFrequencyRegisterValue() {
    TEST_ASSERT_EQUAL_UINT32(0x36419999, sx126x::calculateFrequencyRegisterValue(868100000));
    TEST_ASSERT_EQUAL_UINT32(0x3644CCCC, sx126x::calculateFrequencyRegisterValue(868300000));
    TEST_ASSERT_EQUAL_UINT32(0x36380000, sx126x::calculateFrequencyRegisterValue(867500000));
    TEST_ASSERT_EQUAL_UINT32(0x363B3333, sx126x::calculateFrequencyRegisterValue(867700000));
    TEST_ASSERT_EQUAL_UINT32(0x36586666, sx126x::calculateFrequencyRegisterValue(869525000));
    TEST_ASSERT_EQUAL_UINT32(0x363E6666, sx126x::calculateFrequencyRegisterValue(867900000));
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_calculateFrequencyRegisterValue);
    UNITY_END();
}