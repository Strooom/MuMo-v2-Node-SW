#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <aesblock.hpp>
#include <hexascii.hpp>
#include <gpio.hpp>
// #include <lorawan.hpp>
// #include <settingscollection.hpp>
// #include <maccommand.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {
}
void tearDown(void) {}

void test_spare_IOs() {
    gpio::enableDisableGpio(gpio::group::other, true);
    for (uint32_t i = 0; i < 16; i++) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
        HAL_Delay(1000);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
        HAL_Delay(1000);
    }
}

int main(int argc, char** argv) {
    HAL_Init();
    HAL_Delay(2000);
    SystemClock_Config();
    MX_USART1_UART_Init();

    UNITY_BEGIN();
    RUN_TEST(test_spare_IOs);
    UNITY_END();
}