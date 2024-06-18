// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <spi.hpp>
#include <gpio.hpp>

#ifndef generic
#include "main.h"
extern SPI_HandleTypeDef hspi2;
void MX_SPI2_Init(void);
#endif

bool spi::active{false};

void spi::wakeUp() {
    if (!active) {
#ifndef generic
        MX_SPI2_Init();
#endif
        gpio::enableGpio(gpio::group::spiDisplay);
        active = true;
    }
}

void spi::goSleep() {
    if (active) {
#ifndef generic
        HAL_SPI_DeInit(&hspi2);
#endif
        gpio::disableGpio(gpio::group::spiDisplay);
        active = false;
    }
}
