// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <tsl2591.hpp>
#include <settingscollection.hpp>
#include <logging.hpp>

#ifndef generic
#include <main.h>
extern I2C_HandleTypeDef hi2c2;
#else
#include <cstring>
uint8_t mockTSL2591Registers[256];
#endif

sensorDeviceState tsl2591::state{sensorDeviceState::unknown};
sensorChannel tsl2591::channels[nmbrChannels];
sensorChannelFormat tsl2591::channelFormats[nmbrChannels] =
    {
        {"visibleLight", "lux", 0},
};
int32_t tsl2591::rawChannel0{0};
int32_t tsl2591::rawChannel1{0};

bool tsl2591::isPresent() {
    // 1. Check if something is connected to the I2C bus at the address of the bme680
    if (!testI2cAddress(i2cAddress)) {
        return false;
    }
    // 2. Check if it is a TSL2591 by reading the chip id register/value
    uint8_t chipidValue = readRegister(tsl2591::registers::id);
    return (tsl2591::chipIdValue == chipidValue);
}

void tsl2591::initialize() {
    channels[visibleLight].set(0, 1, 0, 1);
    // TODO : need to read the sensorChannel settins from EEPROM and restore them

    writeRegister(registers::enable, powerOn);
    writeRegister(registers::config, 0x01);        // gain 1x, integration time 200 ms. I found out these fixed settings are more than sufficient for my use case

    goSleep();
}

uint32_t tsl2591::nmbrOfNewMeasurements() {
    uint32_t count{0};
    for (uint32_t channelIndex = 0; channelIndex < nmbrChannels; channelIndex++) {
        if (channels[channelIndex].hasNewValue) {
            count++;
        }
    }
    return count;
}

float tsl2591::valueAsFloat(uint32_t index) {
    return channels[index].getOutput();
}

bool tsl2591::hasNewMeasurement() {
    return (channels[visibleLight].hasNewValue);
}

bool tsl2591::hasNewMeasurement(uint32_t channelIndex) {
    return channels[channelIndex].hasNewValue;
}

void tsl2591::clearNewMeasurements() {
    channels[visibleLight].hasNewValue = false;
}

void tsl2591::goSleep() {
    writeRegister(registers::enable, powerOff);
    state = sensorDeviceState::sleeping;
}

void tsl2591::startSampling() {
    writeRegister(registers::enable, 0x03);
    state = sensorDeviceState::sampling;
}

bool tsl2591::samplingIsReady() {
    return (readRegister(registers::status) & 0x01) == 0x01;
}

void tsl2591::readSample() {
    // CHAN0 must be read before CHAN1 See: https://forums.adafruit.com/viewtopic.php?f=19&t=124176
    rawChannel0 = (readRegister(registers::c0datah) << 8) + readRegister(registers::c0datal);
    rawChannel1 = (readRegister(registers::c1datah) << 8) + readRegister(registers::c1datal);
}

bool tsl2591::anyChannelNeedsSampling() {
    return (channels[visibleLight].needsSampling());
}

void tsl2591::adjustAllCounters() {
    channels[visibleLight].adjustCounters();
}

float tsl2591::calculateLux() {
    float CPL  = 200.0F / 53.0F;
    float Lux1 = (rawChannel0 - (2 * rawChannel1)) / CPL;
    float Lux2 = ((0.6F * rawChannel0) - rawChannel1) / CPL;

    float Lux = 0.0F;
    if (Lux1 > Lux) {
        Lux = Lux1;
    }
    if (Lux2 > Lux) {
        Lux = Lux2;
    }

    return Lux;
}

bool tsl2591::testI2cAddress(uint8_t addressToTest) {
#ifndef generic
    return (HAL_OK == HAL_I2C_IsDeviceReady(&hi2c2, addressToTest << 1, halTrials, halTimeout));
#else
    return true;
#endif
}

uint8_t tsl2591::readRegister(registers registerAddress) {
    uint16_t command = commandMask | static_cast<uint16_t>(registerAddress);
    uint8_t result;

#ifndef generic
    HAL_I2C_Mem_Read(&hi2c2, i2cAddress << 1, command, I2C_MEMADD_SIZE_8BIT, &result, 1, halTimeout);
#else
    result = mockTSL2591Registers[static_cast<uint8_t>(registerAddress)];
#endif
    return result;
}

void tsl2591::writeRegister(registers registerAddress, uint8_t value) {
    uint16_t command = commandMask | static_cast<uint16_t>(registerAddress);
#ifndef generic
    HAL_I2C_Mem_Write(&hi2c2, i2cAddress << 1, command, I2C_MEMADD_SIZE_8BIT, &value, 1, halTimeout);
#else
    mockTSL2591Registers[static_cast<uint8_t>(registerAddress)] = value;
#endif
}

void tsl2591::tick() {
    if (state != sensorDeviceState::sleeping) {
        adjustAllCounters();
        return;
    }

    if (anyChannelNeedsSampling()) {
        startSampling();
        state = sensorDeviceState::sampling;
    } else {
        adjustAllCounters();
    }
}

void tsl2591::run() {
    if ((state == sensorDeviceState::sampling) && samplingIsReady()) {
        readSample();

        if (channels[visibleLight].needsSampling()) {
            float tsl2591visible = calculateLux();
            channels[visibleLight].addSample(tsl2591visible);
            if (channels[visibleLight].hasOutput()) {
                channels[visibleLight].hasNewValue = true;
            }
        }

        state = sensorDeviceState::sleeping;
        adjustAllCounters();
    }
}

void tsl2591::log() {
    if (channels[visibleLight].hasNewValue) {
        //        logging::snprintf(logging::source::sensorData, "%s = %.2f %s\n", channelFormats[visibleLight].name, channels[visibleLight].getOutput(), channelFormats[visibleLight].unit);
        logging::snprintf(logging::source::sensorData, "%s = ... %s\n", channelFormats[visibleLight].name, channelFormats[visibleLight].unit);
    }
}

void tsl2591::saveNewMeasurementsToEeprom() {
}
