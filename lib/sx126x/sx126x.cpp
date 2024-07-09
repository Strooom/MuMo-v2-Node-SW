// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <sx126x.hpp>
#include <logging.hpp>
#include <settingscollection.hpp>
#include <cstring>

#ifndef generic
#include "main.h"
extern SUBGHZ_HandleTypeDef hsubghz;
extern RTC_HandleTypeDef hrtc;
#else
uint8_t mockSX126xDataBuffer[256];
uint8_t mockSX126xRegisters[0x1000];
uint8_t mockSX126xCommandData[256][8];
#include <cstring>
#endif

mcuType sx126x::thePowerVersion{mcuType::nmbrPowerVersions};

void sx126x::setType(uint8_t index) {
    if (index < static_cast<uint8_t>(mcuType::nmbrPowerVersions)) {
        thePowerVersion = static_cast<mcuType>(index);
    }
}

bool sx126x::isValidType() {
    return (thePowerVersion < mcuType::nmbrPowerVersions);
}

void sx126x::initialize(mcuType isPowerVersion) {
    thePowerVersion = isPowerVersion;
    initializeInterface();
    initializeRadio();
    goSleep();
}

uint8_t sx126x::getStatus() {
    uint8_t response[1];
    sx126x::executeGetCommand(sx126x::command::getStatus, response, 1);
    return response[0];
}

float sx126x::getPacketSnr() {
    // RssiPkt = (-1 * response[0])/2;
    // SnrPkt = response[1] / 4;
    // SignalRssiPkt = -response[2]/2;

    uint8_t response[3];
    sx126x::executeGetCommand(sx126x::command::getPacketStatus, response, 3);

    int8_t snr;
    std::memcpy(&snr, &response[1], 1);

    return (static_cast<float>(snr) / 4.0f);
}

void sx126x::configForTransmit(spreadingFactor theSpreadingFactor, uint32_t frequency, uint8_t* payload, uint32_t payloadLength) {
    logging::snprintf(logging::source::sx126xControl, "Configuring for Transmit : [%s], [%u] Hz [%u] bytes\n", toString(theSpreadingFactor), frequency, payloadLength);
    goStandby();
    setRfSwitch(rfSwitchState::tx);
    setRfFrequency(frequency);
    setModulationParameters(theSpreadingFactor);
    setPacketParametersTransmit(static_cast<uint8_t>(payloadLength));
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
    setRfSwitch(rfSwitchState::off);
    constexpr uint8_t nmbrCommandParameters{1};
    uint8_t commandParameters[nmbrCommandParameters]{static_cast<uint8_t>(theSleepMode)};
    executeSetCommand(command::setSleep, commandParameters, nmbrCommandParameters);
}

void sx126x::goStandby(standbyMode theStandbyMode) {
    constexpr uint8_t nmbrCommandParameters{1};
    uint8_t commandParameters[nmbrCommandParameters]{static_cast<uint8_t>(theStandbyMode)};
    executeSetCommand(command::setStandby, commandParameters, nmbrCommandParameters);
}

void sx126x::startTransmit(uint32_t timeOut) {
    constexpr uint8_t nmbrCommandParameters{3};
    uint8_t commandParameters[nmbrCommandParameters];
    commandParameters[0] = static_cast<uint8_t>((timeOut >> 16U) & 0xFF);
    commandParameters[1] = static_cast<uint8_t>((timeOut >> 8U) & 0xFF);
    commandParameters[2] = static_cast<uint8_t>(timeOut & 0xFF);
    executeSetCommand(command::setTx, commandParameters, nmbrCommandParameters);
}

void sx126x::startReceive(uint32_t timeOut) {
    timeOut = 3840;        // TODO : make this a funtion of the DataRate.. currently it is fixed to 60 ms, which should be ok for DR5
    constexpr uint8_t nmbrCommandParameters{3};
    uint8_t commandParameters[nmbrCommandParameters];
    commandParameters[0] = static_cast<uint8_t>((timeOut >> 16U) & 0xFF);
    commandParameters[1] = static_cast<uint8_t>((timeOut >> 8U) & 0xFF);
    commandParameters[2] = static_cast<uint8_t>(timeOut & 0xFF);
    executeSetCommand(command::setRx, commandParameters, nmbrCommandParameters);
}

void sx126x::setRfFrequency(uint32_t frequencyInHz) {
    constexpr uint8_t nmbrCommandParameters{4};
    uint32_t frequencyRegisterValue = calculateFrequencyRegisterValue(frequencyInHz);
    uint8_t commandParameters[nmbrCommandParameters];
    commandParameters[0] = static_cast<uint8_t>((frequencyRegisterValue >> 24U) & 0xFF);        //
    commandParameters[1] = static_cast<uint8_t>((frequencyRegisterValue >> 16U) & 0xFF);        //
    commandParameters[2] = static_cast<uint8_t>((frequencyRegisterValue >> 8U) & 0xFF);         //
    commandParameters[3] = static_cast<uint8_t>(frequencyRegisterValue & 0xFF);                 //
    executeSetCommand(command::setRfFRequency, commandParameters, nmbrCommandParameters);
}

