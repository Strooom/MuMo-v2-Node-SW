This repository will hold the firmware for the MuMo node, a LoRa-based environmental monitor for museum collection items.
The hardware can be found here : https://github.com/Strooom/MuMo-V2-Node-PCB

More information can be found in the Wiki:

https://github.com/Strooom/MuMo-v2-Node-SW/wiki

# Progress indicator
* 2023/01/09 : managed to read the unique 96-bit device ID through stm32wlxx_ll_utils.h | uint32_t LL_GetUID_Wordx(void); (x=0,1,2)
* 2023/01/09 : got ADC working, using DMA - measuring vBat and MCU temperature. Still need to find out how to convert raw ADC values to meaningfull voltage and temperature
* 2023/01/05 : got I2C working - an I2C scan on the Wio-E5 development board
* 2023/01/05 : got UART working, transmitting to serial monitor
* 2023/01/05 : got development environment working, with simple Blink application

