#include <unity.h>
#include "tsl2591.hpp"

uint8_t mockTSL2591Registers[256];

void setUp(void) {
    mockTSL2591Registers[static_cast<uint8_t>(tsl2591::registers::id)]      = tsl2591::chipIdValue;
}

void tearDown(void) {}        // after test

void test_isPresent() {
    TEST_ASSERT_TRUE(tsl2591::isPresent());
}

void test_initialize() {
    TEST_ASSERT_EQUAL(sensorDeviceState::unknown, tsl2591::state);
    tsl2591::initialize();
    TEST_ASSERT_EQUAL(sensorDeviceState::sleeping, tsl2591::state);
}

void test_sample() {
    tsl2591::startSampling();
    TEST_ASSERT_EQUAL_UINT32(0, tsl2591::rawChannel0);
    TEST_ASSERT_EQUAL_UINT32(0, tsl2591::rawChannel1);

}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_isPresent);
    RUN_TEST(test_initialize);
    RUN_TEST(test_sample);
    UNITY_END();
}
