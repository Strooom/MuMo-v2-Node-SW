#include <uart2.hpp>
#include <gpio.hpp>
#ifndef generic
#include "main.h"
#include "stm32wlxx_ll_usart.h"
extern UART_HandleTypeDef huart2;
#endif

// ######################################################
// ### Initialization                                 ###
// ######################################################

bool uart2::initalized{false};
circularBuffer<uint8_t, uart2::commandBufferLength> uart2::rxBuffer;
circularBuffer<uint8_t, uart2::responseBufferLength> uart2::txBuffer;
uint32_t uart2::commandCounter{0};
#ifdef generic
uint8_t uart2::mockReceivedChar;
uint8_t uart2::mockTransmittedChar;
bool uart2::interruptEnabled{false};
#endif

void uart2::initialize() {
    txBuffer.initialize();
#ifndef generic
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    PeriphClkInitStruct.PeriphClockSelection     = RCC_PERIPHCLK_USART2;
    PeriphClkInitStruct.Usart2ClockSelection     = RCC_USART2CLKSOURCE_PCLK1;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
    __HAL_RCC_USART2_CLK_ENABLE();

    (void)USART2->RDR;        // read RDR to clear it

    USART2->BRR = 0x8B;                                          // 115200 baud
    USART2->ICR = 0xFFFF;                                        // clear all pending interrupt flags
    USART2->CR1 = USART2->CR1 | USART_CR1_RXNEIE_RXFNEIE;        // enable Receive data register not empty interrupt
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
    USART2->CR1 = USART2->CR1 | USART_CR1_RE;        // enable Rx direction
    USART2->CR1 = USART2->CR1 | USART_CR1_TE;        // enable Tx direction
    USART2->CR1 = USART2->CR1 | USART_CR1_UE;        // enable UART2
#endif
    gpio::enableGpio(gpio::group::uart2);
#ifndef generic
    HAL_Delay(10);        // after enabling the UART and IOs, some spurious character may be received. Wait a little, then clear the rxBuffer.
#endif
    rxBuffer.initialize();
    commandCounter = 0;
}

// ######################################################
// ### Receiving                                      ###
// ######################################################

void uart2::rxNotEmpty() {
#ifndef generic
    while (USART2->ISR & USART_CR1_RXNEIE_RXFNEIE) {
        uint8_t receivedChar = static_cast<uint8_t>(USART2->RDR);
#else
    uint8_t receivedChar = mockReceivedChar;
#endif
        rxBuffer.push(receivedChar);
        if (receivedChar == commandTerminator) {
            commandCounter++;
        }
#ifndef generic
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
#endif
#ifndef generic
        USART2->TDR = txBuffer.pop();
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

void uart2::transmit(const char* data) {
    transmit(reinterpret_cast<const uint8_t*>(data));
}

void uart2::transmit(const uint8_t* data) {
    for (uint32_t index = 0; index < responseBufferLength; index++) {
        if (data[index] == 0) {
            break;
        }
        txBuffer.push(data[index]);
    }
    startOrContinueTx();
}

void uart2::transmit(const uint8_t* data, const uint32_t length) {
    for (uint32_t index = 0; index < length; index++) {
        txBuffer.push(data[index]);
    }
    startOrContinueTx();
}

void uart2::transmit(const char* data, const uint32_t length) {
    for (uint32_t index = 0; index < length; index++) {
        txBuffer.push(static_cast<uint8_t>(data[index]));
    }
    startOrContinueTx();
}

uint32_t uart2::commandCount() {
    uint32_t result{0};
#ifndef generic
    bool interrupts_enabled = (__get_PRIMASK() == 0);
    __disable_irq();
#endif
    result = commandCounter;
#ifndef generic
    if (interrupts_enabled) {
        __enable_irq();
    }
#endif
    return result;
}

void uart2::receive(char* data) {
    uint32_t index{0};
    uint8_t character{0};
    while (!rxBuffer.isEmpty()) {
        character = rxBuffer.pop();
        if (character == commandTerminator) {
            commandCounter--;
            break;
        } else {
            data[index] = character;
        }
        index++;
    }
    data[index]    = 0;
    commandCounter = 0;
}

uint32_t uart2::amountFreeInTxBuffer() {
    return (txBuffer.length - txBuffer.getLevel());
}
