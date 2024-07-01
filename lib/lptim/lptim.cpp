// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <lptim.hpp>

#ifndef generic
#include "main.h"
extern LPTIM_HandleTypeDef hlptim1;
void MX_LPTIM1_Init(void);
#endif

bool lptim::running{false};

void lptim::initialize() {
#ifndef generic
    MX_LPTIM1_Init();
#endif
}

void lptim::start(uint32_t timeOutIn4096HzTicks) {
    running = true;
#ifndef generic
    HAL_LPTIM_SetOnce_Start_IT(&hlptim1, 0xFFFF, timeOutIn4096HzTicks);
#endif
}

void lptim::stop() {
    running = false;
#ifndef generic
    HAL_LPTIM_SetOnce_Stop_IT(&hlptim1);
#endif
}

uint32_t lptim::ticksFromSeconds(uint32_t seconds) {
    return (seconds * 4096U) - timerOffset;
}