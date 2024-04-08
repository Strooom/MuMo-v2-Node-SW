// ######################################################################################
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
    static constexpr uint32_t responseLength{6};
    uint8_t response[responseLength];
    read(command::getMeasurementHighPrecision, response, responseLength);        // SH4x datasheet section 4.5
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

void sht40::read(command aCommand, uint8_t* response, uint32_t responseLength) {
#ifndef generic
    HAL_I2C_Mem_Read(&hi2c2, i2cAddress << 1, static_cast<uint8_t>(aCommand), I2C_MEMADD_SIZE_8BIT, response, responseLength, halTimeout);
#else
// TODO add mock for generic Unit testing
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
