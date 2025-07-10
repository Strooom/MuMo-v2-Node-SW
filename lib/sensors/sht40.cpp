// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################
#include <sensordevicetype.hpp>
#include <sht40.hpp>
#include <settingscollection.hpp>
#include <float.hpp>
#include <logging.hpp>
#include <i2c.hpp>

#ifndef generic
#include "main.h"
extern I2C_HandleTypeDef hi2c2;
#else
uint8_t mockSHT40Registers[256];
bool mockSHT40Present{false};
#include <cstring>

#endif

uint8_t sht40::i2cAddress;
sensorDeviceState sht40::state{sensorDeviceState::unknown};
sensorChannel sht40::channels[nmbrChannels] = {
    {1, "temperature", "~C"},
    {0, "relativeHumidity", "%RH"},
};

uint32_t sht40::rawDataTemperature;
uint32_t sht40::rawDataRelativeHumidity;
uint32_t sht40::measurementStartTick;

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
    for (uint32_t channelIndex = 0; channelIndex < nmbrChannels; channelIndex++) {
        channels[channelIndex].set(0, 0);
    }
    state = sensorDeviceState::sleeping;
}

void sht40::run() {
    if ((state == sensorDeviceState::sampling) && samplingIsReady()) {
        readSample();
        if (channels[temperature].needsSampling()) {
            float sht40Temperature = calculateTemperature();
            channels[temperature].addSample(sht40Temperature);
        }
        if (channels[relativeHumidity].needsSampling()) {
            float sht40RelativeHumidity = calculateRelativeHumidity();
            channels[relativeHumidity].addSample(sht40RelativeHumidity);
        }
        state = sensorDeviceState::sleeping;
    }
}

void sht40::startSampling() {
    write(command::getMeasurementHighPrecision);
#ifndef generic
    measurementStartTick = HAL_GetTick();
#else
    measurementStartTick = 0;
#endif
    state = sensorDeviceState::sampling;
}

bool sht40::samplingIsReady() {
#ifndef generic
    bool ready = ((HAL_GetTick() - measurementStartTick) > measurementDurationInTicks);
    return ready;
#else
    return true;
#endif
}

void sht40::readSample() {
    static constexpr uint32_t responseLength{6};
    uint8_t response[responseLength];
    read(response, responseLength);        // SH4x datasheet section 4.5
    if (sensirion::checkCrc(response, responseLength)) {
        rawDataTemperature      = sensirion::asUint16(response);
        rawDataRelativeHumidity = sensirion::asUint16(response + 3);
    }
}

float sht40::calculateTemperature() {
    return -45.0F + 175.0F * (static_cast<float>(rawDataTemperature) / 65535.0F);        // SH4x datasheet section 4.6 (2)
}

float sht40::calculateRelativeHumidity() {
    float result;
    result = -6.0F + 125.0F * (static_cast<float>(rawDataRelativeHumidity) / 65535.0F);        // SH4x datasheet section 4.6 (1)
    if (result < 0.0F) {
        result = 0.0F;
    }
    if (result > 100.0F) {
        result = 100.0F;
    }
    return result;
}

bool sht40::testI2cAddress(uint8_t addressToTest) {
#ifndef generic
    return (HAL_OK == HAL_I2C_IsDeviceReady(&hi2c2, static_cast<uint16_t>(addressToTest << 1), halTrials, halTimeout));
#else
    return mockSHT40Present;
#endif
}

void sht40::write(command aCommand) {
    uint8_t pCommand = static_cast<uint8_t>(aCommand);
#ifndef generic
    HAL_I2C_Master_Transmit(&hi2c2, static_cast<uint16_t>(i2cAddress << 1), &pCommand, 1, halTimeout);
#else

#endif
}

void sht40::read(uint8_t* response, uint32_t responseLength) {
#ifndef generic
    HAL_I2C_Master_Receive(&hi2c2, static_cast<uint16_t>(i2cAddress << 1), response, static_cast<uint16_t>(responseLength), halTimeout);
#else
    (void)memcpy(response, mockSHT40Registers, responseLength);
#endif
}
