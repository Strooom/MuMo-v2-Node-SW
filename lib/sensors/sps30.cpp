// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <sps30.hpp>
#include <settingscollection.hpp>
#include <logging.hpp>

#ifndef generic
#include "main.h"
extern I2C_HandleTypeDef hi2c2;
#else
extern uint8_t mockBME680Registers[256];
#include <cstring>

#endif

sensorDeviceState sps30::state{sensorDeviceState::unknown};
sensorChannel sps30::channels[nmbrChannels];
sensorChannelFormat sps30::channelFormats[nmbrChannels] = {
    {"temperature", "~C", 1},
    {"relativeHumidity", "%RH", 0},
    {"barometricPressure", "hPa", 0},
};

uint32_t sps30::rawDataTemperature;
uint32_t sps30::rawDataRelativeHumidity;

bool sps30::isPresent() {
    // 1. Check if something is connected to the I2C bus at the address of the sps30
    if (!testI2cAddress(i2cAddress)) {
        return false;
    }        //
    // 2. Check if it is a BME680 by reading the chip id register/value
    uint8_t chipidValue = readRegister(sps30::registers::chipId);
    return (sps30::chipIdValue == chipidValue);
}

void sps30::initialize() {
    // TODO : need to read the sensorChannel settins from EEPROM and restore them
    channels[temperature].set(0, 1, 0, 1);
    channels[relativeHumidity].set(0, 1, 0, 1);

    uint8_t registerData[42]{};
    readRegisters(0x8A, 23, registerData);             // read all calibration data from the sensorChannel and convert to proper coefficients
    readRegisters(0xE1, 14, registerData + 23);        //
    readRegisters(0x00, 5, registerData + 37);         //

    state = sensorDeviceState::sleeping;
}

bool sps30::hasNewMeasurement() {
    return (channels[temperature].hasNewValue || channels[relativeHumidity].hasNewValue || channels[barometricPressure].hasNewValue);
}

void sps30::clearNewMeasurements() {
    channels[temperature].hasNewValue        = false;
    channels[relativeHumidity].hasNewValue = false;
}


float sps30::valueAsFloat(uint32_t index) {
    return channels[index].getOutput();
}

void sps30::tick() {
    if (state != sensorDeviceState::sleeping) {
        adjustAllCounters();
        return;
    }

    if (anyChannelNeedsSampling()) {
        clearNewMeasurements();
        startSampling();
        state = sensorDeviceState::sampling;
    } else {
        adjustAllCounters();
    }
}

void sps30::run() {
    if ((state == sensorDeviceState::sampling) && samplingIsReady()) {
        readSample();

        if (channels[temperature].needsSampling()) {
            float sps30Temperature = calculateTemperature();
            channels[temperature].addSample(sps30Temperature);
            if (channels[temperature].hasOutput()) {
                channels[temperature].hasNewValue = true;
            }
        }

        if (channels[relativeHumidity].needsSampling()) {
            float sps30RelativeHumidity = calculateRelativeHumidity();
            channels[relativeHumidity].addSample(sps30RelativeHumidity);
            if (channels[relativeHumidity].hasOutput()) {
                channels[relativeHumidity].hasNewValue = true;
            }
        }

        if (channels[barometricPressure].needsSampling()) {
            float sps30BarometricPressure = calculateBarometricPressure();
            channels[barometricPressure].addSample(sps30BarometricPressure);
            if (channels[barometricPressure].hasOutput()) {
                channels[barometricPressure].hasNewValue = true;
            }
        }

        state = sensorDeviceState::sleeping;
        adjustAllCounters();
    }
}

