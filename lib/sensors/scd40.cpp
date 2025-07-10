// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <scd40.hpp>
#include <settingscollection.hpp>
#include <sensirion.hpp>

#ifndef generic
#include "main.h"
extern I2C_HandleTypeDef hi2c2;
#else
uint8_t mockSCD40Registers[256];
#include <cstring>

#endif

sensorDeviceState scd40::state{sensorDeviceState::unknown};
sensorChannel scd40::channels[nmbrChannels] = {
    {1, "temperature", "~C"},
    {0, "relativeHumidity", "%RH"},
    {0, "CO2", "ppm"},
};

uint32_t scd40::rawDataTemperature;
uint32_t scd40::rawDataRelativeHumidity;
uint32_t scd40::rawCo2;

bool scd40::isPresent() {
    return (testI2cAddress(i2cAddress));
}

void scd40::initialize() {
    for (uint32_t channelIndex = 0; channelIndex < nmbrChannels; channelIndex++) {
        channels[channelIndex].set(0, 0);
    }
    state = sensorDeviceState::sleeping;
}

void scd40::run() {
    if ((state == sensorDeviceState::sampling) && samplingIsReady()) {
        readSample();

        if (channels[temperature].needsSampling()) {
            float scd40Temperature = calculateTemperature(rawDataTemperature);
            channels[temperature].addSample(scd40Temperature);
        }
        if (channels[relativeHumidity].needsSampling()) {
            float scd40RelativeHumidity = calculateRelativeHumidity(rawDataRelativeHumidity);
            channels[relativeHumidity].addSample(scd40RelativeHumidity);
        }
        if (channels[co2].needsSampling()) {
            float scd40CO2 = calculateCO2(rawCo2);
            channels[co2].addSample(scd40CO2);
        }
        writeCommand(scd40::commands::stopPeriodicMeasurement);
        state = sensorDeviceState::sleeping;
    }
}

void scd40::startSampling() {
    writeCommand(scd40::commands::startPeriodicMeasurement);
    state = sensorDeviceState::sampling;
}

bool scd40::samplingIsReady() {
    uint16_t tmpResult;
    readCommand(scd40::commands::getDataReadyStatus, &tmpResult, 1U);
    return ((tmpResult & 0xF800) != 0);        // datasheet 3.8.2 : 11 least significant bits are zero -> data not ready
}

void scd40::readSample() {
    uint16_t tmpResult[3];
    readCommand(scd40::commands::readMeasurement, tmpResult, 3U);
    rawCo2                  = tmpResult[0];
    rawDataTemperature      = tmpResult[1];
    rawDataRelativeHumidity = tmpResult[2];
}

float scd40::calculateTemperature(uint32_t rawData) {
    float result{0.0F};
    result = -45.0F + ((175.0F * static_cast<float>(rawData)) / 65535.0F);
    return result;
}

float scd40::calculateRelativeHumidity(uint32_t rawData) {
    float result{0.0F};
    result = (100.0F * static_cast<float>(rawData)) / 65535.0F;
    return result;
}

float scd40::calculateCO2(uint32_t rawData) {
    return static_cast<float>(rawData);
}

bool scd40::testI2cAddress(uint8_t addressToTest) {
#ifndef generic
    bool result = (HAL_OK == HAL_I2C_IsDeviceReady(&hi2c2, static_cast<uint16_t>(addressToTest << 1), halTrials, halTimeout));
    return result;

#else
    return mockSCD40Present;
#endif
}

void scd40::writeCommand(scd40::commands aCommand) {
    uint16_t command = static_cast<uint16_t>(aCommand);
    uint8_t commandAsBytes[2];
    commandAsBytes[0] = static_cast<uint8_t>(command >> 8);
    commandAsBytes[1] = static_cast<uint8_t>(command & 0x00FF);
#ifndef generic
    HAL_I2C_Master_Transmit(&hi2c2, static_cast<uint16_t>(i2cAddress << 1), commandAsBytes, 2U, halTimeout);
#endif
}

void scd40::readCommand(scd40::commands aCommand, uint16_t* data, uint32_t dataLength) {
    uint8_t tmpData[dataLength * 3];
#ifndef generic
    HAL_I2C_Mem_Read(&hi2c2, static_cast<uint16_t>(i2cAddress << 1), static_cast<uint16_t>(aCommand), I2C_MEMADD_SIZE_16BIT, tmpData, static_cast<uint16_t>(dataLength * 3), halTimeout);
#endif
    for (uint32_t index = 0; index < dataLength; index++) {
        data[index] = sensirion::asUint16(&tmpData[index * 3]);
    }
}
