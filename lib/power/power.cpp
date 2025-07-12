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
uint32_t power::usbPowerOffCount{usbPowerOffCountMax};

bool power::hasUsbPower() {
#ifndef generic
    bool result = (GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOB, usbPowerPresent_Pin));
    if (result) {
        usbPowerOffCount = usbPowerOffCountMax;
    } else {
        if (usbPowerOffCount > 0) {
            usbPowerOffCount--;
        }
    }
    return result;
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
