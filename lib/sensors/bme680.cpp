// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <sensordevicetype.hpp>
#include <bme680.hpp>
#include <settingscollection.hpp>
#include <logging.hpp>
#include <float.hpp>
#include <i2c.hpp>

#ifndef generic
#include "main.h"
extern I2C_HandleTypeDef hi2c2;
#else
uint8_t mockBME680Registers[256];
bool mockBME680Present{false};
#include <cstring>
#endif

sensorDeviceState bme680::state{sensorDeviceState::unknown};
sensorChannel bme680::channels[nmbrChannels] = {
    {1, "temperature", "~C"},
    {0, "relativeHumidity", "%"},
    {0, "barometricPressure", "hPa"},
};

uint32_t bme680::rawDataTemperature;
uint32_t bme680::rawDataBarometricPressure;
uint32_t bme680::rawDataRelativeHumidity;

float bme680::calibrationCoefficientTemperature1;
float bme680::calibrationCoefficientTemperature2;
float bme680::calibrationCoefficientTemperature3;
float bme680::calibrationCoefficientTemperature4;

float bme680::calibrationCoefficientPressure1;
float bme680::calibrationCoefficientPressure2;
float bme680::calibrationCoefficientPressure3;
float bme680::calibrationCoefficientPressure4;
float bme680::calibrationCoefficientPressure5;
float bme680::calibrationCoefficientPressure6;
float bme680::calibrationCoefficientPressure7;
float bme680::calibrationCoefficientPressure8;
float bme680::calibrationCoefficientPressure9;
float bme680::calibrationCoefficientPressure10;

float bme680::calibrationCoefficientHumidity1;
float bme680::calibrationCoefficientHumidity2;
float bme680::calibrationCoefficientHumidity3;
float bme680::calibrationCoefficientHumidity4;
float bme680::calibrationCoefficientHumidity5;
float bme680::calibrationCoefficientHumidity6;
float bme680::calibrationCoefficientHumidity7;

bool bme680::isPresent() {
    // 1. Check if something is connected to the I2C bus at the address of the bme680
    if (!testI2cAddress(i2cAddress)) {
        return false;
    }        //
    // 2. Check if it is a BME680 by reading the chip id register/value
    uint8_t chipidValue = readRegister(bme680::registers::chipId);
    return (bme680::chipIdValue == chipidValue);
}

