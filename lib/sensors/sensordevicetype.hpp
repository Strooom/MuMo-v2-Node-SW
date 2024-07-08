// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>

enum class sensorDeviceType : uint32_t {
    mcu,
    battery,
    bme680,
    tsl2591,
    sht40,
    lis3dh,
    sths34,
    scd40,
    sps30,
    xa1110,
    nmbrOfKnownDevices
};

const char *toString(sensorDeviceType theDevice);