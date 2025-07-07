// ######################################################################################
// ### Author : Pascal Roobrouck  https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA  https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <stdint.h>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <logging.hpp>
#include <gpio.hpp>
#include <power.hpp>
#include <settingscollection.hpp>
#include <i2c.hpp>
#include <uart1.hpp>
#include <uart2.hpp>
#include <debugport.hpp>
#ifndef generic
#include "main.h"
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
#endif

uint32_t logging::activeSources{0};
uint32_t logging::activeDestinations{0};
char logging::buffer[bufferLength]{};

void logging::initialize() {
    enable(logging::destination::uart1);
    enable(logging::source::error);
    enable(logging::source::criticalError);
}


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

uint32_t logging::snprintf(const source aSource, const char *format, ...) {
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

void logging::write(const destination aDestination, const uint32_t dataLength) {
    switch (aDestination) {
        case destination::swo:
            debugPort::transmit(reinterpret_cast<const uint8_t *>(buffer), dataLength);
            break;

        case destination::uart2:
            uart2::transmit(reinterpret_cast<const uint8_t *>(buffer), dataLength);
            break;

        case destination::uart1:
            uart1::transmit(reinterpret_cast<const uint8_t *>(buffer), dataLength);
            break;

        default:
            break;
    }
}

uint32_t logging::snprintf(const destination aDestination, const char *format, ...) {
    uint32_t length{0};
    if (activeDestinations != 00) {
        va_list argList;
        va_start(argList, format);
        length = vsnprintf(buffer, bufferLength, format, argList);
        va_end(argList);
        write(aDestination, length);
    }
    return length;
}

void logging::write(const uint32_t dataLength) {
    if (isActive(destination::swo)) {
        write(destination::swo, dataLength);
    }
    if (isActive(destination::uart1)) {
        write(destination::uart1, dataLength);
    }
    if (isActive(destination::uart2)) {
        write(destination::uart2, dataLength);
    }
}

const char *toString(const logging::source aSource) {
    switch (aSource) {
        case logging::source::applicationEvents:
            return "applicationEvents";
        case logging::source::sensorEvents:
            return "sensorEvents";
        case logging::source::sensorData:
            return "sensorData";
        case logging::source::displayEvents:
            return "displayEvents";
        case logging::source::displayData:
            return "displayData";
        case logging::source::eepromData:
            return "eepromData";
        case logging::source::eepromEvents:
            return "eepromEvents";
        case logging::source::lorawanEvents:
            return "lorawanEvents";
        case logging::source::lorawanData:
            return "lorawanData";
        case logging::source::lorawanMac:
            return "lorawanMac";
        case logging::source::sx126xControl:
            return "sx126xControl";
        case logging::source::sx126xBufferData:
            return "sx126xBufferData";
        case logging::source::settings:
            return "settings";
        case logging::source::error:
            return "error";
        case logging::source::criticalError:
            return "criticalError";
        default:
            return "unknown";
    }
}

const char *toString(const logging::destination aDestination) {
    switch (aDestination) {
        case logging::destination::swo:
            return "swo";
        case logging::destination::uart2:
            return "uart2";
        case logging::destination::uart1:
            return "uart1";
        default:
            return "unknown";
    }
}
