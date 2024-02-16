#include "main.h"

// Copy of STM32CubeMX generated code, so I can use it in every target unittest
// Original will be generated in the main.c file

RNG_HandleTypeDef hrng;

static void MX_RNG_Init(void) {
    /* USER CODE BEGIN RNG_Init 0 */

    /* USER CODE END RNG_Init 0 */

    /* USER CODE BEGIN RNG_Init 1 */

    /* USER CODE END RNG_Init 1 */
    hrng.Instance                 = RNG;
    hrng.Init.ClockErrorDetection = RNG_CED_DISABLE;
    if (HAL_RNG_Init(&hrng) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN RNG_Init 2 */

    /* USER CODE END RNG_Init 2 */
}
