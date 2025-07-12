#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <gpio.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>
#include <sps30.hpp>
#include <i2c.hpp>
#include <uart1.hpp>
#include <stdio.h>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {}
void tearDown(void) {}

void test_isPresent() {
    TEST_ASSERT_TRUE(sps30::isPresent());
}

void test_initialize() {
    TEST_ASSERT_EQUAL(sensorDeviceState::unknown, sps30::getState());
    sps30::initialize();
    TEST_ASSERT_EQUAL(sensorDeviceState::standby, sps30::getState());
    HAL_Delay(1000);
}

void test_run() {
    sps30::startSampling();
    TEST_ASSERT_EQUAL(sensorDeviceState::sampling, sps30::getState());
    while (true) {
        if ((sps30::getState() == sensorDeviceState::sampling) && sps30::samplingIsReady()) {
            sps30::readSample();
            break;
        }
    }

    TEST_ASSERT_FLOAT_WITHIN(1.0F, 10.0F, sps30::_pme1);
    // TEST_ASSERT_FLOAT_WITHIN(1.0F, 10.0F, sps30::_pme2dot5);
    // TEST_ASSERT_FLOAT_WITHIN(1.0F, 10.0F, sps30::_pme4);
    // TEST_ASSERT_FLOAT_WITHIN(1.0F, 10.0F, sps30::_pme10);
}

int main(int argc, char **argv) {
    HAL_Init();
    SystemClock_Config();
    HAL_Delay(1000);

    gpio::initialize();
    uart1::initialize();
    i2c::wakeUp();

    UNITY_BEGIN();
    RUN_TEST(test_isPresent);
    RUN_TEST(test_initialize);
    for (uint32_t runIndex = 0; runIndex < 32; runIndex++) {
        RUN_TEST(test_run);
        HAL_Delay(100);
    }
    UNITY_END();
}
