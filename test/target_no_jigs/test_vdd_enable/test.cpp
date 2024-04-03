#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>
#include <gpio.hpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {}
void tearDown(void) {}

void test_enablePower() {
    HAL_GPIO_WritePin(GPIOA, vddEnable_Pin, GPIO_PIN_RESET);
    HAL_Delay(5000);
    TEST_IGNORE_MESSAGE("Reset");
}

void test_disablePower() {
    HAL_GPIO_WritePin(GPIOA, vddEnable_Pin, GPIO_PIN_SET);
    HAL_Delay(5000);
    TEST_IGNORE_MESSAGE("Set");
}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(2000);        // required for testing framework to connect
    SystemClock_Config();

    gpio::enableGpio(gpio::group::vddEnable);

    while (1) {
        UNITY_BEGIN();
        for (uint32_t i = 0; i < 10; i++) {
            RUN_TEST(test_enablePower);
            RUN_TEST(test_disablePower);
        }
        UNITY_END();
    }
}
