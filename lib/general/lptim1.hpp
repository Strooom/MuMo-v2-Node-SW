// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

static constexpr uint32_t timerOffset{128U};        // offset to get timing of LPTIM1 accurate, compensating for start/stop overhead

uint32_t ticksFromSeconds(uint32_t seconds) {
    return (seconds * 4096U) - timerOffset;
}