#include <uart1.hpp>

#ifndef generic
#include "main.h"
extern UART_HandleTypeDef huart1;
#else
#endif
#include <gpio.hpp>

void uart1::initialize() {
    huart1.Instance                    = USART1;
    huart1.Init.BaudRate               = 115200;
    huart1.Init.WordLength             = UART_WORDLENGTH_8B;
    huart1.Init.StopBits               = UART_STOPBITS_1;
    huart1.Init.Parity                 = UART_PARITY_NONE;
    huart1.Init.Mode                   = UART_MODE_TX;
    huart1.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling           = UART_OVERSAMPLING_16;
    huart1.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
    huart1.Init.ClockPrescaler         = UART_PRESCALER_DIV1;
    huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    if (HAL_UART_Init(&huart1) != HAL_OK) {
        Error_Handler();
    }

    // __HAL_RCC_USART1_CLK_ENABLE();
    // CR1 : 0xC - TE, RE CHECK !!
    // CR2 : 0x0 = defaults ??
    // CR3 : 0x00 = defaults ??
    // PRESC : 0x0 = defaults ??
    // BRR = 0x8b = 115200 baudrate
    // CLEAR_BIT(huart->Instance->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
    // CLEAR_BIT(huart->Instance->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));
    // __HAL_UART_ENABLE(huart); -? sets CR1 to 0xD

    gpio::enableGpio(gpio::group::uart1);
}

void uart1::goSleep() {
    HAL_UART_DeInit(&huart1);
    gpio::disableGpio(gpio::group::uart1);
}

void uart1::transmit(const char *data, uint32_t dataLength) {
#ifndef generic
    HAL_UART_Transmit(&huart1, (uint8_t *)data, static_cast<const uint16_t>(dataLength), 1000);
#endif
}