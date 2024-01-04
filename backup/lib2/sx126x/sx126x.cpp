// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "sx126x.h"
#include "logging.hpp"

#ifndef generic
#include "main.h"
extern SUBGHZ_HandleTypeDef hsubghz;
extern RTC_HandleTypeDef hrtc;
#else
#include <cstring>
extern uint8_t mockSX126xDataBuffer[256];            // unitTesting mock for the LoRa Rx/Tx buffer, inside the SX126x
extern uint8_t mockSX126xRegisters[0x1000];          // unitTesting mock for the config registers, inside the SX126x
extern uint8_t mockSX126xCommandData[256][8];        // unitTesting mock for capturing the commands and their parameters to configure the SX126x
#endif

void sx126x::initialize() {
    initializeInterface();
    initializeRadio();
    goSleep();
}

void sx126x::configForTransmit(spreadingFactor theSpreadingFactor, uint32_t frequency, uint8_t* payload, uint32_t payloadLength) {
    logging::snprintf(logging::source::sx126xControl, "Configuring for Transmit : [%s], [%u] Hz [%u] bytes\n", toString(theSpreadingFactor), frequency, payloadLength);
    goStandby();
    setRfSwitch(rfSwitchState::tx);
    setRfFrequency(frequency);
    setModulationParameters(theSpreadingFactor);
    setPacketParametersTransmit(payloadLength);
    writeBuffer(payload, payloadLength);
    uint8_t commandParameters[8];
    commandParameters[0] = 0x02;
    commandParameters[1] = 0x01;
    commandParameters[2] = 0x02;
    commandParameters[3] = 0x01;
    commandParameters[4] = 0x00;
    commandParameters[5] = 0x00;
    commandParameters[6] = 0x00;
    commandParameters[7] = 0x00;
    executeSetCommand(command::setDioIrqParams, commandParameters, 8);
}

void sx126x::configForReceive(spreadingFactor theSpreadingFactor, uint32_t frequency) {
    logging::snprintf(logging::source::sx126xControl, "Configuring for Receive : [%s], [%u] Hz\n", toString(theSpreadingFactor), frequency);
    goStandby();
    setRfSwitch(rfSwitchState::rx);
    setRfFrequency(frequency);
    setModulationParameters(theSpreadingFactor);
    setPacketParametersReceive();
    uint8_t commandParameters[8];
    commandParameters[0] = 0x02;
    commandParameters[1] = 0x02;
    commandParameters[2] = 0x02;
    commandParameters[3] = 0x02;
    commandParameters[4] = 0x00;
    commandParameters[5] = 0x00;
    commandParameters[6] = 0x00;
    commandParameters[7] = 0x00;
    executeSetCommand(command::setDioIrqParams, commandParameters, 8);
}

void sx126x::goSleep(sleepMode theSleepMode) {
    logging::snprintf(logging::source::sx126xControl, "goSleep [%u]\n", static_cast<uint8_t>(theSleepMode));
    setRfSwitch(rfSwitchState::off);
    constexpr uint8_t nmbrCommandParameters{1};
    uint8_t commandParameters[nmbrCommandParameters]{static_cast<uint8_t>(theSleepMode)};
    executeSetCommand(command::setSleep, commandParameters, nmbrCommandParameters);
}

void sx126x::goStandby(standbyMode theStandbyMode) {
    logging::snprintf(logging::source::sx126xControl, "goStandby [%u]\n", static_cast<uint8_t>(theStandbyMode));
    constexpr uint8_t nmbrCommandParameters{1};
    uint8_t commandParameters[nmbrCommandParameters]{static_cast<uint8_t>(theStandbyMode)};
    executeSetCommand(command::setStandby, commandParameters, nmbrCommandParameters);
}

