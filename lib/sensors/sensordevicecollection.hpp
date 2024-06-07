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
    static void tick();
    static void run();
    static void log();
    static void log(uint32_t deviceIndex);
    static void log(uint32_t deviceIndex, uint32_t channelIndex);
    static bool isSleeping();
    static const char* name(uint32_t index);

    static float channelValue(uint32_t deviceIndex, uint32_t channelIndex);
    static uint32_t channelDecimals(uint32_t deviceIndex, uint32_t channelIndex);
    static const char* channelName(uint32_t deviceIndex, uint32_t channelIndex);
    static const char* channelUnits(uint32_t deviceIndex, uint32_t channelIndex);

    static uint32_t nmbrOfChannels(uint32_t deviceIndex);
    static sensorChannel& channel(uint32_t deviceIndex, uint32_t channelIndex);

    static bool hasNewMeasurements();
    static bool hasNewMeasurements(uint32_t deviceIndex);
    static uint32_t nmbrOfNewMeasurements();
    static uint32_t nmbrOfNewMeasurements(uint32_t deviceIndex);
    static void collectNewMeasurements();
    static void collectNewMeasurements(uint32_t deviceIndex);
    static void clearNewMeasurements();
    static void clearNewMeasurements(uint32_t deviceIndex);

    static bool isValidDeviceIndex(uint32_t deviceIndex);
    static bool isValidChannelIndex(uint32_t deviceIndex, uint32_t channelIndex);

#ifndef unitTesting

  private:
#endif

    static bool isPresent[static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices)];
    static uint32_t actualNumberOfDevices;        // TODO : it seems I am not using this at all and could as well calculate it on the fly
    static sensorChannel dummy;                   // dummy, so we can return a valid reference to a channel in edge cases
};