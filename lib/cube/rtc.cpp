#include "main.h"

// Copy of STM32CubeMX generated code, so I can use it in every target unittest
// Original will be generated in the main.c file

RTC_HandleTypeDef hrtc;

 void MX_RTC_Init(void) {
    hrtc.Instance            = RTC;
    hrtc.Init.HourFormat     = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv   = 127;
    hrtc.Init.SynchPrediv    = 255;
    hrtc.Init.OutPut         = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutRemap    = RTC_OUTPUT_REMAP_NONE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;
    hrtc.Init.OutPutPullUp   = RTC_OUTPUT_PULLUP_NONE;
    hrtc.Init.BinMode        = RTC_BINARY_NONE;
    if (HAL_RTC_Init(&hrtc) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 61439, RTC_WAKEUPCLOCK_RTCCLK_DIV16, 0) != HAL_OK) {
        Error_Handler();
    }


}
