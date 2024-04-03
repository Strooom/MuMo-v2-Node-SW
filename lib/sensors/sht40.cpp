// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <sht40.hpp>
#include <settingscollection.hpp>
#include <logging.hpp>

#ifndef generic
#include "main.h"
extern I2C_HandleTypeDef hi2c2;
#else
extern uint8_t mockBME680Registers[256];
#include <cstring>

#endif

uint8_t sht40::i2cAddress;
sensorDeviceState sht40::state{sensorDeviceState::unknown};
sensorChannel sht40::channels[nmbrChannels];
sensorChannelFormat sht40::channelFormats[nmbrChannels] = {
    {"temperature", "~C", 1},
    {"relativeHumidity", "%RH", 0},
};

uint32_t sht40::rawDataTemperature;
uint32_t sht40::rawDataRelativeHumidity;

bool sht40::isPresent() {
    i2cAddress = 0x44;
    if (testI2cAddress(i2cAddress)) {
        return true;
    }
    i2cAddress = 0x45;
    if (testI2cAddress(i2cAddress)) {
        return true;
    }
    return false;
}

void sht40::initialize() {
    // TODO : need to read the sensorChannel settins from EEPROM and restore them
    channels[temperature].set(0, 1, 0, 1);
    channels[relativeHumidity].set(0, 1, 0, 1);

    state = sensorDeviceState::sleeping;
}

bool sht40::hasNewMeasurement() {
    return (channels[temperature].hasNewValue || channels[relativeHumidity].hasNewValue);
}

void sht40::clearNewMeasurements() {
    channels[temperature].hasNewValue      = false;
    channels[relativeHumidity].hasNewValue = false;
}

float sht40::valueAsFloat(uint32_t index) {
    return channels[index].getOutput();
}