void bme680::initialize() {
#ifndef generic
    uint8_t registerData[42]{};
    readRegisters(0x8A, 23, registerData);             // read all calibration data from the sensorChannel and convert to proper coefficients
    readRegisters(0xE1, 14, registerData + 23);        //
    readRegisters(0x00, 5, registerData + 37);         //

    calibrationCoefficientTemperature1 = static_cast<float>((static_cast<uint16_t>(registerData[32]) << 8) | static_cast<uint16_t>(registerData[31]));
    calibrationCoefficientTemperature2 = static_cast<float>((static_cast<int16_t>(registerData[1]) << 8) | static_cast<int16_t>(registerData[0]));
    calibrationCoefficientTemperature3 = static_cast<float>(registerData[2]);

    calibrationCoefficientHumidity1 = static_cast<float>((static_cast<uint16_t>(registerData[25]) << 4) | static_cast<uint16_t>(registerData[24] & 0x0F));
    calibrationCoefficientHumidity2 = static_cast<float>((static_cast<uint16_t>(registerData[23]) << 4) | static_cast<uint16_t>(registerData[24] >> 4));
    calibrationCoefficientHumidity3 = static_cast<float>(registerData[26]);
    calibrationCoefficientHumidity4 = static_cast<float>(registerData[27]);
    calibrationCoefficientHumidity5 = static_cast<float>(registerData[28]);
    calibrationCoefficientHumidity6 = static_cast<float>(registerData[29]);
    calibrationCoefficientHumidity7 = static_cast<float>(registerData[30]);

    calibrationCoefficientPressure1  = static_cast<float>((static_cast<uint16_t>(registerData[5]) << 8) | static_cast<uint16_t>(registerData[4]));
    calibrationCoefficientPressure2  = static_cast<float>(static_cast<int16_t>((static_cast<uint16_t>(registerData[7]) << 8) | static_cast<uint16_t>(registerData[6])));
    calibrationCoefficientPressure3  = static_cast<float>(registerData[8]);
    calibrationCoefficientPressure4  = static_cast<float>(static_cast<int16_t>((static_cast<uint16_t>(registerData[11]) << 8) | static_cast<uint16_t>(registerData[10])));
    calibrationCoefficientPressure5  = static_cast<float>(static_cast<int16_t>((static_cast<uint16_t>(registerData[13]) << 8) | static_cast<uint16_t>(registerData[12])));
    calibrationCoefficientPressure6  = static_cast<float>(registerData[15]);
    calibrationCoefficientPressure7  = static_cast<float>(registerData[14]);
    calibrationCoefficientPressure8  = static_cast<float>(static_cast<int16_t>(static_cast<uint16_t>(registerData[19]) << 8) | static_cast<uint16_t>(registerData[18]));
    calibrationCoefficientPressure9  = static_cast<float>(static_cast<int16_t>(static_cast<uint16_t>(registerData[21]) << 8) | static_cast<uint16_t>(registerData[20]));
    calibrationCoefficientPressure10 = static_cast<float>(registerData[22]);
#else
    mockBME680Registers[static_cast<uint8_t>(bme680::registers::chipId)]      = bme680::chipIdValue;
    mockBME680Registers[static_cast<uint8_t>(bme680::registers::meas_status)] = 0x80;
    uint8_t calibrationDataPart1[23]                                          = {0x62, 0x67, 0x03, 0x10, 0x8F, 0x90, 0x68, 0xD7, 0x58, 0x00, 0x38, 0x22, 0x62, 0xFF, 0x2C, 0x1E, 0x00, 0x00, 0x71, 0xF4, 0x5B, 0xF6, 0x1E};
    uint8_t calibrationDataPart2[14]                                          = {0x3D, 0xBD, 0x37, 0x00, 0x2D, 0x14, 0x78, 0x9C, 0xB6, 0x65, 0xAB, 0xDC, 0xFB, 0x12};
    uint8_t calibrationDataPart3[5]                                           = {0x28, 0xAA, 0x16, 0x4C, 0x03};
    memcpy(mockBME680Registers + 0x8A, calibrationDataPart1, 23);
    memcpy(mockBME680Registers + 0xE1, calibrationDataPart2, 14);
    memcpy(mockBME680Registers + 0xF0, calibrationDataPart3, 5);
#endif
    for (uint32_t channelIndex = 0; channelIndex < nmbrChannels; channelIndex++) {
        channels[channelIndex].set(0, 0);
    }
    state = sensorDeviceState::sleeping;
}

void bme680::run() {
    if ((state == sensorDeviceState::sampling) && samplingIsReady()) {
        readSample();

        if (channels[temperature].needsSampling()) {
            float bme680Temperature = calculateTemperature();
            channels[temperature].addSample(bme680Temperature);
        }

        if (channels[relativeHumidity].needsSampling()) {
            float bme680RelativeHumidity = calculateRelativeHumidity();
            channels[relativeHumidity].addSample(bme680RelativeHumidity);
        }

        if (channels[barometricPressure].needsSampling()) {
            float bme680BarometricPressure = calculateBarometricPressure();
            channels[barometricPressure].addSample(bme680BarometricPressure);
        }

        state = sensorDeviceState::sleeping;
    }
}

void bme680::startSampling() {
    writeRegister(bme680::registers::ctrl_hum, 0b00000001);
    writeRegister(bme680::registers::ctrl_meas, 0b00100101);
    state = sensorDeviceState::sampling;
}

bool bme680::samplingIsReady() {
    uint8_t status    = readRegister(bme680::registers::meas_status);
    uint8_t statusBit = status & 0x80;
    bool noNewData    = statusBit == 0;
    return !noNewData;
}

void bme680::readSample() {
    constexpr uint32_t nmbrRegisters{8};
    uint8_t registerData[nmbrRegisters];
    readRegisters(0x1F, nmbrRegisters, registerData);        // reads 8 registers, from 0x1F up to 0x26, they contain the raw ADC results for temperature, relativeHumidity and pressure
    rawDataTemperature        = ((static_cast<uint32_t>(registerData[3]) << 12) | (static_cast<uint32_t>(registerData[4]) << 4) | (static_cast<uint32_t>(registerData[5]) >> 4));
    rawDataRelativeHumidity   = ((static_cast<uint32_t>(registerData[6]) << 8) | (static_cast<uint32_t>(registerData[7])));
    rawDataBarometricPressure = ((static_cast<uint32_t>(registerData[0]) << 12) | (static_cast<uint32_t>(registerData[1]) << 4) | (static_cast<uint32_t>(registerData[2]) >> 4));
}