bool sps30::anyChannelNeedsSampling() {
    return (channels[temperature].needsSampling() ||
            channels[relativeHumidity].needsSampling() ||
}

void sps30::adjustAllCounters() {
    channels[temperature].adjustCounters();
    channels[relativeHumidity].adjustCounters();
}

void sps30::startSampling() {
    state = sensorDeviceState::sampling;
    // run a ADC conversion cycle for Temp Hum and Presure, and when ready, read and store all raw ADC results
    writeRegister(sps30::registers::ctrl_hum, 0b00000001);
    writeRegister(sps30::registers::ctrl_meas, 0b00100101);
    state = sensorDeviceState::sampling;
}

bool sps30::samplingIsReady() {
    uint8_t status    = readRegister(sps30::registers::meas_status);
    uint8_t statusBit = status & 0x80;
    bool noNewData    = statusBit == 0;
    return !noNewData;
}

void sps30::readSample() {
    constexpr uint32_t nmbrRegisters{8};
    uint8_t registerData[nmbrRegisters];
    readRegisters(0x1F, nmbrRegisters, registerData);        // reads 8 registers, from 0x1F up to 0x26, they contain the raw ADC results for temperature, relativeHumidity and pressure
    rawDataTemperature        = ((static_cast<uint32_t>(registerData[3]) << 12) | (static_cast<uint32_t>(registerData[4]) << 4) | (static_cast<uint32_t>(registerData[5]) >> 4));
    rawDataRelativeHumidity   = ((static_cast<uint32_t>(registerData[6]) << 8) | (static_cast<uint32_t>(registerData[7])));
    rawDataBarometricPressure = ((static_cast<uint32_t>(registerData[0]) << 12) | (static_cast<uint32_t>(registerData[1]) << 4) | (static_cast<uint32_t>(registerData[2]) >> 4));
}

float sps30::calculateTemperature() {
    float var1                         = ((((float)rawDataTemperature / 16384.0f) - (calibrationCoefficientTemperature1 / 1024.0f)) * (calibrationCoefficientTemperature2));
    float var2                         = (((((float)rawDataTemperature / 131072.0f) - (calibrationCoefficientTemperature1 / 8192.0f)) * (((float)rawDataTemperature / 131072.0f) - (calibrationCoefficientTemperature1 / 8192.0f))) * (calibrationCoefficientTemperature3 * 16.0f));
    calibrationCoefficientTemperature4 = var1 + var2;
    return (calibrationCoefficientTemperature4 / 5120.0f);
}

float sps30::calculateRelativeHumidity() {
    float calc_hum;

    float temp_comp = ((calibrationCoefficientTemperature4) / 5120.0f);
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

float sps30::calculateBarometricPressure() {
    float var1   = ((calibrationCoefficientTemperature4 / 2.0f) - 64000.0f);
    float var2   = var1 * var1 * ((calibrationCoefficientPressure6) / (131072.0f));
    var2         = var2 + (var1 * (calibrationCoefficientPressure5) * 2.0f);
    var2         = (var2 / 4.0f) + ((calibrationCoefficientPressure4) * 65536.0f);
    var1         = ((((calibrationCoefficientPressure3 * var1 * var1) / 16384.0f) + (calibrationCoefficientPressure2 * var1)) / 524288.0f);
    var1         = ((1.0f + (var1 / 32768.0f)) * ((float)calibrationCoefficientPressure1));
    float result = (1048576.0f - ((float)rawDataBarometricPressure));

    if ((int)var1 != 0) {
        result     = (((result - (var2 / 4096.0f)) * 6250.0f) / var1);
        var1       = ((calibrationCoefficientPressure9)*result * result) / 2147483648.0f;
        var2       = result * ((calibrationCoefficientPressure8) / 32768.0f);
        float var3 = ((result / 256.0f) * (result / 256.0f) * (result / 256.0f) * (calibrationCoefficientPressure10 / 131072.0f));
        result     = (result + (var1 + var2 + var3 + (calibrationCoefficientPressure7 * 128.0f)) / 16.0f);
    } else {
        result = 0;
    }

    result = result / 100.0F;        // use hPa as unit

    return result;
}

bool sps30::testI2cAddress(uint8_t addressToTest) {
#ifndef generic
    return (HAL_OK == HAL_I2C_IsDeviceReady(&hi2c2, addressToTest << 1, halTrials, halTimeout));
#else
    return true;
#endif
}

uint8_t sps30::readRegister(registers registerAddress) {
    uint8_t result;
#ifndef generic
    HAL_I2C_Mem_Read(&hi2c2, i2cAddress << 1, static_cast<uint16_t>(registerAddress), I2C_MEMADD_SIZE_8BIT, &result, 1, halTimeout);
#else
    result = mockBME680Registers[static_cast<uint8_t>(registerAddress)];
#endif
    return result;
}

void sps30::writeRegister(registers registerAddress, uint8_t value) {
#ifndef generic
    HAL_I2C_Mem_Write(&hi2c2, i2cAddress << 1, static_cast<uint16_t>(registerAddress), I2C_MEMADD_SIZE_8BIT, &value, 1, halTimeout);
#else
    mockBME680Registers[static_cast<uint8_t>(registerAddress)] = value;
#endif
}

void sps30::readRegisters(uint16_t startAddress, uint16_t length, uint8_t* destination) {
#ifndef generic
    HAL_I2C_Mem_Read(&hi2c2, i2cAddress << 1, startAddress, I2C_MEMADD_SIZE_8BIT, destination, length, halTimeout);
#else
    memcpy(destination, mockBME680Registers + startAddress, length);
#endif
}

void sps30::log() {
    if (channels[temperature].hasNewValue) {
        logging::snprintf(logging::source::sensorData, "%s = %.2f *C\n", channelFormats[temperature].name, channels[temperature].getOutput());
    }
    if (channels[relativeHumidity].hasNewValue) {
        logging::snprintf(logging::source::sensorData, "%s = %.0f %s\n", channelFormats[relativeHumidity].name, channels[relativeHumidity].getOutput(), channelFormats[relativeHumidity].unit);
    }
    if (channels[barometricPressure].hasNewValue) {
        logging::snprintf(logging::source::sensorData, "%s = %.0f %s\n", channelFormats[barometricPressure].name, channels[barometricPressure].getOutput(), channelFormats[barometricPressure].unit);
    }
}
