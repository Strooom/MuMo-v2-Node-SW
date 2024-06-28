#ifndef generic
#include "main.h"
#include "stm32wlxx_ll_usart.h"
extern UART_HandleTypeDef huart2;
#else
#endif
#include <uart.hpp>

// ######################################################
// ### Initialization                                 ###
// ######################################################

circularBuffer<uint8_t, uart2::commandBufferLength> uart2::rxBuffer;
circularBuffer<uint8_t, uart2::responseBufferLength> uart2::txBuffer;
uint32_t uart2::commandCounter{0};

void uart2::initialize() {
#ifndef generic

    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    PeriphClkInitStruct.PeriphClockSelection     = RCC_PERIPHCLK_USART2;
    PeriphClkInitStruct.Usart2ClockSelection     = RCC_USART2CLKSOURCE_PCLK1;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
    __HAL_RCC_USART2_CLK_ENABLE();

    uint32_t dummy = USART2->RDR;

    USART2->BRR = 0x8B;                                          // 115200 baud
    USART2->ICR = 0xFFFF;                                        // clear all pending interrupt flags
    USART2->CR1 = USART2->CR1 | USART_CR1_RXNEIE_RXFNEIE;        // enable Receive data register not empty interrupt
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
    USART2->CR1 = USART2->CR1 | USART_CR1_RE;        // enable Rx direction
    USART2->CR1 = USART2->CR1 | USART_CR1_TE;        // enable Tx direction
    USART2->CR1 = USART2->CR1 | USART_CR1_UE;        // enable UART2

#endif
}

// ######################################################
// ### Receiving                                      ###
// ######################################################

void uart2::rxNotEmpty() {
#ifndef generic
    while (USART2->ISR & USART_CR1_RXNEIE_RXFNEIE) {
        uint8_t receivedChar = USART2->RDR;
        if (receivedChar == bootLoaderMagicValue) {
            // jumpToBootLoader();
        } else {
            rxBuffer.push(receivedChar);
        }
        if (receivedChar == commandTerminator) {
            commandCounter++;
        }
    }
#endif
}

// ######################################################
// ### Transmitting                                   ###
// ######################################################

void uart2::startOrContinueTx() {
    if (!txBuffer.isEmpty()) {
#ifndef generic
        bool interrupts_enabled = (__get_PRIMASK() == 0);
        __disable_irq();
        USART2->CR1 |= tdrEmpty;
        if (interrupts_enabled) {
            __enable_irq();
        }
#endif
    }
}

void uart2::txEmpty() {
#ifndef generic
    bool interrupts_enabled = (__get_PRIMASK() == 0);
    __disable_irq();
    if (txBuffer.isEmpty()) {
        USART2->CR1 &= ~tdrEmpty;
    } else {
        USART2->TDR = txBuffer.pop();
    }
    if (interrupts_enabled) {
        __enable_irq();
    }
#endif
}

void uart2::send(const char* data) {
    send(reinterpret_cast<const uint8_t*>(data));
}

void uart2::send(const uint8_t* data) {
    for (uint32_t index = 0; index < responseBufferLength; index++) {
        if (data[index] == 0) {
            break;
        }
        txBuffer.push(data[index]);
    }
    startOrContinueTx();
}

void uart2::send(const uint8_t* data, uint32_t length) {
    if (length > responseBufferLength) {
        length = responseBufferLength;
    }
    for (uint32_t index = 0; index < length; index++) {
        txBuffer.push(data[index]);
    }
    startOrContinueTx();
}

uint32_t uart2::commandCount() {
#ifndef generic
    bool interrupts_enabled = (__get_PRIMASK() == 0);
    __disable_irq();
    uint32_t result = commandCounter;
    if (interrupts_enabled) {
        __enable_irq();
    }
    return result;
#endif
}

void uart2::read(char* data) {
    uint32_t index{0};
    while (!rxBuffer.isEmpty()) {
        uint8_t character = rxBuffer.pop();
        if (character == commandTerminator) {
            commandCounter--;
            break;
        } else {
            data[index] = character;
        }
        index++;
    }
    data[index] = 0;
}
