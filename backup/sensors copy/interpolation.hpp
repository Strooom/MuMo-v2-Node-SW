// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

// TODO : this should be the base class for all actual sensors (eg. battery, bme680, tsl2591, ...) to enforce a common minimum interface

#pragma once
#include <stdint.h>

struct interpolationPoint {
    float voltage;         // [mV]
    uint8_t charge;        // [%, 0-255]
};
