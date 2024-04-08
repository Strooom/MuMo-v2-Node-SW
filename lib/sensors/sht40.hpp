// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once

#include <stdint.h>
#include <sensirion.hpp>
#include <sensordevicestate.hpp>
#include <sensorchannel.hpp>
#include <sensorchannelformat.hpp>

class sht40 {
  public:
    static bool isPresent();
    static void initialize();
    static sensorDeviceState getState() { return state; };
    static bool hasNewMeasurement();
    static float valueAsFloat(uint32_t channelIndex);
    static const char* channelName(uint32_t channelIndex);
    static const char* channelUnit(uint32_t channelIndex);

    static void tick();
    static void run();
    static void log();

    static constexpr uint32_t nmbrChannels{2};
    static constexpr uint32_t temperature{0};
    static constexpr uint32_t relativeHumidity{1};
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
    static void readSample();

    static float calculateTemperature();
    static float calculateRelativeHumidity();

    static void clearNewMeasurements();

    static uint8_t i2cAddress;        // SHT40 can have 0x44 or 0x45 as address -> so we need to probe for it
    static constexpr uint8_t halTrials{0x03};
    static constexpr uint8_t halTimeout{0x10};

    // Commands
    enum class command : uint8_t {
        getMeasurementHighPrecision = 0xFD,        // SH4x datasheet section 4.5
    };

    // Other
    static bool testI2cAddress(uint8_t addressToTest);
    static void read(command aCommand, uint8_t* response, uint32_t responseLength);

    static uint32_t rawDataTemperature;
    static uint32_t rawDataRelativeHumidity;

    static bool awake;
};

