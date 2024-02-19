#include <unity.h>
#include "main.h"
#include <clock.cpp>
#include <errorhandler.cpp>
#include <uart1.cpp>
#include <uart2.cpp>
#include <gpio.hpp>
#include <lptim1.cpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>

#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {}
void tearDown(void) {}

void test_loopbackPollingMode() {
    // simple loopback test, using USART1 in polling mode
    static constexpr uint32_t testDataLength{16};
    uint8_t txData[testDataLength] = "Hello, World!";
    uint8_t rxData[testDataLength];
    for (uint32_t byteIndex = 0; byteIndex < testDataLength; byteIndex++) {
        HAL_UART_Transmit(&huart1, &txData[byteIndex], 1, 0);
        HAL_UART_Receive(&huart1, &rxData[byteIndex], 1, 100);
    }
    TEST_ASSERT_EQUAL_UINT8_ARRAY(txData, rxData, testDataLength);
}

void test_loopbackInterruptMode() {
    // loopback test, using USART1 in interrupt mode
    static constexpr uint32_t testDataLength{16};
    uint8_t txData[testDataLength] = "Hello, World!";
    uint8_t rxData[testDataLength];
    for (uint32_t byteIndex = 0; byteIndex < testDataLength; byteIndex++) {
        HAL_UART_Transmit_IT(&huart1, &txData[byteIndex], 1, 0);
        HAL_UART_Receive(&huart1, &rxData[byteIndex], 1, 100);
    }
    TEST_ASSERT_EQUAL_UINT8_ARRAY(txData, rxData, testDataLength);
}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(2000);        // required for testing framework to connect
    SystemClock_Config();
    MX_USART1_UART_Init();
    UNITY_BEGIN();
    RUN_TEST(test_loopbackPollingMode);
    RUN_TEST(test_loopbackInterruptMode);

    UNITY_END();
}
