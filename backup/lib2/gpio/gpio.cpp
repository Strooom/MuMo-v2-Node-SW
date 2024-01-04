#include "gpio.h"

#ifndef generic
#include "main.h"
#else

#endif

void gpio::enableGpio(group aGroup) {
    enableDisableGpio(aGroup, true);
}

void gpio::disableGpio(group aGroup) {
    enableDisableGpio(aGroup, false);
}

void gpio::enableDisableGpio(group theGroup, bool enable) {
#ifndef generic
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    // __HAL_RCC_GPIOC_CLK_ENABLE(); // No GPIO from port C are being used fttb

    switch (theGroup) {
        case gpio::group::rfControl:
            if (enable) {
                HAL_GPIO_WritePin(GPIOA, rfControl1_Pin | rfControl2_Pin, GPIO_PIN_RESET);
                GPIO_InitTypeDef GPIO_InitStruct{0};
                // PA4     ------> rfControl1
                // PA5     ------> rfControl2
                HAL_GPIO_WritePin(GPIOA, rfControl1_Pin | rfControl2_Pin, GPIO_PIN_RESET);
                GPIO_InitStruct.Pin   = rfControl1_Pin | rfControl2_Pin;
                GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
                GPIO_InitStruct.Pull  = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
                HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
            } else {
                HAL_GPIO_DeInit(GPIOA, rfControl1_Pin | rfControl2_Pin);
            }
            break;

        case gpio::group::i2c:
            if (enable) {
                GPIO_InitTypeDef GPIO_InitStruct{0};
                // PA15     ------> I2C2_SDA
                GPIO_InitStruct.Pin       = I2C2_SDA_Pin;
                GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
                GPIO_InitStruct.Pull      = GPIO_NOPULL;
                GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
                GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
                HAL_GPIO_Init(I2C2_SDA_GPIO_Port, &GPIO_InitStruct);

                // PB15     ------> I2C2_SCL
                GPIO_InitStruct.Pin       = I2C2_SCL_Pin;
                GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
                GPIO_InitStruct.Pull      = GPIO_NOPULL;
                GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
                GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
                HAL_GPIO_Init(I2C2_SCL_GPIO_Port, &GPIO_InitStruct);
            } else {
                HAL_GPIO_DeInit(I2C2_SDA_GPIO_Port, I2C2_SDA_Pin);
                HAL_GPIO_DeInit(I2C2_SCL_GPIO_Port, I2C2_SCL_Pin);
            }
            break;

        case gpio::group::writeProtect:
            if (enable) {
                HAL_GPIO_WritePin(GPIOB, writeProtect_Pin, GPIO_PIN_SET);
                GPIO_InitTypeDef GPIO_InitStruct{0};
                // PB9     ------> writeProtect
                HAL_GPIO_WritePin(GPIOB, writeProtect_Pin, GPIO_PIN_SET);        // write protect is active low
                GPIO_InitStruct.Pin   = writeProtect_Pin;
                GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
                GPIO_InitStruct.Pull  = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
                HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
            } else {
                HAL_GPIO_DeInit(GPIOB, writeProtect_Pin);
            }
            break;

        case gpio::group::i2cSensors:
            enableDisableGpio(gpio::group::i2c, enable);
            break;

        case gpio::group::i2cEeprom:
            enableDisableGpio(gpio::group::i2c, enable);
            enableDisableGpio(gpio::group::writeProtect, enable);
            break;

        case gpio::group::spiDisplay:
            if (enable) {
                HAL_GPIO_WritePin(GPIOB, displayDataCommand_Pin | displayChipSelect_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOA, displayReset_Pin, GPIO_PIN_RESET);
                GPIO_InitTypeDef GPIO_InitStruct{0};

                // PB10     ------> displayBusy
                GPIO_InitStruct.Pin  = displayBusy_Pin;
                GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
                GPIO_InitStruct.Pull = GPIO_PULLDOWN;        // pulldown so line stays low when display is not present
                HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

                // PA0     ------> displayReset
                GPIO_InitStruct.Pin   = displayReset_Pin;
                GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
                GPIO_InitStruct.Pull  = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
                HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

                // PB14     ------> displayDataCommand
                // PB5     ------> displayChipSelect
                GPIO_InitStruct.Pin   = displayDataCommand_Pin | displayChipSelect_Pin;
                GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
                GPIO_InitStruct.Pull  = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
                HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

                // PA10     ------> SPI2_MOSI
                GPIO_InitStruct.Pin       = GPIO_PIN_10;
                GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull      = GPIO_NOPULL;
                GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
                GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
                HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

                // PB13     ------> SPI2_SCK
                GPIO_InitStruct.Pin       = GPIO_PIN_13;
                GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull      = GPIO_NOPULL;
                GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
                GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
                HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
            } else {
                HAL_GPIO_DeInit(GPIOA, displayReset_Pin | GPIO_PIN_10);
                HAL_GPIO_DeInit(GPIOB, displayBusy_Pin | displayDataCommand_Pin | displayChipSelect_Pin | GPIO_PIN_13);
            }
            break;

        case gpio::group::debugPort:
            if (enable) {
                // These need to be set as after reset...
            } else {
                HAL_GPIO_DeInit(GPIOA, GPIO_PIN_13 | GPIO_PIN_14);
                HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3);
            }
            break;

        case gpio::group::uart1:
            if (enable) {
                GPIO_InitTypeDef GPIO_InitStruct{0};
                // PB7     ------> USART1_RX
                // PB6     ------> USART1_TX
                GPIO_InitStruct.Pin       = GPIO_PIN_7 | GPIO_PIN_6;
                GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull      = GPIO_NOPULL;
                GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
                GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
                HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
            } else {
                HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7 | GPIO_PIN_6);
            }
            break;

        case gpio::group::uart2:
            if (enable) {
                GPIO_InitTypeDef GPIO_InitStruct{0};
                // PA3     ------> USART2_RX
                // PA2     ------> USART2_TX
                GPIO_InitStruct.Pin       = GPIO_PIN_3 | GPIO_PIN_2;
                GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull      = GPIO_NOPULL;
                GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
                GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
                HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
            } else {
                HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3 | GPIO_PIN_2);
            }
            break;

        case gpio::group::usbPresent:
            if (enable) {
                GPIO_InitTypeDef GPIO_InitStruct{0};
                // PB4     ------> usbPowerPresent
                GPIO_InitStruct.Pin  = usbPowerPresent_Pin;
                GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
                GPIO_InitStruct.Pull = GPIO_NOPULL;
                HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
            } else {
                HAL_GPIO_DeInit(GPIOB, usbPowerPresent_Pin);
            }
            break;

        default:
            // Error : unknown group
            break;
    }
#endif
}
