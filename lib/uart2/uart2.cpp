#include <uart2.hpp>

#ifndef generic
#include "main.h"
extern UART_HandleTypeDef huart2;
#else
#endif
#include <gpio.hpp>

void uart2x::initialize() {
    // TODO : initialize the UART1 peripheral in STM32 registers
    // TODO : initialize the related GPIO pins
    huart2.Instance                    = USART1;
    huart2.Init.BaudRate               = 115200;
    huart2.Init.WordLength             = UART_WORDLENGTH_8B;
    huart2.Init.StopBits               = UART_STOPBITS_1;
    huart2.Init.Parity                 = UART_PARITY_NONE;
    huart2.Init.Mode                   = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling           = UART_OVERSAMPLING_16;
    huart2.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
    huart2.Init.ClockPrescaler         = UART_PRESCALER_DIV1;
    huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    
    if (HAL_UART_Init(&huart2) != HAL_OK) {
        Error_Handler();
    }
    gpio::enableGpio(gpio::group::uart2);
}


void uart2x::goSleep() {
    HAL_UART_DeInit(&huart2);
    gpio::disableGpio(gpio::group::uart2);
}
