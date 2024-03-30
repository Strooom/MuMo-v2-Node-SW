#include "unity_config.h"
#include "main.h"

#ifdef TARGET_TEST_PORT_UART1
extern UART_HandleTypeDef huart1;
void MX_USART1_UART_Init(void);
void unityOutputStart() {
    MX_USART1_UART_Init();
}
void unityOutputChar(char c) {
    HAL_UART_Transmit(&huart1, (uint8_t *)(&c), 1, 1000);
}
#endif
#ifdef TARGET_TEST_PORT_UART2
extern UART_HandleTypeDef huart2;
void MX_USART2_UART_Init(void);
void unityOutputStart() {
    MX_USART2_UART_Init();
}
void unityOutputChar(char c) {
    HAL_UART_Transmit(&huart2, (uint8_t *)(&c), 1, 1000);
}
#endif

void unityOutputFlush() {}

void unityOutputComplete() {}