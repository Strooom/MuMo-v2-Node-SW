#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <gpio.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>
#include <scd40.hpp>
#include <i2c.hpp>
#include <uart1.hpp>
#include <stdio.h>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {}
void tearDown(void) {}

void test_isPresent() {
    TEST_ASSERT_TRUE(scd40::isPresent());
}

void test_initialize() {
    TEST_ASSERT_EQUAL(sensorDeviceState::unknown, scd40::getState());
    scd40::initialize();
    // HAL_Delay(10);
    TEST_ASSERT_EQUAL(sensorDeviceState::sleeping, scd40::getState());
}

void test_samplingReady() {
    if (scd40::samplingIsReady()) {
        scd40::readSample();
    }
    TEST_ASSERT_FALSE(scd40::samplingIsReady());
}

void test_run() {
    scd40::startSampling();
    TEST_ASSERT_EQUAL(sensorDeviceState::sampling, scd40::getState());

    while (true) {
        if ((scd40::getState() == sensorDeviceState::sampling) && scd40::samplingIsReady()) {
            scd40::readSample();
            break;
        }
    }
    float scd40Temperature      = scd40::calculateTemperature(scd40::rawDataTemperature);
    float scd40RelativeHumidity = scd40::calculateRelativeHumidity(scd40::rawDataRelativeHumidity);
    float scd40CO2              = scd40::calculateCO2(scd40::rawCo2);

    TEST_ASSERT_FLOAT_WITHIN(3.0F, 25.0F, scd40Temperature);
    TEST_ASSERT_FLOAT_WITHIN(10.0F, 60.0F, scd40RelativeHumidity);
    TEST_ASSERT_FLOAT_WITHIN(100.0F, 400.0F, scd40CO2);
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
    RUN_TEST(test_samplingReady);
    for (uint32_t runIndex = 0; runIndex < 10; runIndex++) {
        RUN_TEST(test_run);
        HAL_Delay(100);
    }
    UNITY_END();
}
