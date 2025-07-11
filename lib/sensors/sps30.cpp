// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <sensirion.hpp>
#include <sps30.hpp>
#include <cstring>

#ifndef generic
#include "main.h"
extern I2C_HandleTypeDef hi2c2;
#else
uint8_t mockSPS30Registers[14][60];
bool mockSPS30Present{false};
#endif

sensorDeviceState sps30::state{sensorDeviceState::unknown};
sensorChannel sps30::channels[nmbrChannels] = {
    {1, "pme 1", "uG"},
    {1, "pme 2.5", "uG"},
    {1, "pme 4", "uG"},
    {1, "pme 10", "uG"},
};

float sps30::_pme1{0.0F};
float sps30::_pme2dot5{0.0F};
float sps30::_pme4{0.0F};
float sps30::_pme10{0.0F};

bool sps30::isPresent() {
    return (testI2cAddress(i2cAddress));
}

void sps30::initialize() {
    state = sensorDeviceState::standby;
}

void sps30::startSampling() {
    static constexpr uint32_t commandDataLength{3};
    uint8_t commandData[commandDataLength]{5, 0, 246};        // datasheet section 6.3.1, using float output format as this has the best resolution
    write(command::startMeasurement, commandData, commandDataLength);
#ifndef generic
    HAL_Delay(20); // Datasheet table 8
#endif
    state = sensorDeviceState::sampling;
}

bool sps30::samplingIsReady() {
    static constexpr uint32_t responseLength{3};
    uint8_t response[responseLength]{};
    uint8_t expectedResponse[responseLength] = {0, 1, 176};
    write(command::isDataReady);
    read(response, responseLength);
    return (memcmp(expectedResponse, response, responseLength) == 0);        // Note : could be simpler by just looking at the second byte
}

void sps30::readSample() {
    static constexpr uint32_t responseLength{60};
    uint8_t response[responseLength]{};
    write(command::readMeasurement);
    read(response, responseLength);
    if (sensirion::checkCrc(response, responseLength)) {
        // crc ok
    } else {
        // crc error TODO
    }

    _pme1     = sensirion::asFloat(response);
    _pme2dot5 = sensirion::asFloat(response + 4);
    _pme4     = sensirion::asFloat(response + 8);
    _pme10    = sensirion::asFloat(response + 12);
}

void sps30::run() {
    if ((state == sensorDeviceState::sampling) && samplingIsReady()) {
        readSample();
        if (channels[pme1].needsSampling()) {
            channels[pme1].addSample(_pme1);
        }
        if (channels[pme2dot5].needsSampling()) {
            channels[pme2dot5].addSample(_pme2dot5);
        }

        if (channels[pme4].needsSampling()) {
            channels[pme4].addSample(_pme4);
        }

        if (channels[pme10].needsSampling()) {
            channels[pme10].addSample(_pme10);
        }
        state = sensorDeviceState::standby;
    }
}

void sps30::stopSampling() {
    write(command::stopMeasurement);
    #ifndef generic
    HAL_Delay(20); // Datasheet table 8
#endif

    state = sensorDeviceState::standby;
}

// void sps30::goSleep() {
//     write(command::goSleep, nullptr, 0);
//     state = sensorDeviceState::sleeping;
// }

bool sps30::testI2cAddress(uint8_t addressToTest) {
#ifndef generic
    return (HAL_OK == HAL_I2C_IsDeviceReady(&hi2c2, static_cast<uint16_t>(addressToTest << 1), halTrials, halTimeout));
#else
    return true;
#endif
}

void sps30::write(const sps30::command aCommand) {        // Datasheet 6.1, they call it 'Set Pointer'
    uint16_t command = static_cast<uint16_t>(aCommand);
    uint8_t commandAsBytes[2];
    commandAsBytes[0] = static_cast<uint8_t>(command >> 8);
    commandAsBytes[1] = static_cast<uint8_t>(command & 0x00FF);
#ifndef generic
    for (uint32_t trials = 0; trials < halTrials; trials++) {
        HAL_StatusTypeDef result;
        result = HAL_I2C_Master_Transmit(&hi2c2, static_cast<uint16_t>(i2cAddress << 1), commandAsBytes, 2U, halTimeout);
        if (result == HAL_OK) {
            return;
        }
        HAL_Delay(halTimeout);
    }
#endif
}

void sps30::write(const command aCommand, const uint8_t* sourceData, const uint32_t lengthInBytes) {        // Datasheet 6.1, they call it 'Set Pointer and Write Data'
    uint16_t command = static_cast<uint16_t>(aCommand);
    uint8_t commandAsBytes[lengthInBytes + 2];
    commandAsBytes[0] = static_cast<uint8_t>(command >> 8);
    commandAsBytes[1] = static_cast<uint8_t>(command & 0x00FF);
    memcpy(commandAsBytes + 2, sourceData, lengthInBytes);
#ifndef generic
    for (uint32_t trials = 0; trials < halTrials; trials++) {
        HAL_StatusTypeDef result;
        result = HAL_I2C_Master_Transmit(&hi2c2, static_cast<uint16_t>(i2cAddress << 1), commandAsBytes, static_cast<uint16_t>(lengthInBytes + 2U), halTimeout);
        if (result == HAL_OK) {
            return;
        }
        HAL_Delay(halTimeout);
    }
#endif
}

void sps30::read(uint8_t* destinationData, const uint32_t lengthInBytes) {        // This is the reading part of 'Set Pointer and Read Data'
#ifndef generic
    for (uint32_t trials = 0; trials < halTrials; trials++) {
        HAL_StatusTypeDef result;
        result = HAL_I2C_Master_Receive(&hi2c2, static_cast<uint16_t>(i2cAddress << 1), destinationData, static_cast<uint16_t>(lengthInBytes), halTimeout);
        if (result == HAL_OK) {
            break;
        }
        HAL_Delay(halTimeout);
    }
#endif
}
