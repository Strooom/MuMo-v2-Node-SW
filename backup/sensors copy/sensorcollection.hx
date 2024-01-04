// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include "sensortype.h"
#include "sensorchannel.h"
#include "measurement.h"

class sensorChannelCollection {
  public:
    static void discover();        // this will scan all hardware ports (I2C, UART, ...) for known sensors, and if found add their sensorChannels to the collection
    enum class runResult : uint32_t {
        none,
        newMeasurements,
    };
    static runResult run();        // this will check all sensors in the collection if they need to be sampled, and will measurement them if needed. Sample is stored in measurement collection
    static constexpr uint32_t maxNumberOfChannels{8};
//    static measurement latestMeasurements[maxNumberOfChannels];        // collects the measurements of all sensors during a run
//    static uint32_t actualNumberOfMeasurements;

#ifndef unitTesting

  private:
#endif
    static uint32_t actualNumberOfChannels;
    static sensorChannel theSensorChannelCollection[maxNumberOfChannels];

    static void add(sensorChannelType aType, uint32_t oversamplingLowPower, uint32_t prescalerLowPower, uint32_t oversamplingHighPower, uint32_t prescalerHighPower);
//    static void addMeasurement(sensorType aType, float aValue);
};
