#include <unity.h>
#include <power.hpp>
#include <bme680.hpp>
#include <cstring>

extern uint8_t mockBME680Registers[256];

void setUp(void) {}
void tearDown(void) {}

void test_isPresent() {
    mockBME680Registers[static_cast<uint8_t>(bme680::registers::chipId)] = bme680::chipIdValue;
    TEST_ASSERT_TRUE(bme680::isPresent());
}

void test_initialize() {
    TEST_ASSERT_EQUAL(sensorDeviceState::unknown, bme680::state);
    bme680::initialize();
    TEST_ASSERT_EQUAL(sensorDeviceState::sleeping, bme680::state);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_isPresent);
    RUN_TEST(test_initialize);
    UNITY_END();
}
