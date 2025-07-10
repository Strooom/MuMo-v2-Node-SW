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
#include <stdlib.h>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {}
void tearDown(void) {}

void test_isPresent() {
    TEST_ASSERT_TRUE(scd40::isPresent());
}

void test_initialize() {
    TEST_ASSERT_EQUAL(sensorDeviceState::unknown, scd40::getState());
    scd40::initialize();
    TEST_ASSERT_EQUAL(sensorDeviceState::sleeping, scd40::getState());
}

void test_run() {
    scd40::initialize();
    scd40::startSampling();
    TEST_ASSERT_EQUAL(sensorDeviceState::sampling, scd40::getState());
    uint32_t startTime = HAL_GetTick();
    bool samplingOk{false};
    while (HAL_GetTick() - startTime < 10000) {
        if (scd40::samplingIsReady()) {
            samplingOk = true;
            break;
        }
    }
    TEST_ASSERT_TRUE(samplingOk);
    scd40::readSample();
    float temperature = scd40::calculateTemperature(scd40::rawDataTemperature);
    float relativeHumidity = scd40::calculateRelativeHumidity(scd40::rawDataRelativeHumidity);
    float co2 = scd40::calculateCO2(scd40::rawCo2);
}

void test_calculateTemperature() {
    TEST_ASSERT_FLOAT_WITHIN(0.1F, 25.0F, scd40::calculateTemperature(0x6667));
}

void test_calculateRelativeHumidity() {
    TEST_ASSERT_FLOAT_WITHIN(0.1F, 37.0F, scd40::calculateRelativeHumidity(0x5eb9));
}
void test_calculateCO2() {
    TEST_ASSERT_FLOAT_WITHIN(0.1F, 500.0F, scd40::calculateCO2(0x01f4));
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
    RUN_TEST(test_run);
    RUN_TEST(test_calculateTemperature);
    RUN_TEST(test_calculateRelativeHumidity);
    RUN_TEST(test_calculateCO2);
    UNITY_END();
}