void sx126x::startTransmit(uint32_t timeOut) {
    logging::snprintf(logging::source::sx126xControl, "startTx [%u]\n", timeOut);
    constexpr uint8_t nmbrCommandParameters{3};
    uint8_t commandParameters[nmbrCommandParameters];
    commandParameters[0] = static_cast<uint8_t>((timeOut >> 16) & 0xFF);
    commandParameters[1] = static_cast<uint8_t>((timeOut >> 8) & 0xFF);
    commandParameters[2] = static_cast<uint8_t>(timeOut & 0xFF);
    executeSetCommand(command::setTx, commandParameters, nmbrCommandParameters);
}

void sx126x::startReceive(uint32_t timeOut) {
    timeOut = 3840;        // TODO : make this a funtion of the DataRate.. currently it is fixed to 60 ms, which should be ok for DR5
    logging::snprintf(logging::source::sx126xControl, "startRx [%u]\n", timeOut);
    constexpr uint8_t nmbrCommandParameters{3};
    uint8_t commandParameters[nmbrCommandParameters];
    commandParameters[0] = static_cast<uint8_t>((timeOut >> 16) & 0xFF);
    commandParameters[1] = static_cast<uint8_t>((timeOut >> 8) & 0xFF);
    commandParameters[2] = static_cast<uint8_t>(timeOut & 0xFF);
    executeSetCommand(command::setRx, commandParameters, nmbrCommandParameters);
}

void sx126x::setRfFrequency(uint32_t frequencyInHz) {
    logging::snprintf(logging::source::sx126xControl, "setRfFrequency [%u]\n", frequencyInHz);
    constexpr uint8_t nmbrCommandParameters{4};
    uint32_t frequencyRegisterValue = calculateFrequencyRegisterValue(frequencyInHz);
    uint8_t commandParameters[nmbrCommandParameters];
    commandParameters[0] = static_cast<uint8_t>((frequencyRegisterValue >> 24) & 0xFF);        //
    commandParameters[1] = static_cast<uint8_t>((frequencyRegisterValue >> 16) & 0xFF);        //
    commandParameters[2] = static_cast<uint8_t>((frequencyRegisterValue >> 8) & 0xFF);         //
    commandParameters[3] = static_cast<uint8_t>((frequencyRegisterValue)&0xFF);                //
    executeSetCommand(command::setRfFRequency, commandParameters, nmbrCommandParameters);
}

void sx126x::setModulationParameters(spreadingFactor theSpreadingFactor) {
    logging::snprintf(logging::source::sx126xControl, "setModulationParameters [%s]\n", toString(theSpreadingFactor));
    constexpr uint8_t nmbrCommandParameters{8};
    uint8_t commandParameters[nmbrCommandParameters]{0};
    commandParameters[0] = static_cast<uint8_t>(theSpreadingFactor);        //
    commandParameters[1] = 0x4;                                             // LoRaWAN bandwidth is fixed to 125 kHz
    commandParameters[2] = 0x01;                                            // LoRaWAN is fixed to coding rate 4/5
    commandParameters[3] = 0x00;                                            // TODO ?? LowDatarateOptimize // why would you NOT want to do this ??
                                                                            // the remaining 4 bytes are empty 0x00 for LoRa modulation
    executeSetCommand(command::setModulationParams, commandParameters, nmbrCommandParameters);
}

void sx126x::setPacketParametersTransmit(uint8_t payloadLength) {
    logging::snprintf(logging::source::sx126xControl, "setPacketParametersTransmit [%u]\n", payloadLength);
    constexpr uint8_t nmbrCommandParameters{9};
    uint8_t commandParameters[nmbrCommandParameters]{0};
    commandParameters[0] = 0x00;                 // MSB for PreambleLength
    commandParameters[1] = 0x08;                 // LSB for PreambleLength LoRaWAN is fixed to 8 symbols
    commandParameters[2] = 0x00;                 // Variable length packet (explicit header)
    commandParameters[3] = payloadLength;        // Payload Length
    commandParameters[4] = 0x01;                 // CRC On
    commandParameters[5] = 0x00;                 // Standard IQ for uplink
                                                 // the remaining 3 bytes are empty 0x00 for LoRa
    executeSetCommand(command::setPacketParameters, commandParameters, nmbrCommandParameters);
}

