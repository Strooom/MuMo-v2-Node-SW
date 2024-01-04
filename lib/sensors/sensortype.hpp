// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>

// Note : the values below need to match with the values expected by the payloadDecoder on the application server

enum class sensorChannelType : uint32_t {
    none = 0x00,

    batteryVoltage     = 0x01,              // [mV]
    batteryChargeLevel = 0x02,              // [%] 0, 1-254, 255

    BME680Temperature        = 0x10,        // [°C]
    BME680RelativeHumidity   = 0x11,        // [%]
    BME680BarometricPressure = 0x12,        // hPa (millibar)

    TSL25911VisibleLight = 0x20,            // lux
    TSL25911Infrared     = 0x21,            // lux

    status = 0xE0,
    events = 0xF0
};

const char *toString(sensorChannelType aChannel);

