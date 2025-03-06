// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <sensordevicetype.hpp>
#include <sensorchannel.hpp>

class sensorDeviceCollection {
  public:
    sensorDeviceCollection() = delete;
    static void discover();
    static void set(uint32_t deviceIndex, uint32_t channelIndex, uint32_t oversampling, uint32_t prescaler);
    static void startSampling();
    static void run();
    static void log();
    static void log(uint32_t deviceIndex);
    static void log(uint32_t deviceIndex, uint32_t channelIndex);
    static bool isSamplingReady();

    static float value(uint32_t deviceIndex, uint32_t channelIndex);
    static uint32_t decimals(uint32_t deviceIndex, uint32_t channelIndex);
    static const char* name(uint32_t deviceIndex);
    static const char* name(uint32_t deviceIndex, uint32_t channelIndex);
    static const char* units(uint32_t deviceIndex, uint32_t channelIndex);

    static uint32_t nmbrOfChannels(uint32_t deviceIndex);
    static sensorChannel& channel(uint32_t deviceIndex, uint32_t channelIndex);

    static bool needsSampling();
    static bool needsSampling(uint32_t deviceIndex);
    static bool needsSampling(uint32_t deviceIndex, uint32_t channelIndex);
    static void updateCounters();
    static void updateCounters(uint32_t deviceIndex);
    static void updateCounters(uint32_t deviceIndex, uint32_t channelIndex);
    static bool hasNewMeasurements();
    static bool hasNewMeasurements(uint32_t deviceIndex);
    static bool hasNewMeasurement(uint32_t deviceIndex, uint32_t channelIndex);
    static uint32_t nmbrOfNewMeasurements();
    static uint32_t nmbrOfNewMeasurements(uint32_t deviceIndex);
    static void collectNewMeasurements();
    static void collectNewMeasurements(uint32_t deviceIndex);
    static void clearNewMeasurements();
    static void clearNewMeasurements(uint32_t deviceIndex);

    static bool isValid(uint32_t deviceIndex);
    static bool isValid(uint32_t deviceIndex, uint32_t channelIndex);

    static bool isPresent[static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices)];
#ifndef unitTesting

  private:
#endif

    static sensorChannel dummy;        // dummy, so we can return a valid reference to a channel in edge cases
};