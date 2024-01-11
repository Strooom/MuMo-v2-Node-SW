// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once

#include <stdint.h>

class sensorChannelFormat {
  public:
    static constexpr uint32_t maxNameLength{16};
    static constexpr uint32_t maxUnitLength{4};
    const char name[maxNameLength+1];
    const char unit[maxUnitLength + 1];
    const uint32_t decimals;                       // number of decimals to be shown on display
};