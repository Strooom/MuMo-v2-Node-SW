// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once

#include <cstdint>
#include <batterytype.hpp>
#include <sensordevicestate.hpp>
#include <sensorchannel.hpp>

class battery {
  public:
    static void initalize();
    static void run();
    static sensorDeviceState getState() { return state; };
    static constexpr uint32_t nmbrChannels{2};
    static constexpr uint32_t voltage{0};
    static constexpr uint32_t percentCharged{1};
    static batteryType type;

#ifndef unitTesting

  private:
#endif
    static sensorDeviceState state;
    static void startSampling();
    static bool samplingIsReady();
    static uint32_t readSample();
    static float voltageFromRaw(uint32_t rawADC);
    static sensorChannel channels[nmbrChannels];

    friend class sensorDeviceCollection;
};
