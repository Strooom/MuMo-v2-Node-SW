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
    static bool isPresent();
    static void initialize();
    static void wakeUp();
    static void goSleep();

    static sensorDeviceState getState() { return state; };
    static float valueAsFloat(uint32_t channelIndex);

    static void tick();
    static void run();

    static constexpr uint32_t nmbrChannels{3};
    static constexpr uint32_t pme2dot5{0};
    static constexpr uint32_t pme4{0};
    static constexpr uint32_t pme10{0};
    static sensorChannel channels[nmbrChannels];


#ifndef unitTesting

  private:
#endif
    static sensorDeviceState state;
    static bool anyChannelNeedsSampling();
    static void adjustAllCounters();
    static void startSampling();
    static bool samplingIsReady();
    static void stopSampling();
    static void readSample();


    static constexpr uint8_t i2cAddress{0x69};
    static constexpr uint8_t halTrials{0x03};
    static constexpr uint8_t halTimeout{0x10};

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
    static void write(command aRegister);
    static void write(command aRegister, const uint8_t value);
    static void write(command startAddress, uint8_t* sourceData, uint16_t length);
    static uint8_t readData(command aRegister);
    static void read(command startAddress, uint8_t* destination, uint16_t length);


    static bool awake;
};
