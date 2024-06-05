// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <sensirion.hpp>
#include <sps30.hpp>
#include <settingscollection.hpp>
#include <logging.hpp>
#include <cstring>

#ifndef generic
#include "main.h"
extern I2C_HandleTypeDef hi2c2;
#else
uint8_t mockSPS30Registers[14][60];
#endif

sensorDeviceState sps30::state{sensorDeviceState::unknown};
sensorChannel sps30::channels[nmbrChannels];
sensorChannelFormat sps30::channelFormats[nmbrChannels] = {
    {"pme 2.5", "uG", 0},
    {"pme 4", "uG", 0},
    {"pme 10", "uG", 0},
};

void sps30::wakeUp() {
    write(command::wakeUp);
    write(command::wakeUp);
}

bool sps30::isPresent() {
    // 1. Check if something is connected to the I2C bus at the address of the SPS30
    if (!testI2cAddress(i2cAddress)) {
        return false;
    }
    // 2. Check if it is a SPS30 by reading the string at address pointer 0xD002
    // static constexpr uint32_t signatureLength{12};
    // uint8_t signature[signatureLength];
    // uint8_t expectedSignature[signatureLength] = {'0', '0', 246, '0', '8', 79, '0', '0', 246, '0', '0', 246};        // "00080000" with CRCs
    // read(command::readProductType, signature, signatureLength);
    // return (memcmp(expectedSignature, signature, signatureLength) == 0);

    return true;
}

void sps30::initialize() {
    // TODO : need to read the sensorChannel settins from EEPROM and restore them
    channels[pme2dot5].set(0, 1, 0, 1);
    channels[pme4].set(0, 1, 0, 1);
    channels[pme10].set(0, 1, 0, 1);
    state = sensorDeviceState::sleeping;
}

bool sps30::hasNewMeasurement() {
    return (channels[pme2dot5].hasNewValue || channels[pme4].hasNewValue || channels[pme10].hasNewValue);
}

void sps30::clearNewMeasurements() {
    channels[pme2dot5].hasNewValue = false;
    channels[pme4].hasNewValue     = false;
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

        // if (channels[pme2dot5].needsSampling()) {
        //     float sps30Temperature = calculateTemperature();
        //     channels[pme2dot5].addSample(sps30Temperature);
        //     if (channels[pme2dot5].hasOutput()) {
        //         channels[pme2dot5].hasNewValue = true;
        //     }
        // }

        // if (channels[pme4].needsSampling()) {
        //     float sps30RelativeHumidity = calculateRelativeHumidity();
        //     channels[pme4].addSample(sps30RelativeHumidity);
        //     if (channels[pme4].hasOutput()) {
        //         channels[pme4].hasNewValue = true;
        //     }
        // }

        // if (channels[pme10].needsSampling()) {
        //     float sps30BarometricPressure = calculateBarometricPressure();
        //     channels[pme10].addSample(sps30BarometricPressure);
        //     if (channels[pme10].hasOutput()) {
        //         channels[pme10].hasNewValue = true;
        //     }
        // }

        state = sensorDeviceState::sleeping;
        adjustAllCounters();
    }
}

bool sps30::anyChannelNeedsSampling() {
    return (channels[pme2dot5].needsSampling() || channels[pme4].needsSampling() || channels[pme10].needsSampling());
}

void sps30::adjustAllCounters() {
    channels[pme2dot5].adjustCounters();
    channels[pme4].adjustCounters();
}

void sps30::startSampling() {
    static constexpr uint32_t commandDataLength{3};
    uint8_t commandData[commandDataLength]{5, 0, 246};        // datasheet section 6.3.1
    write(command::startMeasurement, commandData, commandDataLength);

    state = sensorDeviceState::sampling;
}

// void sps30::stopSampling() {
//     write(command::stopMeasurement, nullptr, 0);
//     state = sensorDeviceState::standby;
// }

bool sps30::samplingIsReady() {
    static constexpr uint32_t responseLength{3};
    uint8_t response[responseLength]{};
    uint8_t expectedResponse[responseLength] = {0, 1, 246};
    read(command::readProductType, response, responseLength);
    return (memcmp(expectedResponse, response, responseLength) == 0);
}

void sps30::stopSampling() {
    write(command::stopMeasurement, nullptr, 0);
    state = sensorDeviceState::standby;
}