void sx126x::setPacketParametersReceive() {
    logging::snprintf(logging::source::sx126xControl, "setPacketParametersReceive\n");
    constexpr uint8_t nmbrCommandParameters{9};
    uint8_t commandParameters[nmbrCommandParameters]{0};
    commandParameters[0] = 0x00;        // MSB for PreambleLength
    commandParameters[1] = 0x08;        // LSB for PreambleLength LoRaWAN is fixed to 8 symbols
    commandParameters[2] = 0x00;        // Variable length packet (explicit header)
    commandParameters[3] = 0xFF;        // Maximum Payload Length
    commandParameters[4] = 0x00;        // CRC Off
    commandParameters[5] = 0x01;        // Downlink requires inverted IQ
                                        // the remaining 3 bytes are empty 0x00 for LoRa
    executeSetCommand(command::setPacketParameters, commandParameters, nmbrCommandParameters);
}

uint32_t sx126x::calculateFrequencyRegisterValue(uint32_t rfFrequency) {
    uint64_t tmpResult;
    tmpResult = static_cast<uint64_t>(rfFrequency) << 25;
    tmpResult = tmpResult / TCXOfrequency;
    return static_cast<uint32_t>(tmpResult);
}

void sx126x::setTxParameters(int8_t transmitPowerdBm) {        // caution - signed int8, negative dBm values are in two's complement
    logging::snprintf(logging::source::sx126xControl, "setTxParameters [%d]\n", transmitPowerdBm);
    constexpr uint8_t nmbrCommandParameters{2};
    uint8_t commandParameters[nmbrCommandParameters];

    union {
        int8_t asInt8;
        uint8_t asUint8;
    } txPower;

    txPower.asInt8 = transmitPowerdBm;

    commandParameters[0] = txPower.asUint8;
    commandParameters[1] = 0x02;        // rampTime 40 uS - no info why this value, but this was in the demo application from ST / Semtech, other examples use 200 uS ??
                                        // the remaining 4 bytes are empty 0x00 for LoRa
    executeSetCommand(command::setTxParams, commandParameters, nmbrCommandParameters);
}

void sx126x::initializeRadio() {
    uint8_t commandParameters[8]{0};        // contains parameters when sending a command to the SX126x

    setRfSwitch(rfSwitchState::off);

    commandParameters[0] = 0x01;        // DCDC powermode
    executeSetCommand(command::setRegulatorMode, commandParameters, 1);

    // TODO : add SMPS maximum drive capability 60 mA (io 100mA default)
    //    commandParameters[0] = 0x02;
    //    writeRegisters(register::smpsMaximumDrive, commandParameters, 1);

    commandParameters[0] = 0x1E;
    writeRegisters(registerAddress ::TxClampConfig, commandParameters, 1);

    commandParameters[0] = 0x38;
    writeRegisters(registerAddress ::OCPConfiguration, commandParameters, 1);

    commandParameters[0] = 0x00;
    writeRegisters(registerAddress ::XTAtrim, commandParameters, 1);

    commandParameters[0] = 0x01;
    commandParameters[1] = 0x00;
    commandParameters[2] = 0x00;
    commandParameters[3] = 0x40;
    executeSetCommand(command::setDIO3AsTcxoCtrl, commandParameters, 4);

    commandParameters[0] = 0x7F;
    executeSetCommand(command::calibrate, commandParameters, 1);

    commandParameters[0] = 0xD7;
    commandParameters[1] = 0xDB;
    executeSetCommand(command::calibrateImage, commandParameters, 2);

    commandParameters[0] = 0x01;        // packetType::LoRa;
    executeSetCommand(command::setPacketType, commandParameters, 1);

    commandParameters[0] = 0x36;
    commandParameters[1] = 0x41;
    commandParameters[2] = 0x99;
    commandParameters[3] = 0x99;
    executeSetCommand(command::setRfFRequency, commandParameters, 4);

    commandParameters[0] = 0x02;
    commandParameters[1] = 0x03;
    commandParameters[2] = 0x00;
    commandParameters[3] = 0x01;
    executeSetCommand(command::setPaConfig, commandParameters, 4);

    commandParameters[0] = 0x0D;        // PA optimal setting and operating modes: 14 dBm HighPower mode
    commandParameters[1] = 0x02;        // RampTime
    executeSetCommand(command::setTxParams, commandParameters, 2);

    commandParameters[0] = 0x34;        // LoRa Syncword MSB
    commandParameters[1] = 0x44;        // LoRa Syncword LSB
    writeRegisters(registerAddress ::LoRaSyncWordMSB, commandParameters, 2);

    // goSleep();

    // executeGetCommand(command::getStatus, commandParameters, 1);
}

