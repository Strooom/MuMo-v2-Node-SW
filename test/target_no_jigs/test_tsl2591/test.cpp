#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <gpio.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>
#include <tsl2591.hpp>
#include <i2c.hpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {}
void tearDown(void) {}

void test_isPresent() {
    i2c::wakeUp();
    TEST_ASSERT_TRUE(tsl2591::isPresent());
}

void test_measurement() {
    tsl2591::initialize();
    tsl2591::startSampling();
    while (!tsl2591::samplingIsReady()) {
        HAL_Delay(1);
    }
    tsl2591::readSample();
    float lux = tsl2591::calculateLux();
    TEST_ASSERT_FLOAT_WITHIN(9.0F, 10.0F, lux);
}

void test_isNotPresentI2CSleeping() {
    i2c::goSleep();
    TEST_ASSERT_FALSE(tsl2591::isPresent());
}


int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(2000);        // required for testing framework to connect
    SystemClock_Config();

    UNITY_BEGIN();
    RUN_TEST(test_isPresent);
    RUN_TEST(test_measurement);
    RUN_TEST(test_isNotPresentI2CSleeping);

    UNITY_END();
}
