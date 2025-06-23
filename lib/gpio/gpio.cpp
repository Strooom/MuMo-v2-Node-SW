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
#ifndef generic
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
#endif
}

void gpio::enableGpio(group theGroup) {
#ifndef generic
    GPIO_InitTypeDef GPIO_InitStruct{0};
    switch (theGroup) {
        case gpio::group::rfControl:
            // PA4     ------> rfControl1
            // PA5     ------> rfControl2
            HAL_GPIO_WritePin(GPIOA, rfControl1_Pin | rfControl2_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOA, rfControl1_Pin | rfControl2_Pin, GPIO_PIN_RESET);
            GPIO_InitStruct.Pin   = rfControl1_Pin | rfControl2_Pin;
            GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
            GPIO_InitStruct.Pull  = GPIO_NOPULL;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
            HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
            break;

        case gpio::group::i2c:
            // PA15     ------> I2C2_SDA
            // PB15     ------> I2C2_SCL
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
            break;

        case gpio::group::writeProtect:
            // PB9     ------> writeProtect
            HAL_GPIO_WritePin(GPIOB, writeProtect_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, writeProtect_Pin, GPIO_PIN_SET);        // write protect is active low
            GPIO_InitStruct.Pin   = writeProtect_Pin;
            GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
            GPIO_InitStruct.Pull  = GPIO_NOPULL;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
            HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
            break;

        case gpio::group::spiDisplay:
            // PB10     ------> displayBusy
            // PA0     ------> displayReset
            // PB14     ------> displayDataCommand
            // PB5     ------> displayChipSelect
            // PA10     ------> SPI2_MOSI
            // PB13     ------> SPI2_SCK
            HAL_GPIO_WritePin(GPIOB, displayDataCommand_Pin | displayChipSelect_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOA, displayReset_Pin, GPIO_PIN_RESET);

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
            break;

        case gpio::group::debugPort:
            break;

        case gpio::group::uart1:
            // PB7     ------> USART1_RX : Note : this is not used so could be disabled
            // PB6     ------> USART1_TX
            GPIO_InitStruct.Pin       = GPIO_PIN_7 | GPIO_PIN_6;
            GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Pull      = GPIO_NOPULL;
            GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
            GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
            HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
            break;

        case gpio::group::uart2:
            // PA3     ------> USART2_RX
            // PA2     ------> USART2_TX
            GPIO_InitStruct.Pin       = GPIO_PIN_3 | GPIO_PIN_2;
            GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Pull      = GPIO_NOPULL;
            GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
            GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
            HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
            break;

        case gpio::group::usbPresent:
            // PB4     ------> usbPowerPresent
            GPIO_InitStruct.Pin  = usbPowerPresent_Pin;
            GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
            GPIO_InitStruct.Pull = GPIO_PULLDOWN;
            HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
            break;

        case gpio::group::enableDisplayPower:
// PA9 = Wio-E5 pin 21 - charge the display power rail without a current peak, prevents brownout of the MCU
// PC0 = Wio-E5 pin 13 - switches the 3.3V towards the epaper display
// v2 hardware does not have these pins, only applicable for v3 hardware
#ifdef v3
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

            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
            HAL_Delay(50);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
#endif
            break;

        case gpio::group::enableSensorsEepromPower:
            // PC1 = Wio-E5 pin 12 - switches the 3.3V towards sensors and eeprom
            GPIO_InitStruct.Pin   = GPIO_PIN_1;
            GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
            GPIO_InitStruct.Pull  = GPIO_NOPULL;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
            HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
            break;

        case gpio::group::test0:
            // PB0 = Wio-E5 pin 28 - TestPin used during debugging and power measurements
            GPIO_InitStruct.Pin   = GPIO_PIN_0;
            GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
            GPIO_InitStruct.Pull  = GPIO_NOPULL;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
            HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
            break;

        default:
            break;
    }
#endif
}

