// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

// Notes :
// This driver intends to drive the SX126X chip from Semtech, embedded in the STM32WLE from ST, embedded in the Lora-E5 module from Seeed Studio
// As the (current) scope for my application is only LoRa in the EU868 region, all functions outside this scope are not implemented (FSK, long packets, etc..)
// As the SX126X is embedded inside the STM32WLE5, some of the IOs are 'connected' to registers in the STM32WLE : RESET, BUSY, interrupt lines..
// the RF switch is strictly speaking not part of the SX126X or STM32WLE5, but it is included in the Lora-E5 module, so it is included in this driver

#pragma once
#include <stdint.h>
#include "spreadingfactor.h"

enum class rfSwitchState : uint8_t {
    off,
    tx,
    rx
};

class sx126x {
  public:
    sx126x() = delete;
    static void initialize();
    static void configForTransmit(spreadingFactor theSpreadingFactor, uint32_t frequency, uint8_t *payload, uint32_t payloadLength);
    static void configForReceive(spreadingFactor theSpreadingFactor, uint32_t frequency);

    enum class sleepMode;
    static void goSleep(sleepMode aSleepMode = sleepMode::warmStart);
    enum class standbyMode;
    static void goStandby(standbyMode aStandbyMode = standbyMode::rc);
    static void startTransmit(uint32_t timeOut = 0);
    static void startReceive(uint32_t timeOut);

#ifndef unitTesting

//  private:
#endif
    static constexpr uint32_t rxTxBufferLength{256};
    static constexpr uint32_t TCXOfrequency{32000000U};

    enum class registerAddress : uint16_t {
        LoRaSyncWordMSB          = 0x740,
        LoRaSyncWordLSB          = 0x741,
        TxClampConfig            = 0x8D8,
        OCPConfiguration         = 0x8E7,
        XTAtrim                  = 0x911,
        DIO3outputVoltageControl = 0x920,
        smpsMaximumDrive         = 0x923,
    };

    enum class command {
        // Commands accessing the radio registers and FIFO - SX1261/2 Datasheet Rev 2.1 - Table 11-2
        writeRegister        = 0x0D,
        readRegister         = 0x1D,
        setBufferBaseAddress = 0x8F,
        writeBuffer          = 0x0E,
        readBuffer           = 0x1E,

        // Commands selecting the operating mode of the radio - SX1261/2 Datasheet Rev 2.1 - Table 11-1
        setSleep              = 0x84,
        setStandby            = 0x80,
        setFs                 = 0xC1,
        setTx                 = 0x83,
        setRx                 = 0x82,
        stopTimerOnPreamble   = 0x9F,
        setRxdutycycle        = 0x94,
        setCad                = 0xC5,
        setTxContinuousWave   = 0xD1,
        setTxInfinitePreamble = 0xD2,
        setRegulatorMode      = 0x96,
        calibrate             = 0x89,
        calibrateImage        = 0x98,
        setPaConfig           = 0x95,
        setRxTxFallbackMode   = 0x93,

        // Commands Controlling the RF and Packets Settings - SX1261/2 Datasheet Rev 2.1 - Table 11-4
        setRfFRequency        = 0x86,
        setPacketType         = 0x8A,
        getPacketType         = 0x11,
        setTxParams           = 0x8E,
        setModulationParams   = 0x8B,
        setPacketParameters   = 0x8C,
        setCadParams          = 0x88,
        setLoRaSymbNumTimeout = 0xA0,

        // Commands controlling the radio IRQ and Digital IO (DIO) - SX1261/2 Datasheet Rev 2.1 - Table 11-3
        setDioIrqParams       = 0x08,
        getIrqStatus          = 0x12,
        clearIrqStatus        = 0x02,
        setDIO2AsRfSwitchCtrl = 0x9D,
        setDIO3AsTcxoCtrl     = 0x97,

        // Commands Returning the Radio Status - Errors - Statistics SX1261/2 Datasheet Rev 2.1 - Table 11-5
        getStatus         = 0x0C,
        getRssiInst       = 0x15,
        getRxBufferStatus = 0x13,
        getPacketStatus   = 0x14,
        getDeviceErrors   = 0x17,
        clearDeviceErrors = 0x07,
        getStats          = 0x10,
        resetStats        = 0x00,

    };

    enum class sleepMode {
        coldStart = 0x0 << 2,
        warmStart = 0x1 << 2
    };

    enum class standbyMode {
        rc   = 0x00,
        xosc = 0x01

    };

    enum class rampTime {
        rt10us   = 0x00,
        rt20us   = 0x01,
        rt40us   = 0x02,
        rt80us   = 0x03,
        rt200us  = 0x04,
        rt800us  = 0x05,
        rt1700us = 0x06,
        rt3400us = 0x07
    };

    enum class chipMode {
        standbyRc            = 0x02,
        standbyHseOscillator = 0x03,
        frequencySynthesis   = 0x04,
        receiving            = 0x05,
        transmitting         = 0x06,
    };

    enum class commandStatus {

        dataAvailableForHost    = 0x02,
        commandTimeOut          = 0x03,
        commandProcessingError  = 0x04,
        commandExecutionFailure = 0x05,
        commandTransmitDone     = 0x06,
    };

    static void reset(){};                    // TODO : reset the SX126 from its HW reset pin - in the STMWLE5 this is done from a register in RCC
    static void initializeInterface();        // configures the SPI between the MCU and the SX126x
    static void initializeRadio();            //

    static void setModulationParameters(spreadingFactor theSpreadingFactor);
    static void setRfFrequency(uint32_t frequency);
    static void setPacketParametersReceive();
    static void setPacketParametersTransmit(uint8_t payloadLength);
    static void setTxParameters(int8_t transmitPowerdBm);
    static void setRegulatorMode();
    static void setRfSwitch(rfSwitchState newState);

    static uint32_t calculateFrequencyRegisterValue(uint32_t rfFrequency);

    static void executeGetCommand(command aCommand, uint8_t *responseData, uint8_t responseDataLength);
    static void executeSetCommand(command command, uint8_t *commandData, uint8_t commandDataLength);

    static void readRegisters(registerAddress theRegister, uint8_t *data, uint8_t dataLength);
    static void writeRegisters(registerAddress theRegister, uint8_t *data, uint8_t dataLength);

    static void readBuffer(uint8_t *payload, uint32_t payloadLength);
    static void writeBuffer(uint8_t *payload, uint32_t payloadLength);
};
