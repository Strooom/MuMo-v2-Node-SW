// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once

#include <stdint.h>
#include <sensordevicestate.hpp>
#include <sensorchannel.hpp>

class bme680 {
  public:
  bme680() = delete;
    static bool isPresent();
    static void initialize();
    static void run();

    static sensorDeviceState getState() { return state; };

    static constexpr uint32_t nmbrChannels{3};
    static constexpr uint32_t temperature{0};
    static constexpr uint32_t relativeHumidity{1};
    static constexpr uint32_t barometricPressure{2};
    static sensorChannel channels[nmbrChannels];

#ifndef unitTesting

  private:
#endif
    static sensorDeviceState state;
    static void startSampling();
    static bool samplingIsReady();
    static void readSample();

    static float calculateTemperature();
    static float calculateRelativeHumidity();
    static float calculateBarometricPressure();

    static constexpr uint8_t i2cAddress{0x76};        // default I2C address for this sensorChannel, DSO tied to GND on our hardware
    static constexpr uint8_t halTrials{0x03};         // ST HAL requires a 'retry' parameters
    static constexpr uint8_t halTimeout{0x10};        // ST HAL requires a 'timeout' in ms

    // Registers
    enum class registers : uint8_t {
        chipId      = 0xD0,        // address of register holding chipId
        reset       = 0xE0,        // address of register to reset the device : Write 0xB6
        variant_id  = 0xF0,        // address of register to reset the device : Write 0xB6
        ctrl_hum    = 0x72,        // address of register to set humidity oversampling
        ctrl_meas   = 0x74,        // address of register to set temperature & pressure versampling
        config      = 0x75,        // address of register to set IIR filter & power mode
        temp_xlsb   = 0x24,        // address of register to read temperature XLSB
        temp_lsb    = 0x23,        // address of register to read temperature LSB
        temp_msb    = 0x22,        // address of register to read temperature MSB
        hum_lsb     = 0x26,        // address of register to read humidity LSB
        hum_msb     = 0x25,        // address of register to read humidity MSB
        press_xlsb  = 0x8C,        // address of register to read pressure LSB
        press_lsb   = 0x8B,        // address of register to read pressure LSB
        press_msb   = 0x8A,        // address of register to read pressure MSB
        meas_status = 0x1D,        // address of register to read measurement status
        status      = 0x73,        // address of register to read status
        meas_index  = 0x1E,        // address of register to read measurement index
        meas_period = 0x68,        // address of register to read measurement period
        gas_wait0   = 0x64,        // address of register to read gas wait time
        res_heat0   = 0x5A,        // address of register to read heater resistance
        idac_heat0  = 0x50,        // address of register to read heater current
        gas_r_lsb   = 0x2B,        // address of register to read gas resistance LSB
        gas_r_msb   = 0x2A,        // address of register to read gas resistance MSB
    };

    // Commands
    enum class command : uint8_t {
        softReset = 0xB6,        // value to write to resetRegister to reset the BME68X
    };

    // Other
    static constexpr uint8_t chipIdValue{0x61};        // value to expect at the chipIdregister, this allows to discover/recognize the BME68x

    static bool testI2cAddress(uint8_t addressToTest);                                              //
    static uint8_t readRegister(registers aRegister);                                               // read a single register
    static void readRegisters(uint16_t startAddress, uint16_t length, uint8_t* destination);        // read a range of registers into a buffer
    static void writeRegister(registers aRegister, const uint8_t value);                            // write a single register

    static uint32_t rawDataTemperature;
    static uint32_t rawDataBarometricPressure;
    static uint32_t rawDataRelativeHumidity;

    
    // Calibration data
    static float calibrationCoefficientTemperature1;
    static float calibrationCoefficientTemperature2;
    static float calibrationCoefficientTemperature3;
    static float calibrationCoefficientTemperature4;

    static float calibrationCoefficientPressure1;
    static float calibrationCoefficientPressure2;
    static float calibrationCoefficientPressure3;
    static float calibrationCoefficientPressure4;
    static float calibrationCoefficientPressure5;
    static float calibrationCoefficientPressure6;
    static float calibrationCoefficientPressure7;
    static float calibrationCoefficientPressure8;
    static float calibrationCoefficientPressure9;
    static float calibrationCoefficientPressure10;

    static float calibrationCoefficientHumidity1;
    static float calibrationCoefficientHumidity2;
    static float calibrationCoefficientHumidity3;
    static float calibrationCoefficientHumidity4;
    static float calibrationCoefficientHumidity5;
    static float calibrationCoefficientHumidity6;
    static float calibrationCoefficientHumidity7;

    friend class sensorDeviceCollection;
};