void sx126x::initializeInterface() {
#ifndef generic
    HAL_SUBGHZ_Init(&hsubghz);
#endif
}

void sx126x::setRfSwitch(rfSwitchState newState) {
#ifndef generic
    switch (newState) {
        case rfSwitchState::off:
        default:
            HAL_GPIO_WritePin(GPIOA, rfControl1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOA, rfControl2_Pin, GPIO_PIN_RESET);
            break;

        case rfSwitchState::tx:
            HAL_GPIO_WritePin(GPIOA, rfControl1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOA, rfControl2_Pin, GPIO_PIN_SET);
            break;

        case rfSwitchState::rx:
            HAL_GPIO_WritePin(GPIOA, rfControl1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOA, rfControl2_Pin, GPIO_PIN_RESET);
            break;
    }
#endif
}

void sx126x::writeBuffer(uint8_t* payload, uint32_t payloadLength) {
#ifndef generic
    HAL_SUBGHZ_WriteBuffer(&hsubghz, 0U, payload, payloadLength);        // copy the raw LoRa message into the transmitBuffer of the SX126
#else
    memcpy(mockSX126xDataBuffer, payload, payloadLength);
#endif
}

void sx126x::readBuffer(uint8_t* payload, uint32_t payloadLength) {
#ifndef generic
    HAL_SUBGHZ_ReadBuffer(&hsubghz, 0U, payload, payloadLength);        // read the raw LoRa message which has been received from the SX126 into the receiveBuffer of the LoRaWAN stack
#else
    memcpy(payload, mockSX126xDataBuffer, payloadLength);
#endif
}

void sx126x::executeSetCommand(command theCommand, uint8_t* commandParameters, uint8_t commandParametersLength) {
#ifndef generic
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, static_cast<SUBGHZ_RadioSetCmd_t>(theCommand), commandParameters, commandParametersLength);
#else
    memcpy(mockSX126xCommandData[static_cast<uint8_t>(theCommand)], commandParameters, commandParametersLength);
#endif
}

void sx126x::executeGetCommand(command theCommand, uint8_t* responseData, uint8_t responseDataLength) {
#ifndef generic
    HAL_SUBGHZ_ExecGetCmd(&hsubghz, static_cast<SUBGHZ_RadioGetCmd_t>(theCommand), responseData, responseDataLength);
#else
    memcpy(responseData, mockSX126xCommandData[static_cast<uint8_t>(theCommand)], responseDataLength);
#endif
}

void sx126x::writeRegisters(registerAddress theRegister, uint8_t* data, uint8_t dataLength) {
#ifndef generic
    HAL_SUBGHZ_WriteRegisters(&hsubghz, static_cast<uint16_t>(theRegister), data, dataLength);
#else
    memcpy(&mockSX126xRegisters[static_cast<uint16_t>(theRegister)], data, dataLength);
#endif
}

void sx126x::readRegisters(registerAddress theRegister, uint8_t* data, uint8_t dataLength) {
#ifndef generic
    HAL_SUBGHZ_ReadRegisters(&hsubghz, static_cast<uint16_t>(theRegister), data, dataLength);
#else
    memcpy(data, &mockSX126xRegisters[static_cast<uint16_t>(theRegister)], dataLength);
#endif
}
