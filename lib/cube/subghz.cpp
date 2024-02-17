#include "main.h"

// Copy of STM32CubeMX generated code, so I can use it in every target unittest
// Original will be generated in the main.c file

SUBGHZ_HandleTypeDef hsubghz;


static void MX_SUBGHZ_Init(void) {
    hsubghz.Init.BaudratePrescaler = SUBGHZSPI_BAUDRATEPRESCALER_2;
    if (HAL_SUBGHZ_Init(&hsubghz) != HAL_OK) {
        Error_Handler();
    }
}
