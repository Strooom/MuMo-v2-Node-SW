#include "power.h"

bool power::usbPower{false};

#ifndef generic
#include "main.h"
// extern ADC_HandleTypeDef hadc; To be used for measuring the batteryvoltage
#else
bool power::mockUsbPower{false};
#endif

bool power::hasUsbPower() {
#ifndef generic
    return (GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOB, usbPowerPresent_Pin));
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
