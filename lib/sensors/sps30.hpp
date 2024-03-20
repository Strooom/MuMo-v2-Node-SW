// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once

#include <stdint.h>
#include <sensordevicestate.hpp>
#include <sensorchannel.hpp>
#include <sensorchannelformat.hpp>

class sps30 {
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
    static float calculateBarometricPressure();

    static void clearNewMeasurements();

    static constexpr uint8_t i2cAddress{0x44};
    static constexpr uint8_t halTrials{0x03};         // ST HAL requires a 'retry' parameters
    static constexpr uint8_t halTimeout{0x10};        // ST HAL requires a 'timeout' in ms

    // Registers
    enum class registers : uint8_t {

    };

    // Commands
    enum class commands : uint8_t {
    };

    // Other
    static constexpr uint8_t chipIdValue{0x61};        // value to expect at the chipIdregister, this allows to discover/recognize the BME68x

    static bool testI2cAddress(uint8_t addressToTest);                                              //
    static uint8_t readRegister(registers aRegister);                                               // read a single register
    static void readRegisters(uint16_t startAddress, uint16_t length, uint8_t* destination);        // read a range of registers into a buffer
    static void writeRegister(registers aRegister, const uint8_t value);                            // write a single register

    static uint32_t rawDataTemperature;
    static uint32_t rawDataRelativeHumidity;

    static bool awake;

};

