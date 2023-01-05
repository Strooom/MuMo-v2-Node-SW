
#pragma once
#include <stdint.h>

enum class infoChannelType : uint32_t {
    batteryLevel = 0x00,

    mcuTemperature          = 0x08,
    displayTemperature      = 0x09,
    BME680SensorTemperature = 0x0A,

    BME680SensorRelativeHumidity = 0x10,

    BME680SensorBarometricPressure = 0x18,

    TSL25911SensorLightIntensity = 0x20,

    status = 0xE0,
    events = 0xF0
};


const char* toString(const infoChannelType aChannel);