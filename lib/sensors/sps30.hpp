// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once

#include <stdint.h>
#include <sensordevicestate.hpp>
#include <sensorchannel.hpp>

class sps30 {
  public:
    sps30() = delete;
    static bool isPresent();
    static void initialize();
    static void run();
    static sensorDeviceState getState() { return state; };

    static constexpr uint32_t nmbrChannels{4};
    static constexpr uint32_t pme1{0};
    static constexpr uint32_t pme2dot5{1};
    static constexpr uint32_t pme4{2};
    static constexpr uint32_t pme10{3};
    static sensorChannel channels[nmbrChannels];

#ifndef unitTesting

  private:
#endif
    static sensorDeviceState state;
    static void startSampling();
    static bool samplingIsReady();
    static void stopSampling();
    static void readSample();

    static constexpr uint8_t i2cAddress{0x69};
    static constexpr uint8_t halTrials{0x03};
    static constexpr uint8_t halTimeout{0x20};

    // Commands
    enum class command : uint16_t {
        startMeasurement       = 0x0010,
        stopMeasurement        = 0x0104,
        readMeasurement        = 0x0300,
        isDataReady            = 0x0202,
        reset                  = 0xD304,
        goSleep                = 0x1001,
        startManualFanCleaning = 0x5607,
        wakeUp                 = 0x1103,
        readProductType        = 0xD002,
    };

    // Other
    static constexpr uint8_t chipIdValue{0x61};

    static bool testI2cAddress(uint8_t addressToTest);
    static void write(const command aCommand);
    static void write(const command aCommand, const uint8_t* sourceData, const uint32_t lengthInBytes);
    static void read(uint8_t* destinationData, const uint32_t lengthInBytes);

    static float _pme1;
    static float _pme2dot5;
    static float _pme4;
    static float _pme10;

    friend class sensorDeviceCollection;
};
