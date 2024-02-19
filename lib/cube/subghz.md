1-The default HAL_SUBGHZ_IRQHandler() clears the interrupt register at the END. Then When an Interrupt call back process set the Radio to sleep, the last instruction that clears the interrupt make an SPI access to the RF part hence set it in standby mode !

2-Clear the interrupt CR register juste after having read it at the BEGINING of the interrupt solve this. I just wanted an "opinion" about this palliative AND maybe that it could be taken in account for a next version of HAL_SUBGHZ ?

https://forum.digikey.com/t/sub-ghz-radio-interrupts-on-the-stm32wl-series/19668