void gpio::disableGpio(group theGroup) {
#ifndef generic
    switch (theGroup) {
        case gpio::group::rfControl:
            // PA4     ------> rfControl1
            // PA5     ------> rfControl2
            // Powering down these pins has no effect on power consumption, so we keep them enabled : HAL_GPIO_DeInit(GPIOA, rfControl1_Pin | rfControl2_Pin);
            break;

        case gpio::group::i2c:
            // PA15     ------> I2C2_SDA
            // PB15     ------> I2C2_SCL
            HAL_GPIO_DeInit(I2C2_SDA_GPIO_Port, I2C2_SDA_Pin);
            HAL_GPIO_DeInit(I2C2_SCL_GPIO_Port, I2C2_SCL_Pin);
            break;

        case gpio::group::writeProtect:
            // PB9     ------> writeProtect
            // Powering down these pin has no effect on power consumption, so we keep them enabled : HAL_GPIO_DeInit(GPIOB, writeProtect_Pin);
            break;

        case gpio::group::spiDisplay:
            // V2 hardware cannot remove power from the display, so we must keep at least the reset pin enabled. A floating reset pin will reset the display and wakes it up from its deep sleep, increasing power.
            // V3 hardware removes power, so we need to disable all output pins (or put them to GND), otherwise the display will draw leaking current from these pins.
#ifdef v3
            HAL_GPIO_DeInit(GPIOA, displayReset_Pin);        // PA0     ------> displayReset
            HAL_GPIO_DeInit(GPIOA, GPIO_PIN_10);             // PA10     ------> SPI2_MOSI
            HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13);             // PB13     ------> SPI2_SCK
            HAL_GPIO_DeInit(GPIOB, GPIO_PIN_14);             // PB14     ------> displayDataCommand
            HAL_GPIO_DeInit(GPIOB, GPIO_PIN_5);              // PB5      ------> displayChipSelect
#else
            HAL_GPIO_WritePin(GPIOA, displayReset_Pin, GPIO_PIN_SET);        // PA0     ------> displayReset - keep high to prevent display reset which wakes it up
#endif
            // displayBusy is an input an increases current consumption with 100 uA when enabled
            // All other IOs for display are outputs and apparently increase current consumption with ~15 uA when disabled, so in V2 we keep them enabled
            HAL_GPIO_DeInit(GPIOB, displayBusy_Pin);        // PB10     ------> displayBusy
            break;

        case gpio::group::debugPort:
            // NOTE : SWD AND JTAG pins are enabled after reset. In order to reduce power consumption, we reset them to analog inputs
            // We don't use JTAG so we assigned PIN PB4 another function : usbPowerPresent. For this reason, we don't modify this pin here.
            // PA13 = Wio-E5 pin 3 = SWDIO
            // PA14 = Wio-E5 pin 4 = SWCLK
            // PB3 = Wio-E5 pin 8 = SWO
            // Note that these 3 pins also double as inputs from buttons
            HAL_GPIO_DeInit(GPIOA, GPIO_PIN_13 | GPIO_PIN_14);
            HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3);
            break;

        case gpio::group::uart1:
            // PB7     ------> USART1_RX : Note : this is not used so could be disabled
            // PB6     ------> USART1_TX
            HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7 | GPIO_PIN_6);
            break;

        case gpio::group::uart2:
            // PA3     ------> USART2_RX
            // PA2     ------> USART2_TX
            HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3 | GPIO_PIN_2);
            break;

        case gpio::group::usbPresent:
            // PB4     ------> usbPowerPresent
            // Powering down these pins has no effect on power consumption, so we keep them enabled  : HAL_GPIO_DeInit(GPIOB, usbPowerPresent_Pin);
            break;

        case gpio::group::enableDisplayPower:
// PA9 = Wio-E5 pin 21 - charge the display power rail without a current peak, prevents brownout of the MCU
// PC0 = Wio-E5 pin 13 - switches the 3.3V towards the epaper display
// We do not disable these pins but rather drive it high in order to keep power off from this rail
#ifdef v3
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
#endif

            // HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9);
            // HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0);
            break;

        case gpio::group::enableSensorsEepromPower:
            // PC1 = Wio-E5 pin 12 - switches the 3.3V towards sensors and eeprom
            // We do not disable this pin but rather drive it high in order to keep power off from this rail
            // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
            HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1);

            break;

        case gpio::group::test0:
            // PB0 = Wio-E5 pin 28 - TestPin used during debugging and power measurements
            HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0);
            break;

        default:
            break;
    }
#endif
}
