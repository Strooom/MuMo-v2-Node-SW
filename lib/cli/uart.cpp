// #include "uart.hpp"

// #ifndef generic
// #include "main.h"
// extern UART_HandleTypeDef huart2;

// #else
// #endif

// circularBuffer<uint8_t, cli::commandBufferLength> cli::commandBuffer;
// circularBuffer<uint8_t, cli::responseBufferLength> cli::responseBuffer;


// void cli::startTx() {
//     if (!commandBuffer.isEmpty()) {
//     #ifndef generic
//         bool interrupts_enabled = (__get_PRIMASK() == 0);
//         __disable_irq();
//         USART2->CR1 = USART2->CR1 | USART_CR1_TXEIE_TXFNFIE;
//         if (interrupts_enabled) {
//             __enable_irq();
//         }


//     #endif
// }
// }

// void cli::handleRxEvent() {
// #ifndef generic
//     uint8_t received_char;
//     if (HAL_UART_Receive(&huart2, &received_char, 1, 0) == HAL_OK) {
//         commandBuffer.push(received_char);
//         if (received_char == '\r') {
//             handleEvents();
//         }
//         if (received_char == bootLoaderMagicValue) {
//             jumpToBootLoader();
//         }
//     }
// #endif
// }

// void cli::handleTxEvent() {
// #ifndef generic
//     uint8_t received_char;
//     if (HAL_UART_Receive(&huart2, &received_char, 1, 0) == HAL_OK) {
//         commandBuffer.push(received_char);
//         if (received_char == '\r') {
//             handleEvents();
//         }
//         if (received_char == bootLoaderMagicValue) {
//             jumpToBootLoader();
//         }
//     }
// #endif
// }

// void cli::handleEvents() {
// }




// void cli::txEmptyInterrupt() {
// #ifndef generic
//     if (responseBuffer.isEmpty()) {
//         USART2->CR1 = USART2->CR1 & ~USART_CR1_TXEIE_TXFNFIE;
//     } else {
//         USART2->TDR = responseBuffer.pop();
//     }
// #endif
// }

// void cli::txCompleteInterrupt() {
// #ifndef generic
//     USART2->CR1 = USART2->CR1 & ~USART_CR1_TCIE;
// #endif
// }

// void cli::rxNotEmptyInterrupt() {
// #ifndef generic
//     uint8_t received_char = USART2->RDR;
//     if (received_char == bootLoaderMagicValue) {
//         jumpToBootLoader();
//     } else {
//         commandBuffer.push(received_char);
//     }
//     // if (received_char == '\r') {
//     //     handleEvents();
//     // }
// #endif
// }