float bme680::calculateTemperature() {
    float var1                         = ((((float)rawDataTemperature / 16384.0f) - (calibrationCoefficientTemperature1 / 1024.0f)) * calibrationCoefficientTemperature2);
    float var2                         = (((((float)rawDataTemperature / 131072.0f) - (calibrationCoefficientTemperature1 / 8192.0f)) * (((float)rawDataTemperature / 131072.0f) - (calibrationCoefficientTemperature1 / 8192.0f))) * (calibrationCoefficientTemperature3 * 16.0f));
    calibrationCoefficientTemperature4 = var1 + var2;
    return (calibrationCoefficientTemperature4 / 5120.0f);
}

float bme680::calculateRelativeHumidity() {
    float calc_hum;

    float temp_comp = (calibrationCoefficientTemperature4 / 5120.0f);
    float var1      = static_cast<float>(rawDataRelativeHumidity) - (calibrationCoefficientHumidity1 * 16.0f) + ((calibrationCoefficientHumidity3 / 2.0f) * temp_comp);
    float var2      = var1 * (calibrationCoefficientHumidity2 / 262144.0f) * (1.0f + ((calibrationCoefficientHumidity4 / 16384.0f) * temp_comp) + ((calibrationCoefficientHumidity5 / 1048576.0f) * temp_comp * temp_comp));
    float var3      = calibrationCoefficientHumidity6 / 16384.0f;
    float var4      = calibrationCoefficientHumidity7 / 2097152.0f;
    calc_hum        = var2 + ((var3 + (var4 * temp_comp)) * var2 * var2);
    if (calc_hum > 100.0f) {
        calc_hum = 100.0f;
    } else if (calc_hum < 0.0f) {
        calc_hum = 0.0f;
    }

    return calc_hum;
}

float bme680::calculateBarometricPressure() {
    float var1   = ((calibrationCoefficientTemperature4 / 2.0f) - 64000.0f);
    float var2   = var1 * var1 * (calibrationCoefficientPressure6 / 131072.0f);
    var2         = var2 + (var1 * calibrationCoefficientPressure5 * 2.0f);
    var2         = (var2 / 4.0f) + (calibrationCoefficientPressure4 * 65536.0f);
    var1         = ((((calibrationCoefficientPressure3 * var1 * var1) / 16384.0f) + (calibrationCoefficientPressure2 * var1)) / 524288.0f);
    var1         = ((1.0f + (var1 / 32768.0f)) * calibrationCoefficientPressure1);
    float result = (1048576.0f - ((float)rawDataBarometricPressure));

    if ((int)var1 != 0) {
        result     = (((result - (var2 / 4096.0f)) * 6250.0f) / var1);
        var1       = (calibrationCoefficientPressure9 * result * result) / 2147483648.0f;
        var2       = result * (calibrationCoefficientPressure8 / 32768.0f);
        float var3 = ((result / 256.0f) * (result / 256.0f) * (result / 256.0f) * (calibrationCoefficientPressure10 / 131072.0f));
        result     = (result + (var1 + var2 + var3 + (calibrationCoefficientPressure7 * 128.0f)) / 16.0f);
    } else {
        result = 0;
    }

    result = result / 100.0F;        // use hPa as unit

    return result;
}

bool bme680::testI2cAddress(uint8_t addressToTest) {
#ifndef generic
    bool result = (HAL_OK == HAL_I2C_IsDeviceReady(&hi2c2, static_cast<uint16_t>(addressToTest << 1), halTrials, halTimeout));
    return result;

#else
    return mockBME680Present;
#endif
}

uint8_t bme680::readRegister(registers registerAddress) {
    uint8_t result;
#ifndef generic
    HAL_I2C_Mem_Read(&hi2c2, i2cAddress << 1, static_cast<uint16_t>(registerAddress), I2C_MEMADD_SIZE_8BIT, &result, 1, halTimeout);
#else
    result = mockBME680Registers[static_cast<uint8_t>(registerAddress)];
#endif
    return result;
}

void bme680::writeRegister(registers registerAddress, uint8_t value) {
#ifndef generic
    HAL_I2C_Mem_Write(&hi2c2, i2cAddress << 1, static_cast<uint16_t>(registerAddress), I2C_MEMADD_SIZE_8BIT, &value, 1, halTimeout);
#else
    mockBME680Registers[static_cast<uint8_t>(registerAddress)] = value;
#endif
}

void bme680::readRegisters(uint16_t startAddress, uint16_t length, uint8_t* destination) {
#ifndef generic
    HAL_I2C_Mem_Read(&hi2c2, i2cAddress << 1, startAddress, I2C_MEMADD_SIZE_8BIT, destination, length, halTimeout);
#else
    (void)memcpy(destination, mockBME680Registers + startAddress, length);
#endif
}
