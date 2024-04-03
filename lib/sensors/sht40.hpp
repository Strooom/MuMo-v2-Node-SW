// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
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
    static float calculateBarometricPressure();

    static void clearNewMeasurements();

    static uint8_t i2cAddress; // SHT40 can have 0x44 or 0x45 as address -> so we need to probe for it
    static constexpr uint8_t halTrials{0x03};
    static constexpr uint8_t halTimeout{0x10};

    // Registers
    enum class registers : uint8_t {

    };

    // Commands
    enum class commands : uint8_t {
        MEASURE_HIGH_PRECISION_TICKS_CMD_ID              = 0xfd,
        MEASURE_MEDIUM_PRECISION_TICKS_CMD_ID            = 0xf6,
        MEASURE_LOWEST_PRECISION_TICKS_CMD_ID            = 0xe0,
        ACTIVATE_HIGHEST_HEATER_POWER_LONG_TICKS_CMD_ID  = 0x39,
        ACTIVATE_HIGHEST_HEATER_POWER_SHORT_TICKS_CMD_ID = 0x32,
        ACTIVATE_MEDIUM_HEATER_POWER_LONG_TICKS_CMD_ID   = 0x2f,
        ACTIVATE_MEDIUM_HEATER_POWER_SHORT_TICKS_CMD_ID  = 0x24,
        ACTIVATE_LOWEST_HEATER_POWER_LONG_TICKS_CMD_ID   = 0x1e,
        ACTIVATE_LOWEST_HEATER_POWER_SHORT_TICKS_CMD_ID  = 0x15,
        SERIAL_NUMBER_CMD_ID                             = 0x89,
        SOFT_RESET_CMD_ID                                = 0x94,
    };

    // Other
    static bool testI2cAddress(uint8_t addressToTest);
    static uint8_t readRegister(registers aRegister);
    static void readRegisters(uint16_t startAddress, uint16_t length, uint8_t* destination);
    static void writeRegister(registers aRegister, const uint8_t value);

    static uint32_t rawDataTemperature;
    static uint32_t rawDataRelativeHumidity;

    static bool awake;
};

static float temperatureFromRaw(uint32_t rawData);
static float relativeHumidityFromRaw(uint32_t rawData);

// float sensirion::temperatureFromRaw(uint32_t rawData) {
// }

// float sensirion::relativeHumidityFromRaw(uint32_t rawData) {
// }
