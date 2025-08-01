// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <sensordevicetype.hpp>
#include <tsl2591.hpp>
#include <settingscollection.hpp>
#include <logging.hpp>
#include <float.hpp>
#include <i2c.hpp>

#ifndef generic
#include <main.h>
extern I2C_HandleTypeDef hi2c2;
#else
#include <cstring>
uint8_t mockTSL2591Registers[256];
bool mockTSL2591Present{false};
#endif

sensorDeviceState tsl2591::state{sensorDeviceState::unknown};
sensorChannel tsl2591::channels[nmbrChannels]{
    {0, "visibleLight", "lux"},
};

int32_t tsl2591::rawChannel0{0};
int32_t tsl2591::rawChannel1{0};
uint32_t tsl2591::cleaningSampleIndex{0};

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
    writeRegister(registers::enable, powerOn);
    writeRegister(registers::config, 0x11);
    for (uint32_t channelIndex = 0; channelIndex < nmbrChannels;
         channelIndex++) {
        channels[channelIndex].set(0, 0);
    }
    goSleep();
}

void tsl2591::goSleep() {
    writeRegister(registers::enable, powerOff);
    state = sensorDeviceState::sleeping;
}

void tsl2591::startSampling() {
    cleaningSampleIndex = 0;
    writeRegister(registers::enable, powerOn);
    writeRegister(registers::config, 0x11);
    writeRegister(registers::enable, 0x03);
    state = sensorDeviceState::sampling;
}

bool tsl2591::samplingIsReady() {
#ifndef generic
    bool deviceIsReady   = (readRegister(registers::status) & 0x01) == 0x01;
    bool cleaningIsReady = cleaningSampleIndex >= nmbrOfCleaningSamples;
    return deviceIsReady && cleaningIsReady;
#else
    return true;
#endif
}

void tsl2591::readSample() {
    // CHAN0 must be read before CHAN1 See: https://forums.adafruit.com/viewtopic.php?f=19&t=124176
    rawChannel0 = (readRegister(registers::c0datah) << 8) + readRegister(registers::c0datal);
    rawChannel1 = (readRegister(registers::c1datah) << 8) + readRegister(registers::c1datal);
}

float tsl2591::calculateLux() {
    float lux = (static_cast<float>(rawChannel0) * coefficient1) + (static_cast<float>(rawChannel0) * coefficient2);
    return lux;
}

bool tsl2591::testI2cAddress(uint8_t addressToTest) {
#ifndef generic
    return (HAL_OK == HAL_I2C_IsDeviceReady(&hi2c2, static_cast<uint16_t>(addressToTest << 1), halTrials, halTimeout));
#else
    return mockTSL2591Present;
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

void tsl2591::run() {
    bool deviceIsReady = (readRegister(registers::status) & 0x01) == 0x01;
    if ((state == sensorDeviceState::sampling) && deviceIsReady) {
        if (cleaningSampleIndex < nmbrOfCleaningSamples) {
            readSample();
            writeRegister(registers::enable, 0x03);
            cleaningSampleIndex++;
        } else {
            readSample();
            if (channels[visibleLight].needsSampling()) {
                channels[visibleLight].addSample(calculateLux());
            }
            goSleep();
        }
    }
}
