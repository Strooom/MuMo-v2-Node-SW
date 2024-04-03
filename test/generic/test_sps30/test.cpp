#include <unity.h>
#include "sps30.hpp"

uint8_t mockSPS30Registers[256];

void setUp(void) {
    //mockSPS30Registers[static_cast<uint8_t>(tsl2591::registers::id)]      = tsl2591::chipIdValue;
}

void tearDown(void) {}        // after test

void test_isPresent() {
    TEST_ASSERT_TRUE(sps30::isPresent());
}

void test_initialize() {
    TEST_ASSERT_EQUAL(sensorDeviceState::unknown, sps30::state);
    sps30::initialize();
    TEST_ASSERT_EQUAL(sensorDeviceState::sleeping, sps30::state);
}

void test_sample() {
    sps30::startSampling();
    // TEST_ASSERT_EQUAL_UINT32(0, sps30::rawChannel0);
    // TEST_ASSERT_EQUAL_UINT32(0, sps30::rawChannel1);

}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_isPresent);
    RUN_TEST(test_initialize);
    RUN_TEST(test_sample);
    UNITY_END();
}