void sht40::tick() {
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

void sht40::run() {
    if ((state == sensorDeviceState::sampling) && samplingIsReady()) {
        readSample();

        if (channels[temperature].needsSampling()) {
            float sht40Temperature = calculateTemperature();
            channels[temperature].addSample(sht40Temperature);
            if (channels[temperature].hasOutput()) {
                channels[temperature].hasNewValue = true;
            }
        }

        if (channels[relativeHumidity].needsSampling()) {
            float sht40RelativeHumidity = calculateRelativeHumidity();
            channels[relativeHumidity].addSample(sht40RelativeHumidity);
            if (channels[relativeHumidity].hasOutput()) {
                channels[relativeHumidity].hasNewValue = true;
            }
        }

        state = sensorDeviceState::sleeping;
        adjustAllCounters();
    }
}

bool sht40::anyChannelNeedsSampling() {
    return (channels[temperature].needsSampling() || channels[relativeHumidity].needsSampling());
}

void sht40::adjustAllCounters() {
    channels[temperature].adjustCounters();
    channels[relativeHumidity].adjustCounters();
}

void sht40::startSampling() {
    state = sensorDeviceState::sampling;
}

bool sht40::samplingIsReady() {
    bool noNewData{false};
    return !noNewData;
}

void sht40::readSample() {
    // constexpr uint32_t nmbrRegisters{8};
    // uint8_t registerData[nmbrRegisters];
    // readRegisters(0x1F, nmbrRegisters, registerData);        // reads 8 registers, from 0x1F up to 0x26, they contain the raw ADC results for temperature, relativeHumidity and pressure
    // rawDataTemperature        = ((static_cast<uint32_t>(registerData[3]) << 12) | (static_cast<uint32_t>(registerData[4]) << 4) | (static_cast<uint32_t>(registerData[5]) >> 4));
    // rawDataRelativeHumidity   = ((static_cast<uint32_t>(registerData[6]) << 8) | (static_cast<uint32_t>(registerData[7])));
    // rawDataBarometricPressure = ((static_cast<uint32_t>(registerData[0]) << 12) | (static_cast<uint32_t>(registerData[1]) << 4) | (static_cast<uint32_t>(registerData[2]) >> 4));
}

float sht40::calculateTemperature() {
    // float var1                         = ((((float)rawDataTemperature / 16384.0f) - (calibrationCoefficientTemperature1 / 1024.0f)) * (calibrationCoefficientTemperature2));
    // float var2                         = (((((float)rawDataTemperature / 131072.0f) - (calibrationCoefficientTemperature1 / 8192.0f)) * (((float)rawDataTemperature / 131072.0f) - (calibrationCoefficientTemperature1 / 8192.0f))) * (calibrationCoefficientTemperature3 * 16.0f));
    // calibrationCoefficientTemperature4 = var1 + var2;
    // return (calibrationCoefficientTemperature4 / 5120.0f);
    return 0.0F;
}

float sht40::calculateRelativeHumidity() {
    // float calc_hum;

    // float temp_comp = ((calibrationCoefficientTemperature4) / 5120.0f);
    // float var1      = static_cast<float>(rawDataRelativeHumidity) - (calibrationCoefficientHumidity1 * 16.0f) + ((calibrationCoefficientHumidity3 / 2.0f) * temp_comp);
    // float var2      = var1 * (calibrationCoefficientHumidity2 / 262144.0f) * (1.0f + ((calibrationCoefficientHumidity4 / 16384.0f) * temp_comp) + ((calibrationCoefficientHumidity5 / 1048576.0f) * temp_comp * temp_comp));
    // float var3      = calibrationCoefficientHumidity6 / 16384.0f;
    // float var4      = calibrationCoefficientHumidity7 / 2097152.0f;
    // calc_hum        = var2 + ((var3 + (var4 * temp_comp)) * var2 * var2);
    // if (calc_hum > 100.0f) {
    //     calc_hum = 100.0f;
    // } else if (calc_hum < 0.0f) {
    //     calc_hum = 0.0f;
    // }

    // return calc_hum;
    return 0.0F;
}

bool sht40::testI2cAddress(uint8_t addressToTest) {
#ifndef generic
    return (HAL_OK == HAL_I2C_IsDeviceReady(&hi2c2, addressToTest << 1, halTrials, halTimeout));
#else
    return true;
#endif
}

uint8_t sht40::readRegister(registers registerAddress) {
    uint8_t result;
#ifndef generic
    HAL_I2C_Mem_Read(&hi2c2, i2cAddress << 1, static_cast<uint16_t>(registerAddress), I2C_MEMADD_SIZE_8BIT, &result, 1, halTimeout);
#else
    result = mockBME680Registers[static_cast<uint8_t>(registerAddress)];
#endif
    return result;
}

void sht40::writeRegister(registers registerAddress, uint8_t value) {
#ifndef generic
    HAL_I2C_Mem_Write(&hi2c2, i2cAddress << 1, static_cast<uint16_t>(registerAddress), I2C_MEMADD_SIZE_8BIT, &value, 1, halTimeout);
#else
    mockBME680Registers[static_cast<uint8_t>(registerAddress)] = value;
#endif
}

void sht40::readRegisters(uint16_t startAddress, uint16_t length, uint8_t* destination) {
#ifndef generic
    HAL_I2C_Mem_Read(&hi2c2, i2cAddress << 1, startAddress, I2C_MEMADD_SIZE_8BIT, destination, length, halTimeout);
#else
    memcpy(destination, mockBME680Registers + startAddress, length);
#endif
}

void sht40::log() {
    if (channels[temperature].hasNewValue) {
        logging::snprintf(logging::source::sensorData, "%s = %.2f *C\n", channelFormats[temperature].name, channels[temperature].getOutput());
    }
    if (channels[relativeHumidity].hasNewValue) {
        logging::snprintf(logging::source::sensorData, "%s = %.0f %s\n", channelFormats[relativeHumidity].name, channels[relativeHumidity].getOutput(), channelFormats[relativeHumidity].unit);
    }
}
