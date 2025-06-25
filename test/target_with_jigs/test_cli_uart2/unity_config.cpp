#include "unity_config.h"
#include "main.h"

extern UART_HandleTypeDef huart2;
void MX_USART2_UART_Init(void);

void unityOutputStart() {
    MX_USART2_UART_Init();
}

void unityOutputChar(char c) {
    HAL_UART_Transmit(&huart2, (uint8_t *)(&c), 1, 1000);
}

void unityOutputFlush() {}

void unityOutputComplete() {
}