// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <sensordevicetype.hpp>

class sensorDeviceCollection {
  public:
    sensorDeviceCollection() = delete;
    static void discover();                   // discover all sensor devices that are present by scanning the I2C addresses and checking register-values
    static void tick();                       // service all sensor devices after a RTC tick
    static void run();                        // service all sensor devices until they are back to sleep
    static void log();
    static bool isSleeping();
    static const char* name(uint32_t index);

    static float valueAsFloat(uint32_t deviceIndex, uint32_t channelIndex);
    static uint32_t channelDecimals(uint32_t deviceIndex, uint32_t channelIndex);
    static const char* channelName(uint32_t deviceIndex, uint32_t channelIndex);
    static const char* channelUnits(uint32_t deviceIndex, uint32_t channelIndex);

    static uint32_t nmbrOfNewMeasurements();
    static bool hasNewMeasurements();
    static void collectNewMeasurements();

#ifndef unitTesting

  private:
#endif

    static bool isPresent[static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices)];        // an array of bools, with true for each known device that is present
    static uint32_t actualNumberOfDevices;
};