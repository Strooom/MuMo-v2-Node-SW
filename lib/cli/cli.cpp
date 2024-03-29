#include "clicommand.h"
#include "cli.h"

#ifndef generic
#include "main.h"
extern UART_HandleTypeDef huart2;

#else
#endif

circularBuffer<uint8_t, cli::commandBufferLength> cli::commandBuffer;
circularBuffer<uint8_t, cli::responseBufferLength> cli::responseBuffer;


void cli::startTx() {
    if (!commandBuffer.isEmpty()) {
    #ifndef generic
        bool interrupts_enabled = (__get_PRIMASK() == 0);
        __disable_irq();
        USART2->CR1 = USART2->CR1 | USART_CR1_TXEIE_TXFNFIE;
        if (interrupts_enabled) {
            __enable_irq();
        }


    #endif
}
}

void cli::handleRxEvent() {
#ifndef generic
    uint8_t received_char;
    if (HAL_UART_Receive(&huart2, &received_char, 1, 0) == HAL_OK) {
        commandBuffer.push(received_char);
        if (received_char == '\r') {
            handleEvents();
        }
        if (received_char == bootLoaderMagicValue) {
            jumpToBootLoader();
        }
    }
#endif
}

void cli::handleTxEvent() {
#ifndef generic
    uint8_t received_char;
    if (HAL_UART_Receive(&huart2, &received_char, 1, 0) == HAL_OK) {
        commandBuffer.push(received_char);
        if (received_char == '\r') {
            handleEvents();
        }
        if (received_char == bootLoaderMagicValue) {
            jumpToBootLoader();
        }
    }
#endif
}

void cli::handleEvents() {
}

void cli::jumpToBootLoader() {
#ifndef generic
    HAL_UART_DeInit(&huart2);        // TODO : maybe need to reset some other stuff as well, to bring the MCU back to a clean state as after reset

    uint32_t i = 0;
    void (*SysMemBootJump)(void);
    volatile uint32_t BootAddr = 0x1FFF0000;

    __disable_irq();
    SysTick->CTRL = 0;
    HAL_RCC_DeInit();
    for (i = 0; i < 5; i++) {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }
    __enable_irq();

    SysMemBootJump = (void (*)(void))(*((uint32_t *)((BootAddr + 4))));

    __set_MSP(*(uint32_t *)BootAddr);        // Set the main stack pointer to the boot loader stack

    SysMemBootJump();
#endif
    while (1) {
    }
}



void cli::txEmptyInterrupt() {
#ifndef generic
    if (responseBuffer.isEmpty()) {
        USART2->CR1 = USART2->CR1 & ~USART_CR1_TXEIE_TXFNFIE;
    } else {
        USART2->TDR = responseBuffer.pop();
    }
#endif
}

void cli::txCompleteInterrupt() {
#ifndef generic
    USART2->CR1 = USART2->CR1 & ~USART_CR1_TCIE;
#endif
}

void cli::rxNotEmptyInterrupt() {
#ifndef generic
    uint8_t received_char = USART2->RDR;
    if (received_char == bootLoaderMagicValue) {
        jumpToBootLoader();
    } else {
        commandBuffer.push(received_char);
    }
    // if (received_char == '\r') {
    //     handleEvents();
    // }
#endif
}