void sps30::readSample() {
    static constexpr uint32_t responseLength{60};
    uint8_t response[responseLength]{};
    read(command::readProductType, response, responseLength);
    if (sensirion::checkCrc(response, responseLength)) {
        // crc ok
    } else {
        // crc error
    }

    // int16_t error;
    // uint8_t data[10][4];

    // error =
    //     sensirion_i2c_write_cmd(SPS30_I2C_ADDRESS, SPS_CMD_READ_MEASUREMENT);
    // if (error != NO_ERROR) {
    //     return error;
    // }

    // error = sensirion_i2c_read_words_as_bytes(SPS30_I2C_ADDRESS, &data[0][0],
    //                                           SENSIRION_NUM_WORDS(data));

    // if (error != NO_ERROR) {
    //     return error;
    // }

    // measurement->mc_1p0                = sensirion_bytes_to_float(data[0]);
    // measurement->mc_2p5                = sensirion_bytes_to_float(data[1]);
    // measurement->mc_4p0                = sensirion_bytes_to_float(data[2]);
    // measurement->mc_10p0               = sensirion_bytes_to_float(data[3]);
    // measurement->nc_0p5                = sensirion_bytes_to_float(data[4]);
    // measurement->nc_1p0                = sensirion_bytes_to_float(data[5]);
    // measurement->nc_2p5                = sensirion_bytes_to_float(data[6]);
    // measurement->nc_4p0                = sensirion_bytes_to_float(data[7]);
    // measurement->nc_10p0               = sensirion_bytes_to_float(data[8]);
    // measurement->typical_particle_size = sensirion_bytes_to_float(data[9]);
    // constexpr uint32_t nmbrRegisters{8};
    // uint8_t registerData[nmbrRegisters];
    // readData(0x1F, nmbrRegisters, registerData);        // reads 8 registers, from 0x1F up to 0x26, they contain the raw ADC results for temperature, relativeHumidity and pressure
    // rawDataTemperature        = ((static_cast<uint32_t>(registerData[3]) << 12) | (static_cast<uint32_t>(registerData[4]) << 4) | (static_cast<uint32_t>(registerData[5]) >> 4));
    // rawDataRelativeHumidity   = ((static_cast<uint32_t>(registerData[6]) << 8) | (static_cast<uint32_t>(registerData[7])));
    // rawDataBarometricPressure = ((static_cast<uint32_t>(registerData[0]) << 12) | (static_cast<uint32_t>(registerData[1]) << 4) | (static_cast<uint32_t>(registerData[2]) >> 4));
}

void sps30::goSleep() {
    write(command::goSleep, nullptr, 0);
    state = sensorDeviceState::sleeping;
}

bool sps30::testI2cAddress(uint8_t addressToTest) {
#ifndef generic
    return (HAL_OK == HAL_I2C_IsDeviceReady(&hi2c2, addressToTest << 1, halTrials, halTimeout));
#else
    return true;
#endif
}

void sps30::read(command theCommand, uint8_t* destination, uint16_t length) {
#ifndef generic
    //HAL_I2C_Mem_Read(&hi2c2, i2cAddress << 1, startAddress, I2C_MEMADD_SIZE_8BIT, destination, length, halTimeout);
#else
    // memcpy(destination, mockSPS30Registers + theCommand, length);
#endif
}

void sps30::write(command startAddress) {
#ifndef generic
    //HAL_I2C_Mem_Write(&hi2c2, i2cAddress << 1, static_cast<uint16_t>(registerAddress), I2C_MEMADD_SIZE_8BIT, &value, 1, halTimeout);
#else
    // mockSPS30Registers[static_cast<uint8_t>(registerAddress)] = value;
#endif
}

void sps30::write(command startAddress, uint8_t value) {
#ifndef generic
    //HAL_I2C_Mem_Write(&hi2c2, i2cAddress << 1, static_cast<uint16_t>(registerAddress), I2C_MEMADD_SIZE_8BIT, &value, 1, halTimeout);
#else
    // mockSPS30Registers[static_cast<uint8_t>(registerAddress)] = value;
#endif
}

void sps30::write(command startAddress, uint8_t* destination, uint16_t length) {
#ifndef generic
    //HAL_I2C_Mem_Write(&hi2c2, i2cAddress << 1, static_cast<uint16_t>(registerAddress), I2C_MEMADD_SIZE_8BIT, &value, 1, halTimeout);
#else
    // mockSPS30Registers[static_cast<uint8_t>(registerAddress)] = value;
#endif
}


void sps30::log() {
    // if (channels[temperature].hasNewValue) {
    //     logging::snprintf(logging::source::sensorData, "%s = %.2f *C\n", channelFormats[temperature].name, channels[temperature].getOutput());
    // }
    // if (channels[pme4].hasNewValue) {
    //     logging::snprintf(logging::source::sensorData, "%s = %.0f %s\n", channelFormats[pme4].name, channels[pme4].getOutput(), channelFormats[pme4].unit);
    // }
    // if (channels[pme10].hasNewValue) {
    //     logging::snprintf(logging::source::sensorData, "%s = %.0f %s\n", channelFormats[pme10].name, channels[pme10].getOutput(), channelFormats[pme10].unit);
    // }
}
