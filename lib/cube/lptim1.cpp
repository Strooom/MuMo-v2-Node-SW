#include "main.h"

// Copy of STM32CubeMX generated code, so I can use it in every target unittest
// Original will be generated in the main.c file

LPTIM_HandleTypeDef hlptim1;

void MX_LPTIM1_Init(void) {
    hlptim1.Instance             = LPTIM1;
    hlptim1.Init.Clock.Source    = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;        // 32.768 kHz XTAL
    hlptim1.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV8;                    // 4096 Hz
    hlptim1.Init.Trigger.Source  = LPTIM_TRIGSOURCE_SOFTWARE;
    hlptim1.Init.OutputPolarity  = LPTIM_OUTPUTPOLARITY_HIGH;
    hlptim1.Init.UpdateMode      = LPTIM_UPDATE_IMMEDIATE;
    hlptim1.Init.CounterSource   = LPTIM_COUNTERSOURCE_INTERNAL;
    hlptim1.Init.Input1Source    = LPTIM_INPUT1SOURCE_GPIO;
    hlptim1.Init.Input2Source    = LPTIM_INPUT2SOURCE_GPIO;
    if (HAL_LPTIM_Init(&hlptim1) != HAL_OK) {
        Error_Handler();
    }
}

