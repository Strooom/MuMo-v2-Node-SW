// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <stdint.h>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include "logging.hpp"

#ifndef generic
#include "main.h"
extern UART_HandleTypeDef huart2;
#endif

uint32_t logging::activeSources{0};
uint32_t logging::activeDestinations{0};
char logging::buffer[bufferLength]{};

uint32_t logging::snprintf(const char *format, ...) {
    uint32_t length{0};
    if (activeDestinations != 00) {
        va_list argList;
        va_start(argList, format);
        length = vsnprintf(buffer, bufferLength, format, argList);
        va_end(argList);
        write(length);
    }
    return length;
}

uint32_t logging::snprintf(source aSource, const char *format, ...) {
    uint32_t length{0};
    if (activeDestinations != 00) {
        if (isActive(aSource)) {
            va_list argList;
            va_start(argList, format);
            length = vsnprintf(buffer, bufferLength, format, argList);
            va_end(argList);
            write(length);
        }
    }
    return length;
}

void logging::write(uint32_t dataLength) {
    if (isActive(destination::debugProbe)) {
        for (uint32_t index = 0; index < dataLength; index++) {
#ifndef generic
            ITM_SendChar(static_cast<uint32_t>(buffer[index]));
#endif
        }
    }
    if (isActive(destination::uart)) {
#ifndef generic
        HAL_UART_Transmit(&huart2, (uint8_t *)buffer, dataLength, 1000);
#endif
    }
}
