#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <gpio.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>
#include <sht40.hpp>
#include <i2c.hpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {}
void tearDown(void) {}

void test_isPresent() {
    i2c::wakeUp();
    TEST_ASSERT_TRUE(sht40::isPresent());
}

void test_measurement() {
    sht40::startSampling();
    while (!sht40::samplingIsReady()) {
        HAL_Delay(1);
    }
    sht40::readSample();
    float temperature = sht40::calculateTemperature();
    TEST_ASSERT_FLOAT_WITHIN(4.0F, 22.0F, temperature);
    float relativeHumidity = sht40::calculateRelativeHumidity();
    TEST_ASSERT_FLOAT_WITHIN(10.0F, 50.0F, relativeHumidity);
}

void test_isNotPresentI2CSleeping() {
    i2c::goSleep();
    TEST_ASSERT_FALSE(sht40::isPresent());
}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(2000);
    SystemClock_Config();

    UNITY_BEGIN();
    RUN_TEST(test_isPresent);
    RUN_TEST(test_measurement);
    RUN_TEST(test_isNotPresentI2CSleeping);
    UNITY_END();
}
