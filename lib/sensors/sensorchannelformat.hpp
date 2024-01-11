// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once

#include <stdint.h>

class sensorChannelFormat {
  public:
    static constexpr uint32_t textLength{8};
    const char name[textLength];
    const char fullFormat[textLength];          // format string to derive the text output for eg. loggging. Example "%.1f °C" resulting in "20.5 °C"
    const char baseFormat[textLength];          // format string to derive the text to be shown on display in big font. Example "%.0f" resulting in "20"
    const char suffixFormat[textLength];        // format string to derive the text to be shown on display in small font. Example "%0.1f°C" resulting in ".5°C"
};