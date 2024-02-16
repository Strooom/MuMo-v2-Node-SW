#include "main.h"

// Copy of STM32CubeMX generated code, so I can use it in every target unittest
// Original will be generated in the main.c file

void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}
