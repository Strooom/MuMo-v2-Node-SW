// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <i2c.hpp>
#include <gpio.hpp>

#ifndef generic
#include "main.h"
extern I2C_HandleTypeDef hi2c2;
void MX_I2C2_Init(void);
#endif

bool i2c::initalized{false};

void i2c::wakeUp() {
    if (!initalized) {
#ifndef generic
        MX_I2C2_Init();
#endif
        gpio::enableGpio(gpio::group::i2c);
        gpio::enableGpio(gpio::group::writeProtect);
        initalized = true;
    }
}

void i2c::goSleep() {
    if (initalized) {
#ifndef generic
        HAL_I2C_DeInit(&hi2c2);
#endif
        gpio::disableGpio(gpio::group::i2c);
        gpio::disableGpio(gpio::group::writeProtect);
        initalized = false;
    }
}