void sx126x::setModulationParameters(spreadingFactor theSpreadingFactor) {
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
    tmpResult = static_cast<uint64_t>(rfFrequency) << 25U;
    tmpResult = tmpResult / TCXOfrequencyInHz;
    return static_cast<uint32_t>(tmpResult);
}

void sx126x::setTxParameters(int8_t transmitPowerdBm) {        // caution - signed int8, negative dBm values are in two's complement
    constexpr uint8_t nmbrCommandParameters{2};
    uint8_t commandParameters[nmbrCommandParameters];

    uint8_t txPower;
    std::memcpy(&txPower, &transmitPowerdBm, 1);

    commandParameters[0] = txPower;
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

    commandParameters[0] = 0x01;
    executeSetCommand(command::setPacketType, commandParameters, 1);

    commandParameters[0] = 0x36;
    commandParameters[1] = 0x41;
    commandParameters[2] = 0x99;
    commandParameters[3] = 0x99;
    executeSetCommand(command::setRfFRequency, commandParameters, 4);

    switch (thePowerVersion) {
        case mcuType::lowPower:        // lowPower Amp : Wio-E5-LE-HF 14 dBm
            commandParameters[0] = 0x04;
            commandParameters[1] = 0x00;
            commandParameters[2] = 0x01;
            commandParameters[3] = 0x01;
            break;

        default:        // highPower Amp : LoRa-E5-HF 14dBm
            commandParameters[0] = 0x02;
            commandParameters[1] = 0x02;
            commandParameters[2] = 0x00;
            commandParameters[3] = 0x01;
            break;
    }
    executeSetCommand(command::setPaConfig, commandParameters, 4);

    commandParameters[0] = 0x0D;        // PA optimal setting and operating modes: 14 dBm HighPower mode
    commandParameters[1] = 0x02;        // RampTime
    executeSetCommand(command::setTxParams, commandParameters, 2);

    commandParameters[0] = 0x34;        // LoRa Syncword MSB
    commandParameters[1] = 0x44;        // LoRa Syncword LSB
    writeRegisters(registerAddress ::LoRaSyncWordMSB, commandParameters, 2);
}

void sx126x::initializeInterface() {
#ifndef generic
    hsubghz.Init.BaudratePrescaler = SUBGHZSPI_BAUDRATEPRESCALER_2;
    HAL_SUBGHZ_Init(&hsubghz);
#endif
}

void sx126x::setRfSwitch(rfSwitchState newState) {
#ifndef generic
    switch (newState) {
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
    HAL_SUBGHZ_WriteBuffer(&hsubghz, 0U, payload, static_cast<uint16_t>(payloadLength));        // copy the raw LoRa message into the transmitBuffer of the SX126
#else
    (void)memcpy(mockSX126xDataBuffer, payload, payloadLength);
#endif
}

void sx126x::readBuffer(uint8_t* payload, uint32_t payloadLength) {
#ifndef generic
    HAL_SUBGHZ_ReadBuffer(&hsubghz, 0U, payload, static_cast<uint16_t>(payloadLength));        // read the raw LoRa message which has been received from the SX126 into the receiveBuffer of the LoRaWAN stack
#else
    (void)memcpy(payload, mockSX126xDataBuffer, payloadLength);
#endif
}

void sx126x::executeSetCommand(command theCommand, uint8_t* commandParameters, uint8_t commandParametersLength) {
#ifndef generic
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, static_cast<SUBGHZ_RadioSetCmd_t>(theCommand), commandParameters, commandParametersLength);
#else
    (void)memcpy(mockSX126xCommandData[static_cast<uint8_t>(theCommand)], commandParameters, commandParametersLength);
#endif
}

void sx126x::executeGetCommand(command theCommand, uint8_t* responseData, uint8_t responseDataLength) {
#ifndef generic
    HAL_SUBGHZ_ExecGetCmd(&hsubghz, static_cast<SUBGHZ_RadioGetCmd_t>(theCommand), responseData, responseDataLength);
#else
    (void)memcpy(responseData, mockSX126xCommandData[static_cast<uint8_t>(theCommand)], responseDataLength);
#endif
}

void sx126x::writeRegisters(registerAddress theRegister, uint8_t* data, uint8_t dataLength) {
#ifndef generic
    HAL_SUBGHZ_WriteRegisters(&hsubghz, static_cast<uint16_t>(theRegister), data, dataLength);
#else
    (void)memcpy(&mockSX126xRegisters[static_cast<uint16_t>(theRegister)], data, dataLength);
#endif
}

void sx126x::readRegisters(registerAddress theRegister, uint8_t* data, uint8_t dataLength) {
#ifndef generic
    HAL_SUBGHZ_ReadRegisters(&hsubghz, static_cast<uint16_t>(theRegister), data, dataLength);
#else
    (void)memcpy(data, &mockSX126xRegisters[static_cast<uint16_t>(theRegister)], dataLength);
#endif
}
