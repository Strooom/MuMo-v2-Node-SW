// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "power.hpp"

bool power::usbPower{false};

#ifndef generic
#include "main.h"
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

// NOTE : if this function is only called from the main thread, there is no need to check the PRIMASK state : 
//   in main thread, interrupts should be enabled all the time, except when accessing a resource shared with an ISR
// When there are no common resources accessed from different ISRs, there is also no point in disabling interrupts in the ISR
//   as the ISR itself cannot be interrupted by the same interrupt.
// --> check that goSleep() is only called from the main thread, not from ISR
// --> check that resources accessed from an ISR, are not accessed by any other ISR

void power::goSleep() {
#ifndef generic
    uint32_t currentPriMaskState = __get_PRIMASK();
    __disable_irq();
    HAL_SuspendTick();
    HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
    HAL_ResumeTick();
    __set_PRIMASK(currentPriMaskState);
#endif
}
