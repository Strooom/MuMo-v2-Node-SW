// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <sensortype.hpp>

class screen {
  public:
    static void show();

  private:
    static const char* getLastMeasurement(sensorChannelType);
    static constexpr uint32_t maxNumberOfLines{4};
    static sensorChannelType lines[maxNumberOfLines];
};