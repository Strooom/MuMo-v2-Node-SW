// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once

#include <stdint.h>
#include <sensordevicestate.hpp>
#include <sensorchannel.hpp>

class scd40 {
  public:
    scd40() = delete;
    static bool isPresent();
    static void initialize();
    static void run();
    static sensorDeviceState getState() { return state; };

    static constexpr uint32_t nmbrChannels{3};
    static constexpr uint32_t temperature{0};
    static constexpr uint32_t relativeHumidity{1};
    static constexpr uint32_t co2{2};
    static sensorChannel channels[nmbrChannels];

#ifndef unitTesting

  private:
#endif
    static sensorDeviceState state;
    static void startSampling();
    static bool samplingIsReady();
    static void readSample();

    static float calculateTemperature(uint32_t rawData);
    static float calculateRelativeHumidity(uint32_t rawData);
    static float calculateCO2(uint32_t rawData);

    static constexpr uint8_t i2cAddress{0x62};
    static constexpr uint8_t halTrials{0x03};         // ST HAL requires a 'retry' parameters
    static constexpr uint8_t halTimeout{0x20};        // ST HAL requires a 'timeout' in ms

    // Commands
    enum class command : uint16_t {
        startPeriodicMeasurement         = 0x21B1,
        startLowPowerPeriodicMeasurement = 0x21AC,
        getDataReadyStatus               = 0xE4B8,
        readMeasurement                  = 0xEC05,
        stopPeriodicMeasurement          = 0x3F86,
    };

    static bool testI2cAddress(const uint8_t addressToTest);
    static void writeCommand(const scd40::command aCommand);
    static void readData(uint16_t* data, const uint32_t dataLength);

    static uint32_t rawDataTemperature;
    static uint32_t rawDataRelativeHumidity;
    static uint32_t rawCo2;

    friend class sensorDeviceCollection;
};
