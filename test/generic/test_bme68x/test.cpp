#include <unity.h>
#include <power.hpp>
#include <bme680.hpp>
#include <cstring>

extern uint8_t mockBME680Registers[256];

void setUp(void) {
    mockBME680Registers[static_cast<uint8_t>(bme680::registers::chipId)]      = bme680::chipIdValue;
    mockBME680Registers[static_cast<uint8_t>(bme680::registers::meas_status)] = 0x80;

    // Calibration data
    uint8_t calibrationDataPart1[23] = {0x62, 0x67, 0x03, 0x10, 0x8F, 0x90, 0x68, 0xD7, 0x58, 0x00, 0x38, 0x22, 0x62, 0xFF, 0x2C, 0x1E, 0x00, 0x00, 0x71, 0xF4, 0x5B, 0xF6, 0x1E};
    uint8_t calibrationDataPart2[14] = {0x3D, 0xBD, 0x37, 0x00, 0x2D, 0x14, 0x78, 0x9C, 0xB6, 0x65, 0xAB, 0xDC, 0xFB, 0x12};
    uint8_t calibrationDataPart3[5]  = {0x28, 0xAA, 0x16, 0x4C, 0x03};
    memcpy(mockBME680Registers + 0x8A, calibrationDataPart1, 23);
    memcpy(mockBME680Registers + 0xE1, calibrationDataPart2, 14);
    memcpy(mockBME680Registers + 0xF0, calibrationDataPart3, 5);

    // Raw Measurements Data : TODO
}

void tearDown(void) {}        // after test

void test_isPresent() {
    mockBME680Registers[static_cast<uint8_t>(bme680::registers::chipId)] = bme680::chipIdValue;
    TEST_ASSERT_TRUE(bme680::isPresent());
}

void test_initialize() {
    TEST_ASSERT_EQUAL(sensorDeviceState::unknown, bme680::state);
    bme680::initialize();
    TEST_ASSERT_EQUAL(sensorDeviceState::sleeping, bme680::state);
}


void test_tickAndRun() {
    power::mockUsbPower = false;
    bme680::initialize();
    bme680::channels[bme680::temperature].set(0, 1, 0, 0);
    bme680::channels[bme680::relativeHumidity].set(0, 0, 0, 0);
    bme680::channels[bme680::barometricPressure].set(0, 0, 0, 0);
    TEST_ASSERT_TRUE(bme680::channels[bme680::temperature].needsSampling());
    TEST_ASSERT_FALSE(bme680::channels[bme680::relativeHumidity].needsSampling());
    TEST_ASSERT_FALSE(bme680::channels[bme680::barometricPressure].needsSampling());
    TEST_ASSERT_TRUE(bme680::anyChannelNeedsSampling());
    bme680::tick();
    TEST_ASSERT_EQUAL(sensorDeviceState::sampling, bme680::state);
    bme680::run();
    TEST_ASSERT_EQUAL(sensorDeviceState::sleeping, bme680::state);

    bme680::channels[bme680::temperature].set(0, 0, 0, 0);
    bme680::channels[bme680::relativeHumidity].set(0, 1, 0, 0);
    bme680::channels[bme680::barometricPressure].set(0, 1, 0, 0);
    TEST_ASSERT_FALSE(bme680::channels[bme680::temperature].needsSampling());
    TEST_ASSERT_TRUE(bme680::channels[bme680::relativeHumidity].needsSampling());
    TEST_ASSERT_TRUE(bme680::channels[bme680::barometricPressure].needsSampling());
    TEST_ASSERT_TRUE(bme680::anyChannelNeedsSampling());
    bme680::tick();
    TEST_ASSERT_EQUAL(sensorDeviceState::sampling, bme680::state);
    bme680::run();
    TEST_ASSERT_EQUAL(sensorDeviceState::sleeping, bme680::state);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_isPresent);
    RUN_TEST(test_initialize);
    RUN_TEST(test_tickAndRun);
    UNITY_END();
}
