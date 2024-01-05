// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once

#include <stdint.h>
#include <sensordevicestate.hpp>
#include <sensorchannel.hpp>
#include <batterytype.hpp>

class battery {
  public:
    static void initalize();
    static sensorDeviceState getState() { return state; };
    static float getLastChannelValue(uint32_t index);
    static void tick();
    static void run();

    static constexpr uint32_t nmbrChannels{2};
    static constexpr uint32_t voltage{0};
    static constexpr uint32_t percentCharged{1};
    static sensorChannel channels[nmbrChannels];

#ifndef unitTesting

  private:
#endif
    static sensorDeviceState state;
    static bool anyChannelNeedsSampling();
    static void startSampling();
    static bool samplingIsReady();
    static uint32_t readSample();
    static batteryType type;
};
