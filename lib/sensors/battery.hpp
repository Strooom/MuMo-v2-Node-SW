// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once

#include <stdint.h>
#include <sensordevicestate.hpp>
#include <sensorchannel.hpp>
#include <sensorchannelformat.hpp>
#include <batterytype.hpp>

class battery {
  public:
    static void initalize();
    static sensorDeviceState getState() { return state; };
    static bool hasNewMeasurement();
    static float valueAsFloat(uint32_t channelIndex);

    static void tick();
    static void run();
    static void log();

    static constexpr uint32_t nmbrChannels{2};
    static constexpr uint32_t voltage{0};
    static constexpr uint32_t percentCharged{1};
    static sensorChannel channels[nmbrChannels];
    static sensorChannelFormat channelFormats[nmbrChannels];

#ifndef unitTesting

  private:
#endif
    static sensorDeviceState state;
    static bool anyChannelNeedsSampling();
    static void adjustAllCounters();
    static void startSampling();
    static bool samplingIsReady();
    static uint32_t readSample();
    static void clearNewMeasurements();
    static float voltageFromRaw(uint32_t rawADC);

    static batteryType type;
};
