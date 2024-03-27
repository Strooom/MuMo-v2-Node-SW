#include "main.h"
#include <circularbuffer.hpp>
#include <applicationevent.hpp>

// Copy of STM32CubeMX generated code, so I can use it in every target unittest
// Original will be generated in the main.c file

SUBGHZ_HandleTypeDef hsubghz;
extern circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void MX_SUBGHZ_Init(void) {
    hsubghz.Init.BaudratePrescaler = SUBGHZSPI_BAUDRATEPRESCALER_2;
    if (HAL_SUBGHZ_Init(&hsubghz) != HAL_OK) {
        Error_Handler();
    }
}

void HAL_SUBGHZ_TxCpltCallback(SUBGHZ_HandleTypeDef *hsubghz) {
    applicationEventBuffer.push(applicationEvent::sx126xTxComplete);
}

void HAL_SUBGHZ_RxCpltCallback(SUBGHZ_HandleTypeDef *hsubghz) {
    applicationEventBuffer.push(applicationEvent::sx126xRxComplete);
}

void HAL_SUBGHZ_RxTxTimeoutCallback(SUBGHZ_HandleTypeDef *hsubghz) {
    applicationEventBuffer.push(applicationEvent::sx126xTimeout);
}