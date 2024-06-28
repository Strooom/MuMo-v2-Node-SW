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

bool spi::initialized{false};

void spi::wakeUp() {
    if (!initialized) {
#ifndef generic
        MX_SPI2_Init();
#endif
        gpio::enableGpio(gpio::group::spiDisplay);
        initialized = true;
    }
}

void spi::goSleep() {
    if (initialized) {
#ifndef generic
        HAL_SPI_DeInit(&hspi2);
#endif
        gpio::disableGpio(gpio::group::spiDisplay);
        initialized = false;
    }
}
