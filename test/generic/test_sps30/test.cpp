#include <unity.h>
#include <sps30.hpp>
#include <sensirion.hpp>

// extern uint8_t mockSPS30Registers[256];

void setUp(void) {}
void tearDown(void) {}

void test_initialize() {
    TEST_ASSERT_EQUAL(sensorDeviceState::unknown, sps30::state);
    sps30::initialize();
    TEST_ASSERT_EQUAL(sensorDeviceState::standby, sps30::state);
}

void test_startSamplingCommand() {
    static constexpr uint32_t commandDataLength{3};
    uint8_t commandData[commandDataLength]{5, 0, 0};
    sensirion::insertCrc(commandData, commandDataLength);
    TEST_ASSERT_EQUAL_UINT8(246, commandData[2]);
}


void test_samplingIsReadyCommand() {
    static constexpr uint32_t commandDataLength{3};
    uint8_t commandData[commandDataLength]{0, 1, 0};
    sensirion::insertCrc(commandData, commandDataLength);
    TEST_ASSERT_EQUAL_UINT8(176, commandData[2]);
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_startSamplingCommand);
    RUN_TEST(test_samplingIsReadyCommand);
    UNITY_END();
}
