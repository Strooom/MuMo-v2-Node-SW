#include "gpio.hpp"

#ifndef generic
#include "main.h"
#else

#endif

bool gpio::isDebugProbePresent() {
#ifndef generic
    return ((CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) == 0x0001);
#else
    return false;
#endif
}

void gpio::initialize() {
}

void gpio::enableGpio(group aGroup) {
    enableDisableGpio(aGroup, true);
}

void gpio::disableGpio(group aGroup) {
    enableDisableGpio(aGroup, false);
}

void gpio::disableAllGpio() {
    enableDisableGpio(gpio::group::i2c, false);
    enableDisableGpio(gpio::group::writeProtect, false);
    enableDisableGpio(gpio::group::spiDisplay, false);
    enableDisableGpio(gpio::group::usbPresent, false);
    enableDisableGpio(gpio::group::uart1, false);
    enableDisableGpio(gpio::group::uart2, false);
    enableDisableGpio(gpio::group::rfControl, false);
    if (!isDebugProbePresent()) {
        enableDisableGpio(gpio::group::debugPort, false);
    }
}

void gpio::enableDisableGpio(group theGroup, bool enable) {
#ifndef generic
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    switch (theGroup) {
        case gpio::group::rfControl:
            // PA4     ------> rfControl1
            // PA5     ------> rfControl2
            if (enable) {
                HAL_GPIO_WritePin(GPIOA, rfControl1_Pin | rfControl2_Pin, GPIO_PIN_RESET);
                GPIO_InitTypeDef GPIO_InitStruct{0};
                HAL_GPIO_WritePin(GPIOA, rfControl1_Pin | rfControl2_Pin, GPIO_PIN_RESET);
                GPIO_InitStruct.Pin   = rfControl1_Pin | rfControl2_Pin;
                GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
                GPIO_InitStruct.Pull  = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
            } else {
                HAL_GPIO_DeInit(GPIOA, rfControl1_Pin | rfControl2_Pin);
            }
            break;

        case gpio::group::i2c:
            // PA15     ------> I2C2_SDA
            // PB15     ------> I2C2_SCL
            if (enable) {
                GPIO_InitTypeDef GPIO_InitStruct{0};
                GPIO_InitStruct.Pin       = I2C2_SDA_Pin;
                GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
                GPIO_InitStruct.Pull      = GPIO_NOPULL;
                GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
                GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
                HAL_GPIO_Init(I2C2_SDA_GPIO_Port, &GPIO_InitStruct);

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
            // PB9     ------> writeProtect
            if (enable) {
                HAL_GPIO_WritePin(GPIOB, writeProtect_Pin, GPIO_PIN_SET);
                GPIO_InitTypeDef GPIO_InitStruct{0};
                HAL_GPIO_WritePin(GPIOB, writeProtect_Pin, GPIO_PIN_SET);        // write protect is active low
                GPIO_InitStruct.Pin   = writeProtect_Pin;
                GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
                GPIO_InitStruct.Pull  = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
            } else {
                HAL_GPIO_DeInit(GPIOB, writeProtect_Pin);
            }
            break;

        case gpio::group::spiDisplay:
            if (enable) {
                // PB10     ------> displayBusy
                // PA0     ------> displayReset
                // PB14     ------> displayDataCommand
                // PB5     ------> displayChipSelect
                // PA10     ------> SPI2_MOSI
                // PB13     ------> SPI2_SCK
                HAL_GPIO_WritePin(GPIOB, displayDataCommand_Pin | displayChipSelect_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOA, displayReset_Pin, GPIO_PIN_RESET);
                GPIO_InitTypeDef GPIO_InitStruct{0};

                GPIO_InitStruct.Pin  = displayBusy_Pin;
                GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
                GPIO_InitStruct.Pull = GPIO_PULLDOWN;        // pulldown so line stays low when display is not present
                HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

                GPIO_InitStruct.Pin   = displayReset_Pin;
                GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
                GPIO_InitStruct.Pull  = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

                GPIO_InitStruct.Pin   = displayDataCommand_Pin | displayChipSelect_Pin;
                GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
                GPIO_InitStruct.Pull  = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

                GPIO_InitStruct.Pin       = GPIO_PIN_10;
                GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull      = GPIO_NOPULL;
                GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_MEDIUM;
                GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
                HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

                GPIO_InitStruct.Pin       = GPIO_PIN_13;
                GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull      = GPIO_NOPULL;
                GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_MEDIUM;
                GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
                HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
            } else {
                HAL_GPIO_DeInit(GPIOA, GPIO_PIN_10);
                HAL_GPIO_DeInit(GPIOB, displayBusy_Pin | GPIO_PIN_13);
                HAL_GPIO_WritePin(GPIOA, displayReset_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOB, displayDataCommand_Pin | displayChipSelect_Pin, GPIO_PIN_RESET);

                // HAL_GPIO_DeInit(GPIOA, GPIO_PIN_10 | displayReset_Pin);
                // HAL_GPIO_DeInit(GPIOB, displayBusy_Pin | displayDataCommand_Pin | displayChipSelect_Pin | GPIO_PIN_13);
            }
            break;

        case gpio::group::debugPort:
            if (enable) {
                // These need to be set as after reset...
            } else {
                // NOTE : This resets the SWD AND JTAG pins to analog inputs (lowest power consumption).
                // We don't use JTAG, but we assigned PIN PB4 another function (usbPowerPresent). Disabling all debug pins, also disables PB4 as usbPowerPresent.
                HAL_GPIO_DeInit(GPIOA, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
                HAL_GPIO_DeInit(GPIOB, GPIO_PIN_4);
                // HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3);
            }
            break;

        case gpio::group::uart1:
            // PB7     ------> USART1_RX : Note : this is not used so could be disabled
            // PB6     ------> USART1_TX
            if (enable) {
                GPIO_InitTypeDef GPIO_InitStruct{0};
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
            // PA3     ------> USART2_RX
            // PA2     ------> USART2_TX
            if (enable) {
                GPIO_InitTypeDef GPIO_InitStruct{0};
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
            // PB4     ------> usbPowerPresent
            if (enable) {
                GPIO_InitTypeDef GPIO_InitStruct{0};
                GPIO_InitStruct.Pin  = usbPowerPresent_Pin;
                GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
                GPIO_InitStruct.Pull = GPIO_PULLDOWN;
                HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
            } else {
                HAL_GPIO_DeInit(GPIOB, usbPowerPresent_Pin);
            }
            break;

        case gpio::group::enableDisplayPower: {
            // PA9 = Wio-E5 pin 21 - charge the display power rail without a current peak, prevents brownout of the MCU
            // PC0 = Wio-E5 pin 13 - switches the 3.3V towards the epaper display
            GPIO_InitTypeDef GPIO_InitStruct{0};
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);

            GPIO_InitStruct.Pin   = GPIO_PIN_9;
            GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
            GPIO_InitStruct.Pull  = GPIO_NOPULL;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
            HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

            GPIO_InitStruct.Pin   = GPIO_PIN_0;
            GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
            GPIO_InitStruct.Pull  = GPIO_NOPULL;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
            HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

            if (enable) {
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
                HAL_Delay(50);
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
            } else {
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
            }
        } break;

        case gpio::group::enableSensorsEepromPower:
            // PC1 = Wio-E5 pin 12 - switches the 3.3V towards sensors and eeprom
            if (enable) {
                GPIO_InitTypeDef GPIO_InitStruct{0};
                GPIO_InitStruct.Pin   = GPIO_PIN_1;
                GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
                GPIO_InitStruct.Pull  = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
            } else {
                HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1);
            }
            break;

        case gpio::group::other:
            // PB0 = Wio-E5 pin 28 - TestPin used during debugging and power measurements
            if (enable) {
                GPIO_InitTypeDef GPIO_InitStruct{0};
                GPIO_InitStruct.Pin   = GPIO_PIN_0;
                GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
                GPIO_InitStruct.Pull  = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
            } else {
                HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0);
            }
            break;

        default:
            break;
    }
#endif
}
