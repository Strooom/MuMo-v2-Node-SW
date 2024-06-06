// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################
#include <sensordevicetype.hpp>
#include <sht40.hpp>
#include <settingscollection.hpp>
#include <float.hpp>
#include <logging.hpp>
#include <measurementcollection.hpp>

#ifndef generic
#include "main.h"
extern I2C_HandleTypeDef hi2c2;
#else
uint8_t mockSHT40Registers[256];
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
    channels[temperature].set(0, 1, 0, 1);
    channels[relativeHumidity].set(0, 1, 0, 1);
    state = sensorDeviceState::sleeping;
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
        // clearNewMeasurements(); TODO : do we need this ?
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
    return (HAL_GetTick() - measurementStartTick) > measurementDurationInTicks;
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
    return (HAL_OK == HAL_I2C_IsDeviceReady(&hi2c2, addressToTest << 1, halTrials, halTimeout));
#else
    return true;
#endif
}

void sht40::write(command aCommand) {
    uint8_t pCommand = static_cast<uint8_t>(aCommand);
#ifndef generic
    HAL_I2C_Master_Transmit(&hi2c2, i2cAddress << 1, &pCommand, 1, halTimeout);
#else
// TODO add mock for generic Unit testing
#endif
}

void sht40::read(uint8_t* response, uint32_t responseLength) {
#ifndef generic
    HAL_I2C_Master_Receive(&hi2c2, i2cAddress << 1, response, responseLength, halTimeout);
#else
    (void)memcpy(response, mockSHT40Registers, responseLength);
#endif
}


void sht40::addNewMeasurements() {
    for (uint32_t channelIndex = 0; channelIndex < nmbrChannels; channelIndex++) {
        if (channels[channelIndex].hasNewValue) {
            measurementCollection::addMeasurement(static_cast<uint32_t>(sensorDeviceType::sht40), channelIndex, channels[channelIndex].getOutput());
        }
    }
}
