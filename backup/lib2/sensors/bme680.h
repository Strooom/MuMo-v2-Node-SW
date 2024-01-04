// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include "sensorchannel.h"
#include "sensortype.h"
#include "sensorstate.h"

class bme680 {
  public:
    static bool isPresent();
    static void initialize();
    static void sample();

    static float getTemperature();
    static float getRelativeHumidity();
    static float getBarometricPressure();

    static void goSleep();

    static constexpr uint32_t nmbrChannels{3};
    static sensorChannel channels[nmbrChannels];

#ifndef unitTesting

  private:
#endif
    static sensorState state;
    static constexpr uint8_t i2cAddress{0x76};        // default I2C address for this sensorChannel, DSO tied to GND on our hardware
    static constexpr uint8_t halTrials{0x03};         // ST HAL requires a 'retry' parameters
    static constexpr uint8_t halTimeout{0x10};        // ST HAL requires a 'timeout' in ms

    // Registers
    enum class registers : uint8_t {
        chipId     = 0xD0,         // address of register holding chipId
        reset      = 0xE0,         // address of register to reset the device : Write 0xB6
        variant_id = 0xF0,         // address of register to reset the device : Write 0xB6
        ctrl_hum   = 0x72,         // address of register to set humidity oversampling
        ctrl_meas  = 0x74,         // address of register to set temperature & pressure versampling
        config     = 0x75,         // address of register to set IIR filter & power mode

        temp_xlsb = 0x24,          // address of register to read temperature XLSB
        temp_lsb  = 0x23,          // address of register to read temperature LSB
        temp_msb  = 0x22,          // address of register to read temperature MSB

        hum_lsb = 0x26,            // address of register to read humidity LSB
        hum_msb = 0x25,            // address of register to read humidity MSB

        press_xlsb = 0x8C,         // address of register to read pressure LSB
        press_lsb  = 0x8B,         // address of register to read pressure LSB
        press_msb  = 0x8A,         // address of register to read pressure MSB

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
    enum class commands : uint8_t {
        softReset = 0xB6,        // value to write to resetRegister to reset the BME68X
    };

    // Other
    static constexpr uint8_t chipIdValue{0x61};                                                     // value to expect at the chipIdregister, this allows to discover/recognize the BME68x

    static bool testI2cAddress(uint8_t addressToTest);                                              //
    static uint8_t readRegister(registers aRegister);                                               // read a single register
    static void readRegisters(uint16_t startAddress, uint16_t length, uint8_t* destination);        // read a range of registers into a buffer
    static void writeRegister(registers aRegister, const uint8_t value);                            // write a single register

    static uint32_t rawDataTemperature;
    static uint32_t rawDataBarometricPressure;
    static uint32_t rawDataRelativeHumidity;

    static bool awake;

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
};

/*
Initialization
Write I2C : register = [E0], data[1] = [B6 ]
Read I2C : register = [D0], data[1] = [61 ]
Read I2C : register = [F0], data[1] = [00 ]
Read I2C : register = [8A], data[23] = [62 67 03 10 8F 90 68 D7 58 00 38 22 62 FF 2C 1E 00 00 71 F4 5B F6 1E ]
Read I2C : register = [E1], data[14] = [3D BD 37 00 2D 14 78 9C B6 65 AB DC FB 12 ]
Read I2C : register = [00], data[5] = [28 AA 16 4C 03 ]

Configuration
Read I2C : register = [74], data[1] = [00 ]
Read I2C : register = [74], data[1] = [00 ]
Read I2C : register = [71], data[5] = [00 00 00 00 00 ]
Write I2C : register = [71], data[9] = [80 72 05 73 00 74 44 75 00 ]
Set Heater Config
Read I2C : register = [74], data[1] = [44 ]
Write I2C : register = [5A], data[1] = [78 ]
Write I2C : register = [64], data[1] = [59 ]
Read I2C : register = [70], data[2] = [00 80 ]
Write I2C : register = [70], data[3] = [08 71 80 ]

Set Operation Mode
Read I2C : register = [74], data[1] = [44 ]
Write I2C : register = [74], data[1] = [45 ]

Read Data
Read I2C : register = [1D], data[17] = [80 FF 48 92 00 75 F8 00 62 20 80 00 00 00 04 00 04 ]
Read I2C : register = [5A], data[1] = [78 ]
Read I2C : register = [50], data[1] = [00 ]
Read I2C : register = [64], data[1] = [59 ]


par_h1	uint16_t	893
par_h2	uint16_t	987
par_h3	int8_t	0 '\0'
par_h4	int8_t	45 '-'
par_h5	int8_t	20 '\024'
par_h6	uint8_t	120 'x'
par_h7	int8_t	-100 '\234'
par_gh1	int8_t	-5 'û'
par_gh2	int16_t	-9045
par_gh3	int8_t	18 '\022'

par_t1	uint16_t	26038
par_t2	int16_t	26466
par_t3	int8_t	3

par_p1	uint16_t	37007
par_p2	int16_t	-10392
par_p3	int8_t	88 'X'
par_p4	int16_t	8760
par_p5	int16_t	-158
par_p6	int8_t	30 '\036'
par_p7	int8_t	44 ','
par_p8	int16_t	-2959
par_p9	int16_t	-2469
par_p10	uint8_t	30 '\036'
t_fine	float	107711.469
res_heat_range	uint8_t	1 '\001'
res_heat_val	int8_t	40 '('
range_sw_err	int8_t	0 '\0'

adc_temp	uint32_t	484376

mine :
rawData1	uint8_t	16 '\020'
rawData2	uint8_t	143 '\217'
rawData3	uint8_t	144 '\220'
rawData	uint32_t	67833
 theirs
 buff[5]	uint8_t	119 'w'
buff[6]	uint8_t	16 '\020'
buff[7]	uint8_t	128 '\200'

My READ
Read I2C : register = [8A], data[23] = [62 67 03 10 8F 90 68 D7 58 00 38 22 62 FF 2C 1E 00 00 71 F4 5B F6 1E ]
Read I2C : register = [E1], data[14] = [3D BD 37 00 2D 14 78 9C B6 65 AB DC FB 12 ]
Read I2C : register = [00], data[5] = [28 AA 16 4C 03 ]



*/