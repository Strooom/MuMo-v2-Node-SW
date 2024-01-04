// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>

enum class sensorDeviceType : uint32_t {
    battery,
    mcu,
    bme680,
    bme688,
    tsl2591,
    sht40,
    veml7700,
    lis3dh,
    sths34,
    nmbrOfKnownDevices

};

const char* toString(sensorDeviceType theType);