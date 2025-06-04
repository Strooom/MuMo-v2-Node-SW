#include <uart1.hpp>
#include <gpio.hpp>

#ifndef generic
#include "main.h"
extern UART_HandleTypeDef huart1;
#else
#endif

bool uart1::initalized{false};
circularBuffer<uint8_t, uart1::responseBufferLength> uart1::txBuffer;
#ifdef generic
uint8_t uart1::mockReceivedChar;
uint8_t uart1::mockTransmittedChar;
bool uart1::interruptEnabled{false};
#endif

void uart1::wakeUp() {
    if (!initalized) {
#ifndef generic
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
#endif

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
        initalized = true;
    }
}

void uart1::goSleep() {
    if (initalized) {
#ifndef generic
        HAL_UART_DeInit(&huart1);
#endif
        gpio::disableGpio(gpio::group::uart1);
        initalized = false;
    }
}

void uart1::initialize() {
    txBuffer.initialize();
#ifndef generic
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    PeriphClkInitStruct.PeriphClockSelection     = RCC_PERIPHCLK_USART1;
    PeriphClkInitStruct.Usart1ClockSelection     = RCC_USART1CLKSOURCE_PCLK2;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
    __HAL_RCC_USART1_CLK_ENABLE();

    // (void)USART1->RDR;        // read RDR to clear it

    USART1->BRR = 0x8B;          // 115200 baud
    USART1->ICR = 0xFFFF;        // clear all pending interrupt flags
    // USART1->CR1 = USART1->CR1 | USART_CR1_RXNEIE_RXFNEIE;        // enable Receive data register not empty interrupt
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    // USART1->CR1 = USART1->CR1 | USART_CR1_RE;        // enable Rx direction
    USART1->CR1 = USART1->CR1 | USART_CR1_TE;        // enable Tx direction
    USART1->CR1 = USART1->CR1 | USART_CR1_UE;        // enable UART1
#endif
    gpio::enableGpio(gpio::group::uart1);
#ifndef generic
    // HAL_Delay(10);            // after enabling the UART and IOs, some spurious character may be received. Wait a little, then clear the rxBuffer.
    // (void)USART1->RDR;        // read RDR to clear it

#endif
}

// ######################################################
// ### Receiving                                      ###
// ######################################################

void uart1::rxNotEmpty() {
#ifndef generic
    while (USART1->ISR & USART_CR1_RXNEIE_RXFNEIE) {
        uint8_t receivedChar = static_cast<uint8_t>(USART1->RDR);
#else
    uint8_t receivedChar = mockReceivedChar;        // currently the data received on uart1 is ignored, as it us only used for debug/trace output
#endif
#ifndef generic
    }
#endif
}

// ######################################################
// ### Transmitting                                   ###
// ######################################################

void uart1::startOrContinueTx() {
    if (!txBuffer.isEmpty()) {
#ifndef generic
        bool interrupts_enabled = (__get_PRIMASK() == 0);
        __disable_irq();
        USART1->CR1 |= tdrEmpty;
        if (interrupts_enabled) {
            __enable_irq();
        }
#endif
    }
}

void uart1::txEmpty() {
#ifndef generic
    bool interrupts_enabled = (__get_PRIMASK() == 0);
    __disable_irq();
    if (txBuffer.isEmpty()) {
        USART1->CR1 &= ~tdrEmpty;
    } else {
#endif
#ifndef generic
        USART1->TDR = txBuffer.pop();
#else
    mockTransmittedChar = txBuffer.pop();
#endif
#ifndef generic
    }
    if (interrupts_enabled) {
        __enable_irq();
    }
#endif
}

void uart1::transmit(const char* data) {
    transmit(reinterpret_cast<const uint8_t*>(data));
}

void uart1::transmit(const uint8_t* data) {
    for (uint32_t index = 0; index < responseBufferLength; index++) {
        if (data[index] == 0) {
            break;
        }
        txBuffer.push(data[index]);
    }
    startOrContinueTx();
}

void uart1::transmit(const uint8_t* data, const uint32_t length) {
    for (uint32_t index = 0; index < length; index++) {
        txBuffer.push(data[index]);
    }
    startOrContinueTx();
}

// void uart1::transmit(const uint8_t* data, uint32_t dataLength) {
// #ifndef generic
//     HAL_UART_Transmit(&huart1, data, static_cast<const uint16_t>(dataLength), 1000);
// #endif
// }
