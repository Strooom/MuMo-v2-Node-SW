// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <power.hpp>
#include <gpio.hpp>

#ifndef generic
#include "main.h"
#else
bool power::mockUsbPower{false};
#endif

bool power::usbPower{false};

bool power::hasUsbPower() {
#ifndef generic
    gpio::enableGpio(gpio::group::usbPresent);
    bool pinState = (GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOB, usbPowerPresent_Pin));
    gpio::disableGpio(gpio::group::usbPresent);
    return pinState;
#else
    return mockUsbPower;
#endif
}

bool power::isUsbConnected() {
    bool newUsbPower = hasUsbPower();
    if (newUsbPower && !usbPower) {
        usbPower = true;
        return true;
    } else {
        return false;
    }
}

bool power::isUsbRemoved() {
    bool newUsbPower = hasUsbPower();
    if (!newUsbPower && usbPower) {
        usbPower = false;
        return true;
    } else {
        return false;
    }
}

