#include <lorawan.hpp>
#include <sx126x.hpp>
#include <lptim1.hpp>
#include <logging.hpp>
#include <circularbuffer.hpp>
#include <settingscollection.hpp>
#include <applicationevent.hpp>
#include <maccommand.hpp>
#include <aesblock.hpp>
#include <stm32wle5_aes.hpp>
#include <hexascii.hpp>
#include <swaplittleandbigendian.hpp>
#include <ctime>

#ifndef generic
#include "main.h"
extern RNG_HandleTypeDef hrng;
extern LPTIM_HandleTypeDef hlptim1;
#endif

#pragma region Instantiate and Initialize the Static members

deviceAddress LoRaWAN::DevAddr;
aesKey LoRaWAN::applicationKey;
aesKey LoRaWAN::networkKey;
aesBlock LoRaWAN::K1;
aesBlock LoRaWAN::K2;
frameCount LoRaWAN::uplinkFrameCount(1);
frameCount LoRaWAN::downlinkFrameCount(0);

dataRates LoRaWAN::theDataRates;
uint32_t LoRaWAN::currentDataRateIndex{0};
loRaTxChannelCollection LoRaWAN::txChannels;
uint32_t LoRaWAN::rx1DelayInSeconds{1};
uint32_t LoRaWAN::rx1DataRateOffset{0};
uint32_t LoRaWAN::rx2DataRateIndex{3};        // Maps with setting on TTI Rx2 - SF9
uint32_t LoRaWAN::rx2FrequencyInHz{869525000U};

uint8_t LoRaWAN::rawMessage[b0BlockLength + maxLoRaPayloadLength + 15]{};

uint32_t LoRaWAN::macPayloadLength{0};
uint32_t LoRaWAN::framePortLength{1};
uint32_t LoRaWAN::frameOptionsLength{0};
uint32_t LoRaWAN::frameHeaderLength{0};
uint32_t LoRaWAN::framePayloadLength{0};
uint32_t LoRaWAN::loRaPayloadLength{0};

uint32_t LoRaWAN::framePortOffset{};
uint32_t LoRaWAN::framePayloadOffset{};
uint32_t LoRaWAN::micOffset{};

txRxCycleState LoRaWAN::state{txRxCycleState::idle};

linearBuffer<LoRaWAN::macInOutLength> LoRaWAN::macIn;
linearBuffer<LoRaWAN::macInOutLength> LoRaWAN::macOut;

#pragma endregion
#pragma region Global Objects & Variables

extern circularBuffer<applicationEvent, 16U> applicationEventBuffer;

#pragma endregion
#pragma region 0 : General

void LoRaWAN::initialize() {
    state = txRxCycleState::idle;
    macIn.initialize();
    macOut.initialize();
    clearRawMessage();
    restoreConfig();
    generateKeysK1K2();
    restoreState();
    restoreChannels();
    sx126x::initialize();
}

void LoRaWAN::saveConfig() {
    settingsCollection::save(DevAddr.asUint32, settingsCollection::settingIndex::DevAddr);
    settingsCollection::saveByteArray(applicationKey.asBytes(), settingsCollection::settingIndex::applicationSessionKey);
    settingsCollection::saveByteArray(networkKey.asBytes(), settingsCollection::settingIndex::networkSessionKey);
}

void LoRaWAN::saveState() {
    settingsCollection::save(static_cast<uint8_t>(rx1DelayInSeconds), settingsCollection::settingIndex::rx1Delay);
    settingsCollection::save(uplinkFrameCount.toUint32(), settingsCollection::settingIndex::uplinkFrameCounter);
    settingsCollection::save(downlinkFrameCount.toUint32(), settingsCollection::settingIndex::downlinkFrameCounter);
    settingsCollection::save(static_cast<uint8_t>(currentDataRateIndex), settingsCollection::settingIndex::dataRate);
    settingsCollection::save(static_cast<uint8_t>(rx1DataRateOffset), settingsCollection::settingIndex::rx1DataRateOffset);
    settingsCollection::save(static_cast<uint8_t>(rx2DataRateIndex), settingsCollection::settingIndex::rx2DataRateIndex);
}

void LoRaWAN::saveChannels() {
    uint8_t tmpChannelData[loRaTxChannelCollection::maxNmbrChannels * loRaChannel::sizeInBytes];
    for (uint32_t channelIndex = 0; channelIndex < loRaTxChannelCollection::maxNmbrChannels; channelIndex++) {
        txChannels.channel[channelIndex].toBytes(tmpChannelData + (channelIndex * loRaChannel::sizeInBytes));
    }
    settingsCollection::saveByteArray(tmpChannelData, settingsCollection::settingIndex::txChannels);
    settingsCollection::save(rx2FrequencyInHz, settingsCollection::settingIndex::rxChannel);
}

void LoRaWAN::restoreConfig() {
    DevAddr = settingsCollection::read<uint32_t>(settingsCollection::settingIndex::DevAddr);
    uint8_t tmpKeyArray[aesKey::lengthInBytes];
    settingsCollection::readByteArray(tmpKeyArray, settingsCollection::settingIndex::applicationSessionKey);
    applicationKey.setFromByteArray(tmpKeyArray);
    settingsCollection::readByteArray(tmpKeyArray, settingsCollection::settingIndex::networkSessionKey);
    networkKey.setFromByteArray(tmpKeyArray);
}

void LoRaWAN::restoreState() {
    rx1DelayInSeconds    = settingsCollection::read<uint8_t>(settingsCollection::settingIndex::rx1Delay);
    uplinkFrameCount     = settingsCollection::read<uint32_t>(settingsCollection::settingIndex::uplinkFrameCounter);
    downlinkFrameCount   = settingsCollection::read<uint32_t>(settingsCollection::settingIndex::downlinkFrameCounter);
    currentDataRateIndex = settingsCollection::read<uint8_t>(settingsCollection::settingIndex::dataRate);
    rx1DataRateOffset    = settingsCollection::read<uint8_t>(settingsCollection::settingIndex::rx1DataRateOffset);
    rx2DataRateIndex     = settingsCollection::read<uint8_t>(settingsCollection::settingIndex::rx2DataRateIndex);
}

void LoRaWAN::restoreChannels() {
    uint8_t tmpChannelData[loRaTxChannelCollection::maxNmbrChannels * loRaChannel::sizeInBytes];
    settingsCollection::readByteArray(tmpChannelData, settingsCollection::settingIndex::txChannels);

    for (uint32_t channelIndex = 0; channelIndex < loRaTxChannelCollection::maxNmbrChannels; channelIndex++) {
        txChannels.channel[channelIndex].fromBytes(tmpChannelData + (channelIndex * loRaChannel::sizeInBytes));
    }
    rx2FrequencyInHz = settingsCollection::read<uint32_t>(settingsCollection::settingIndex::rxChannel);
}

#pragma endregion
#pragma region 1 : managing the rawMessage Buffer

void LoRaWAN::clearRawMessage() {
    memset(rawMessage, 0, rawMessageLength);
}

void LoRaWAN::setOffsetsAndLengthsTx(uint32_t theFramePayloadLength, uint32_t theFrameOptionsLength) {
    // This function is used in the uplink direction, where we know the length of the application payload, and we construct a LoRa payload from it
    // NOTE : maximum lengths for payload and frameOptions are not tested here...
    framePayloadLength = theFramePayloadLength;
    frameOptionsLength = theFrameOptionsLength;
    if (framePayloadLength == 0) {
        framePortLength = 0;
    } else {
        framePortLength = 1;
    }
    framePortOffset    = b0BlockLength + macHeaderLength + deviceAddressLength + frameControlLength + frameCountLSHLength + frameOptionsLength;
    framePayloadOffset = b0BlockLength + macHeaderLength + deviceAddressLength + frameControlLength + frameCountLSHLength + frameOptionsLength + framePortLength;
    micOffset          = b0BlockLength + macHeaderLength + deviceAddressLength + frameControlLength + frameCountLSHLength + frameOptionsLength + framePortLength + framePayloadLength;
    loRaPayloadLength  = macHeaderLength + deviceAddressLength + frameControlLength + frameCountLSHLength + frameOptionsLength + framePortLength + theFramePayloadLength + micLength;
    frameHeaderLength  = deviceAddressLength + frameControlLength + frameCountLSHLength + frameOptionsLength;
    macPayloadLength   = frameHeaderLength + framePortLength + framePayloadLength;
}

void LoRaWAN::setOffsetsAndLengthsRx(uint32_t theLoRaPayloadLength) {
    // This function is used in the downlink, where we received a certain LoRa payload, and we decode it to identify all the fields inside it
    loRaPayloadLength  = theLoRaPayloadLength;
    frameOptionsLength = rawMessage[frameControlOffset] & 0x0F;
    frameHeaderLength  = deviceAddressLength + frameControlLength + frameCountLSHLength + frameOptionsLength;
    macPayloadLength   = loRaPayloadLength - (macHeaderLength + micLength);
    if (macPayloadLength > frameHeaderLength) {
        framePortLength    = 1;
        framePayloadLength = macPayloadLength - (frameHeaderLength + framePortLength);
    } else {
        framePortLength    = 0;
        framePayloadLength = 0;
    }
    framePortOffset    = b0BlockLength + macHeaderLength + deviceAddressLength + frameControlLength + frameCountLSHLength + frameOptionsLength;
    framePayloadOffset = b0BlockLength + macHeaderLength + deviceAddressLength + frameControlLength + frameCountLSHLength + frameOptionsLength + framePortLength;
    micOffset          = b0BlockLength + loRaPayloadLength - micLength;
}

void LoRaWAN::insertPayload(const uint8_t data[], const uint32_t length) {
    (void)memcpy(rawMessage + framePayloadOffset, data, length);
}

void LoRaWAN::insertHeaders(const uint8_t theFrameOptions[], const uint32_t theFrameOptionslength, const uint32_t theFramePayloadLength, uint8_t theFramePort) {
    rawMessage[macHeaderOffset]         = macHeader(frameType::unconfirmedDataUp).asUint8();
    rawMessage[deviceAddressOffset]     = DevAddr.asUint8[0];
    rawMessage[deviceAddressOffset + 1] = DevAddr.asUint8[1];
    rawMessage[deviceAddressOffset + 2] = DevAddr.asUint8[2];
    rawMessage[deviceAddressOffset + 3] = DevAddr.asUint8[3];

    if (theFrameOptionslength > 15) {
        logging::snprintf(logging::source::error, "Error : frameOptionsLength > 15\n");
        frameOptionsLength = 15;
    }
    rawMessage[frameControlOffset] = frameOptionsLength & 0x0F;        // FCTRL : TODO : later we will need to add ACK etc..

    if (frameOptionsLength > 0) {
        for (uint32_t index = 0; index < frameOptionsLength; index++) {
            rawMessage[frameOptionsOffset + index] = theFrameOptions[index];
        }
    }

    rawMessage[frameCountOffset]     = uplinkFrameCount[0];
    rawMessage[frameCountOffset + 1] = uplinkFrameCount[1];

    if ((frameOptionsLength > 0) && (framePayloadLength > 0) && theFramePort == 0) {
        logging::snprintf(logging::source::error, "Error : illegal combination of frameOptions and framePort == 0\n");
    }

    if (theFramePayloadLength > 0) {
        rawMessage[framePortOffset] = theFramePort;        //
    }
}

void LoRaWAN::insertBlockB0(linkDirection theDirection, frameCount& aFrameCount) {
    rawMessage[0]  = 0x49;        // see LoRaWAN® L2 1.0.4 Specification - line 808
    rawMessage[1]  = 0;
    rawMessage[2]  = 0;
    rawMessage[3]  = 0;
    rawMessage[4]  = 0;
    rawMessage[5]  = static_cast<uint8_t>(theDirection);
    rawMessage[6]  = DevAddr.asUint8[0];
    rawMessage[7]  = DevAddr.asUint8[1];
    rawMessage[8]  = DevAddr.asUint8[2];
    rawMessage[9]  = DevAddr.asUint8[3];
    rawMessage[10] = aFrameCount[0];
    rawMessage[11] = aFrameCount[1];
    rawMessage[12] = aFrameCount[2];
    rawMessage[13] = aFrameCount[3];
    rawMessage[14] = 0;
    rawMessage[15] = macHeaderLength + frameHeaderLength + framePortLength + framePayloadLength;
}

void LoRaWAN::padForMicCalculation(const uint32_t messageLength) {
    uint32_t nmbrOfBytesToPad = aesBlock::calculateNmbrOfBytesToPad(messageLength);
    if (nmbrOfBytesToPad > 0) {
        rawMessage[micOffset] = 0x80;
        memset(rawMessage + micOffset + 1, 0, nmbrOfBytesToPad - 1);
    }
}

uint16_t LoRaWAN::receivedFramecount() {
    return (static_cast<uint16_t>(rawMessage[frameCountOffset]) + (static_cast<uint16_t>(rawMessage[frameCountOffset + 1]) << 8));
}

uint32_t LoRaWAN::receivedDeviceAddress() {
    return (static_cast<uint32_t>(rawMessage[deviceAddressOffset]) + (static_cast<uint32_t>(rawMessage[deviceAddressOffset + 1]) << 8) + (static_cast<uint32_t>(rawMessage[deviceAddressOffset + 2]) << 16) + (static_cast<uint32_t>(rawMessage[deviceAddressOffset + 3]) << 24));
}

uint32_t LoRaWAN::receivedMic() {
    return (static_cast<uint32_t>(rawMessage[micOffset]) + (static_cast<uint32_t>(rawMessage[micOffset + 1]) << 8) + (static_cast<uint32_t>(rawMessage[micOffset + 2]) << 16) + (static_cast<uint32_t>(rawMessage[micOffset + 3]) << 24));
};

#pragma endregion
#pragma region 2 : Encrypting / Decrypting application payload - Calculating MIC

void LoRaWAN::prepareBlockAi(aesBlock& theBlock, linkDirection theDirection, uint32_t blockIndex) {
    theBlock[0] = 0x01;
    theBlock[1] = 0x00;
    theBlock[2] = 0x00;
    theBlock[3] = 0x00;
    theBlock[4] = 0x00;
    theBlock[5] = static_cast<uint8_t>(theDirection);
    theBlock[6] = DevAddr.asUint8[0];                   // LSByte
    theBlock[7] = DevAddr.asUint8[1];                   //
    theBlock[8] = DevAddr.asUint8[2];                   //
    theBlock[9] = DevAddr.asUint8[3];                   // MSByte
    if (theDirection == linkDirection::uplink) {        //
        theBlock[10] = uplinkFrameCount[0];             // LSByte
        theBlock[11] = uplinkFrameCount[1];             //
        theBlock[12] = uplinkFrameCount[2];             //
        theBlock[13] = uplinkFrameCount[3];             // MSByte
    } else {                                            //
        theBlock[10] = downlinkFrameCount[0];           // LSByte
        theBlock[11] = downlinkFrameCount[1];           //
        theBlock[12] = downlinkFrameCount[2];           //
        theBlock[13] = downlinkFrameCount[3];           // MSByte
    }                                                   //
    theBlock[14] = 0x00;                                //
    theBlock[15] = blockIndex;                          // Blocks Ai are indexed from 1..k, where k is the number of blocks
}

void LoRaWAN::encryptDecryptPayload(aesKey& theKey, linkDirection theLinkDirection) {
    uint32_t nmbrOfBlocks = aesBlock::nmbrOfBlocksFromBytes(framePayloadLength);
#ifdef HARDWARE_AES
    stm32wle5_aes::initialize(aesMode::CTR);
    aesBlock A1;
    prepareBlockAi(A1, theLinkDirection, 1);
    stm32wle5_aes::setKey(theKey);
    stm32wle5_aes::setInitializationVector(A1);
    stm32wle5_aes::enable();
    aesBlock tmpBlock;
    for (uint32_t blockIndex = 0; blockIndex < nmbrOfBlocks; blockIndex++) {
        uint8_t* tmpOffset;
        tmpOffset = rawMessage + (blockIndex * 16) + framePayloadOffset;
        tmpBlock.setFromByteArray(tmpOffset);
        stm32wle5_aes::write(tmpBlock);
        while (!stm32wle5_aes::isComputationComplete()) {
        }
        stm32wle5_aes::read(tmpBlock);
        stm32wle5_aes::clearComputationComplete();

        memcpy(tmpOffset, tmpBlock.asBytes(), aesBlock::lengthInBytes);
    }
    stm32wle5_aes::disable();
#else
    aesBlock theBlock;
    uint32_t incompleteLastBlockSize = aesBlock::incompleteLastBlockSizeFromBytes(framePayloadLength);
    bool hasIncompleteLastBlock      = aesBlock::hasIncompleteLastBlockFromBytes(framePayloadLength);
    for (uint32_t blockIndex = 0x00; blockIndex < nmbrOfBlocks; blockIndex++) {
        prepareBlockAi(theBlock, theLinkDirection, (blockIndex + 1));
        theBlock.encrypt(theKey);

        if (hasIncompleteLastBlock && (blockIndex == (nmbrOfBlocks - 1))) {
            for (uint32_t byteIndex = 0; byteIndex < incompleteLastBlockSize; byteIndex++) {
                rawMessage[(blockIndex * 16) + byteIndex + framePayloadOffset] ^= theBlock[byteIndex];
            }
        } else {
            for (uint32_t byteIndex = 0; byteIndex < 16; byteIndex++) {
                rawMessage[(blockIndex * 16) + byteIndex + framePayloadOffset] ^= theBlock[byteIndex];
            }
        }
    }
#endif
}

void LoRaWAN::generateKeysK1K2() {
    bool msbSet;
    K1.setFromHexString("00000000000000000000000000000000");
    K2.setFromHexString("00000000000000000000000000000000");
    K1.encrypt(networkKey);
    msbSet = ((K1[0] & 0x80) == 0x80);
    K1.shiftLeft();
    if (msbSet) {
        K1[15] = K1[15] ^ 0x87;
    }
    K2     = K1;
    msbSet = ((K2[0] & 0x80) == 0x80);
    K2.shiftLeft();
    if (msbSet) {
        K2[15] = K2[15] ^ 0x87;
    }
}

uint32_t LoRaWAN::calculateMic() {
#ifdef HARDWARE_AES
    // Implementation according to RFC 4493 AES-CMAC

    uint32_t payloadLength = micOffset;
    uint32_t nmbrOfBlocks  = aesBlock::nmbrOfBlocksFromBytes(payloadLength);
    bool hasIncompleteLastBlock;
    if (nmbrOfBlocks == 0) {
        nmbrOfBlocks           = 1;
        hasIncompleteLastBlock = true;
    } else {
        hasIncompleteLastBlock = aesBlock::hasIncompleteLastBlockFromBytes(payloadLength);
    }
    padForMicCalculation(payloadLength);

    aesBlock allZeroes;

    stm32wle5_aes::initialize(aesMode::CBC);
    stm32wle5_aes::setKey(networkKey);
    stm32wle5_aes::setInitializationVector(allZeroes);
    stm32wle5_aes::enable();
    aesBlock tmpBlock;
    for (uint32_t blockIndex = 0; blockIndex < nmbrOfBlocks; blockIndex++) {
        uint8_t* tmpOffset;
        tmpOffset = rawMessage + (blockIndex * 16);
        tmpBlock.setFromByteArray(tmpOffset);
        if (blockIndex == (nmbrOfBlocks - 1)) {
            if (hasIncompleteLastBlock) {
                tmpBlock.XOR(K2.asBytes());
            } else {
                tmpBlock.XOR(K1.asBytes());
            }
        }
        stm32wle5_aes::write(tmpBlock);
        while (!stm32wle5_aes::isComputationComplete()) {
        }
        stm32wle5_aes::read(tmpBlock);
        stm32wle5_aes::clearComputationComplete();
    }
    stm32wle5_aes::disable();
    uint32_t mic = tmpBlock.asWords()[0];

#else
    uint32_t payloadLength      = micOffset;
    uint32_t nmbrOfBlocks       = aesBlock::nmbrOfBlocksFromBytes(payloadLength);
    bool hasIncompleteLastBlock = aesBlock::hasIncompleteLastBlockFromBytes(payloadLength);
    aesBlock allZeroes;

    aesBlock outputBlock;
    uint32_t incompleteLastBlockSize = aesBlock::incompleteLastBlockSizeFromBytes(payloadLength);
    uint8_t outputAsBytes[16];
    unsigned char Old_Data[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    if (nmbrOfBlocks > 0) {
        // Perform full calculating until n-1 message blocks
        for (uint32_t blockIndex = 0x0; blockIndex < (nmbrOfBlocks - 1); blockIndex++) {
            outputBlock.setFromByteArray(rawMessage + (blockIndex * 16));        //  Copy data into block
            outputBlock.XOR(Old_Data);
            outputBlock.encrypt(networkKey);
            (void)memcpy(outputAsBytes, outputBlock.asBytes(), 16);

            for (uint32_t byteIndex = 0; byteIndex < 16; byteIndex++) {
                Old_Data[byteIndex] = outputAsBytes[byteIndex];
            }
        }
        // Perform calculation on last block
        // Check if Datalength is a multiple of 16
        if (incompleteLastBlockSize == 0) {
            outputBlock.setFromByteArray(rawMessage + ((nmbrOfBlocks - 1) * 16));        //  Copy data into block
            outputBlock.XOR(Old_Data);
            outputBlock.XOR(K1.asBytes());
            outputBlock.encrypt(networkKey);

        } else {
            for (uint32_t byteIndex = 0; byteIndex < 16; byteIndex++) {
                if (byteIndex < incompleteLastBlockSize) {
                    outputAsBytes[byteIndex] = rawMessage[((nmbrOfBlocks - 1) * 16) + byteIndex];
                }
                if (byteIndex == incompleteLastBlockSize) {
                    outputAsBytes[byteIndex] = 0x80;
                }
                if (byteIndex > incompleteLastBlockSize) {
                    outputAsBytes[byteIndex] = 0x00;
                }
            }
            outputBlock.setFromByteArray(outputAsBytes);
            outputBlock.XOR(Old_Data);
            outputBlock.XOR(K2.asBytes());
            outputBlock.encrypt(networkKey);
        }

    } else {
        outputAsBytes[0] = 0x80;
        for (uint32_t byteIndex = 1; byteIndex < 16; byteIndex++) {
            outputAsBytes[byteIndex] = 0x00;
        }

        outputBlock.setFromByteArray(outputAsBytes);
        outputBlock.XOR(K2.asBytes());
        outputBlock.XOR(Old_Data);        // I think this step is useless, as Old_Data is all zeroes
        outputBlock.encrypt(networkKey);
    }
    uint32_t mic = ((outputBlock.asBytes()[3] << 24) + (outputBlock.asBytes()[2] << 16) + (outputBlock.asBytes()[1] << 8) + (outputBlock.asBytes()[0]));
#endif
    return mic;
}

void LoRaWAN::insertMic() {
    uint32_t mic = calculateMic();
    insertMic(mic);
}

void LoRaWAN::insertMic(uint32_t aMic) {
    rawMessage[micOffset]     = aMic & 0x000000FF;                // LSByte
    rawMessage[micOffset + 1] = (aMic & 0x0000FF00) >> 8;         //
    rawMessage[micOffset + 2] = (aMic & 0x00FF0000) >> 16;        //
    rawMessage[micOffset + 3] = (aMic & 0xFF000000) >> 24;        // MSByte
}

// void LoRaWAN::padForEncryptDecrypt() {
//     nmbrOfBytesToPad = aesBlock::calculateNmbrOfBytesToPad(framePayloadLength);
//     for (uint32_t index = framePayloadOffset; index < (framePayloadOffset + nmbrOfBytesToPad); index++) {
//         rawMessage[index] = 0x00;
//     }
// }

#pragma endregion
#pragma region 3 : TxRxCycle State Machine

// void LoRaWAN::run() {
//     if ((macOut.getLevel() > 15) && isIdle()) {        // if we have more than 15 bytes of MAC stuff, we need a separate uplink message (cannot piggyback on a data message), so we send the msg now
//         sendUplink();                                  // start an uplink cycle with the MAC stuff on port 0
//         removeNonStickyMacStuff();                     // after all MAC stuff was sent, remove it from the macOut buffer, except for the sticky MAC stuff, which is only removed after receiving a donwlink
//     }
// }

txRxCycleState LoRaWAN::getState() {
    return state;
}

bool LoRaWAN::isIdle() {
    return (state == txRxCycleState::idle);
}

void LoRaWAN::handleEvents(applicationEvent theEvent) {
    logging::snprintf(logging::source::lorawanEvents, "LoRaWAN event %s [%u] in state %s [%u] \n", toString(theEvent), static_cast<uint8_t>(theEvent), toString(state), static_cast<uint8_t>(state));

    switch (state) {
        default:
        case txRxCycleState::idle:
            break;

        case txRxCycleState::waitForCadEnd:
            switch (theEvent) {
                case applicationEvent::sx126xCadEnd:
                    return;
                    break;
                default:
                    break;
            }
            break;

        case txRxCycleState::waitForRandomTimeBeforeTransmit:
            switch (theEvent) {
                case applicationEvent::lowPowerTimerExpired:
                    sx126x::startTransmit();
                    goTo(txRxCycleState::waitForTxComplete);
                    return;
                    break;
                default:
                    break;
            }
            break;

        case txRxCycleState::waitForTxComplete:
            switch (theEvent) {
                case applicationEvent::sx126xTxComplete:
                    startTimer(ticksFromSeconds(rx1DelayInSeconds));
                    goTo(txRxCycleState::waitForRx1Start);
                    return;
                    break;
                default:
                    break;
            }
            break;

        case txRxCycleState::waitForRx1Start:
            switch (theEvent) {
                case applicationEvent::lowPowerTimerExpired: {
                    stopTimer();
                    startTimer(ticksFromSeconds(1U));
                    uint32_t rxFrequency = txChannels.channel[txChannels.getCurrentChannelIndex()].frequencyInHz;
                    uint32_t rxTimeout   = getReceiveTimeout(theDataRates.theDataRates[currentDataRateIndex].theSpreadingFactor);
                    sx126x::configForReceive(theDataRates.theDataRates[currentDataRateIndex].theSpreadingFactor, rxFrequency);
                    sx126x::startReceive(rxTimeout);
                    goTo(txRxCycleState::waitForRx1CompleteOrTimeout);
                    return;
                } break;
                default:
                    break;
            }
            break;

        case txRxCycleState::waitForRx1CompleteOrTimeout:
            switch (theEvent) {
                case applicationEvent::sx126xRxComplete: {
                    logging::snprintf(logging::source::applicationEvents, "Received LoRaWAN  msg in Rx1\n");
                    messageType receivedMessageType = decodeMessage();
                    switch (receivedMessageType) {
                        case messageType::application:
                            stopTimer();
                            processMacContents();
                            applicationEventBuffer.push(applicationEvent::downlinkApplicationPayloadReceived);
                            goTo(txRxCycleState::waitForRxMessageReadout);
                            return;
                            break;
                        case messageType::lorawanMac:
                            stopTimer();
                            processMacContents();
                            goTo(txRxCycleState::idle);
                            return;
                            break;
                        default:
                        case messageType::invalid:
                            goTo(txRxCycleState::waitForRx2Start);
                            return;
                            break;
                    }
                } break;
                case applicationEvent::sx126xTimeout:
                    goTo(txRxCycleState::waitForRx2Start);
                    return;
                    break;
                default:
                    break;
            }
            break;

        case txRxCycleState::waitForRx2Start:
            switch (theEvent) {
                case applicationEvent::lowPowerTimerExpired: {
                    stopTimer();
                    sx126x::configForReceive(theDataRates.theDataRates[rx2DataRateIndex].theSpreadingFactor, rx2FrequencyInHz);
                    uint32_t rxTimeout = getReceiveTimeout(theDataRates.theDataRates[rx2DataRateIndex].theSpreadingFactor);
                    sx126x::startReceive(rxTimeout);
                    goTo(txRxCycleState::waitForRx2CompleteOrTimeout);
                    return;
                } break;
                default:
                    break;
            }
            break;

        case txRxCycleState::waitForRx2CompleteOrTimeout:
            switch (theEvent) {
                case applicationEvent::sx126xRxComplete: {
                    logging::snprintf(logging::source::applicationEvents, "Received LoRaWAN  msg in Rx2\n");
                    messageType receivedMessageType = decodeMessage();
                    switch (receivedMessageType) {
                        case messageType::application:
                            processMacContents();
                            applicationEventBuffer.push(applicationEvent::downlinkApplicationPayloadReceived);
                            goTo(txRxCycleState::waitForRxMessageReadout);
                            return;
                            break;
                        case messageType::lorawanMac:
                            processMacContents();
                            goTo(txRxCycleState::idle);
                            return;
                            break;
                        default:
                        case messageType::invalid:
                            goTo(txRxCycleState::idle);
                            return;
                            break;
                    }
                } break;
                case applicationEvent::sx126xTimeout:
                    goTo(txRxCycleState::idle);
                    return;
                    break;
                default:
                    break;
            }
            break;
    }
    logging::snprintf(logging::source::error, "Error : received event %s [%u] in state %s [%u] \n", toString(theEvent), static_cast<uint8_t>(theEvent), toString(state), static_cast<uint8_t>(state));
    goTo(txRxCycleState::idle);
}

void LoRaWAN::goTo(txRxCycleState newState) {
    logging::snprintf(logging::source::lorawanEvents, "LoRaWAN stateChange from %s [%u] to %s [%u]\n", toString(state), state, toString(newState), newState);
    // Tasks to do when leaving the current state
    switch (state) {
        case txRxCycleState::idle:
            break;

        case txRxCycleState::waitForRandomTimeBeforeTransmit:
            stopTimer();
            break;

        case txRxCycleState::waitForTxComplete:
            break;

        case txRxCycleState::waitForRx1Start:
            break;

        case txRxCycleState::waitForRx1CompleteOrTimeout:
            break;

        case txRxCycleState::waitForRx2Start:
            break;

        case txRxCycleState::waitForRx2CompleteOrTimeout:
            break;

        default:
            break;
    }
    state = newState;
    // Tasks to do when entering the new state
    switch (newState) {
        case txRxCycleState::idle:
            sx126x::goSleep(sx126x::sleepMode::warmStart);
            break;

        case txRxCycleState::waitForRandomTimeBeforeTransmit: {
            uint32_t randomDelayAsTicks = randomNumber() % maxRandomDelayBeforeTx;
            startTimer(randomDelayAsTicks);
        } break;

        case txRxCycleState::waitForTxComplete:
            break;

        case txRxCycleState::waitForRx1Start:
            break;

        case txRxCycleState::waitForRx1CompleteOrTimeout:
            break;

        case txRxCycleState::waitForRx2Start:
            break;

        case txRxCycleState::waitForRx2CompleteOrTimeout:
            break;

        case txRxCycleState::waitForRxMessageReadout:
            break;

        default:
            break;
    }
}

void LoRaWAN::startTimer(uint32_t timeOutIn4096HzTicks) {
#ifndef generic
    HAL_LPTIM_SetOnce_Start_IT(&hlptim1, 0xFFFF, timeOutIn4096HzTicks);
#endif
}

void LoRaWAN::stopTimer() {
#ifndef generic
    HAL_LPTIM_SetOnce_Stop_IT(&hlptim1);
#endif
}

#pragma endregion
#pragma region 4 : MAC layer functions

void LoRaWAN::processMacContents() {
    dumpMacIn();
    while (macIn.getLevel() > 0) {
        macCommand theMacCommand = static_cast<macCommand>(macIn[0]);

        switch (theMacCommand) {
            case macCommand::linkCheckAnswer:
                processLinkCheckAnswer();
                break;

            case macCommand::linkAdaptiveDataRateRequest:
                processLinkAdaptiveDataRateRequest();
                break;

            case macCommand::dutyCycleRequest:
                processDutyCycleRequest();
                break;

            case macCommand::deviceStatusRequest:
                processDeviceStatusRequest();
                break;

            case macCommand::newChannelRequest:
                processNewChannelRequest();
                break;

            case macCommand::receiveParameterSetupRequest:
                processReceiveParameterSetupRequest();
                break;

            case macCommand::receiveTimingSetupRequest:
                processReceiveTimingSetupRequest();
                break;

            case macCommand::transmitParameterSetupRequest:
                processTransmitParameterSetupRequest();
                break;

            case macCommand::downlinkChannelRequest:
                processDownlinkChannelRequest();
                break;

            case macCommand::deviceTimeAnswer:
                processDeviceTimeAnswer();
                break;

            default:
                // unknown command.. abort further processing as each command has a different length and so we can not skip this unknown command
                macIn.initialize();
                logging::snprintf(logging::source::lorawanMac, "Unknown MAC command \n");
                break;
        }
    }
    dumpMacOut();
    saveState();
    saveChannels();
}

void LoRaWAN::removeNonStickyMacStuff() {
    linearBuffer<64> tmpMacOut;

    while (macOut.getLevel() > 0) {
        macCommand theMacCommand = static_cast<macCommand>(macOut[0]);
        switch (theMacCommand) {
            case macCommand::linkCheckRequest:
                macOut.consume(1);
                break;

            case macCommand::linkAdaptiveDataRateAnswer:
                macOut.consume(2);
                break;

            case macCommand::dutyCycleAnswer:
                macOut.consume(1);
                break;

            case macCommand::receiveParameterSetupAnswer:
                tmpMacOut.append(macOut[0]);
                tmpMacOut.append(macOut[1]);
                macOut.consume(2);
                break;

            case macCommand::deviceStatusAnswer:
                macOut.consume(3);
                break;

            case macCommand::newChannelAnswer:
                macOut.consume(2);
                break;

            case macCommand::receiveTimingSetupAnswer:
                tmpMacOut.append(macOut[0]);
                macOut.consume(1);
                break;

            case macCommand::transmitParameterSetupAnswer:
                tmpMacOut.append(macOut[0]);
                macOut.consume(1);
                break;

            case macCommand::downlinkChannelAnswer:
                tmpMacOut.append(macOut[0]);
                tmpMacOut.append(macOut[1]);
                macOut.consume(2);
                break;

            case macCommand::deviceTimeRequest:
                macOut.consume(1);
                break;

            default:
                break;
        }
    }

    macOut.initialize();
    macOut.append(tmpMacOut.asUint8Ptr(), tmpMacOut.getLevel());
    // TODO : if we have sticky MACstuff, we could append a linkcheckrequest and thus force the LNS to send us a downlink, which confirms the sticky MAC stuff...
}

void LoRaWAN::processLinkCheckAnswer() {
    constexpr uint32_t linkCheckAnswerLength{3};                                                                                        // commandId, margin, GwCnt
    uint8_t margin       = macIn[1];                                                                                                    // margin : [0..254] = value in dB above the demodulation floor -> the higher the better,  margin [255] = reserved
    uint8_t gatewayCount = macIn[2];                                                                                                    // GwCnt : number of gateways that successfully received the last uplink
    macIn.consume(linkCheckAnswerLength);                                                                                               // consume all bytes
    logging::snprintf(logging::source::lorawanMac, "LinkCheckAnswer : margin = %d, gatewayCount = %d \n", margin, gatewayCount);        // TODO : For the time being, we just show this info in the logging. Later we could use it to show the quality of the link on the display
}

void LoRaWAN::processLinkAdaptiveDataRateRequest() {
    constexpr uint32_t linkAdaptiveDataRateRequestLength{5};                                                   //  commandId, dataRate_txPower[1], chMask[2], redundancy[1]
    uint8_t dataRateTxPower = macIn[1];                                                                        // dataRate : [0..15] = the data rate used for the last transmission, txPower : [0..15] = the TX power used for the last transmission
    uint16_t chMask         = (static_cast<uint16_t>(macIn[2]) | static_cast<uint16_t>(macIn[3] << 8));        // ChMask : [0..15] = the channel mask used for the last transmission
    uint8_t redundancy      = macIn[4];                                                                        // Redundancy : [0..15] = the redundancy used for the last transmission
    macIn.consume(linkAdaptiveDataRateRequestLength);
    logging::snprintf(logging::source::lorawanMac, "LinkAdaptiveDataRateRequest : 0x%02X, 0x%04X, 0x%02X \n", dataRateTxPower, chMask, redundancy);

    // TODO : currently I don't understand the purpose of this mac command, so I just ignore it. Maybe it's more useful in US-915 than EU-868

    constexpr uint32_t linkAdaptiveDataRateAnswerLength{2};        // commandId, status
    uint8_t answer[linkAdaptiveDataRateAnswerLength];
    answer[0] = static_cast<uint8_t>(macCommand::linkAdaptiveDataRateAnswer);
    answer[1] = static_cast<uint8_t>(0);        // TODO : For the time being we just reject this stupid mac command
    macOut.append(answer, linkAdaptiveDataRateAnswerLength);
    logging::snprintf(logging::source::lorawanMac, "linkAdaptiveDataRateAnswer : 0x%02X \n", answer[1]);
}

void LoRaWAN::processDutyCycleRequest() {
    constexpr uint32_t dutyCycleRequestLength{2};        // commandId, dutyCycle
    uint8_t dutyCycle = macIn[1];
    macIn.consume(dutyCycleRequestLength);
    logging::snprintf(logging::source::lorawanMac, "DutyCycleRequest : 0x%02X \n", dutyCycle);

    // TODO : until we implement dutyCycle management, we ignore this command

    constexpr uint32_t dutyCycleAnswerLength{1};
    uint8_t answer[dutyCycleAnswerLength];
    answer[0] = static_cast<uint8_t>(macCommand::dutyCycleAnswer);
    macOut.append(answer, dutyCycleAnswerLength);
    logging::snprintf(logging::source::lorawanMac, "DutyCycleAnswer \n");
}

void LoRaWAN::processReceiveParameterSetupRequest() {
    constexpr uint32_t receiveParameterSetupRequestLength{5};        // commandId, downLinkSettings, frequency[3]
    rx1DataRateOffset = (macIn[1] & 0x70) >> 4;
    rx2DataRateIndex  = macIn[1] & 0x0F;
    rx2FrequencyInHz  = (static_cast<uint32_t>(macIn[2]) + (static_cast<uint32_t>(macIn[3]) << 8) + (static_cast<uint32_t>(macIn[4]) << 16)) * 100;
    macIn.consume(receiveParameterSetupRequestLength);
    logging::snprintf(logging::source::lorawanMac, "ReceiveParameterSetupRequest : rx1DataRateOffset = %u, rx2DataRateIndex = %u, rx2Frequency = %u Hz \n", rx1DataRateOffset, rx2DataRateIndex, rx2FrequencyInHz);

    constexpr uint32_t receiveParameterSetupAnswerLength{2};        // commandId, status
    uint8_t answer[receiveParameterSetupAnswerLength];
    answer[0] = static_cast<uint8_t>(macCommand::receiveParameterSetupAnswer);
    answer[1] = static_cast<uint8_t>(0b00000111);        // The end-device accepts the new settings
    macOut.append(answer, receiveParameterSetupAnswerLength);
    logging::snprintf(logging::source::lorawanMac, "ReceiveParameterSetupAnswer : 0x%02X \n", answer[1]);
}

void LoRaWAN::processDeviceStatusRequest() {
    constexpr uint32_t deviceStatusRequestLength{1};
    macIn.consume(deviceStatusRequestLength);
    logging::snprintf(logging::source::lorawanMac, "DeviceStatusRequest \n");

    constexpr uint32_t deviceStatusAnswerLength{3};        // commandId, batteryLevel, margin
    uint8_t answer[deviceStatusAnswerLength];
    answer[0] = static_cast<uint8_t>(macCommand::deviceStatusAnswer);
    answer[1] = static_cast<uint8_t>(255);        // The end-device was not able to measure the battery level
    answer[2] = static_cast<uint8_t>(10);         // TODO : implement real value here
    macOut.append(answer, deviceStatusAnswerLength);
    logging::snprintf(logging::source::lorawanMac, "DeviceStatusAnswer : 0x%02X, 0x%02X \n", answer[1], answer[2]);
}

void LoRaWAN::processNewChannelRequest() {
    constexpr uint32_t newChannelRequestLength{6};        // 6 bytes : commandId, channelIndex, frequency[3], DRrange
    uint32_t channelIndex = macIn[1];                     //
    if ((channelIndex >= 3) && (channelIndex <= 15)) {
        txChannels.channel[channelIndex].frequencyInHz        = (static_cast<uint32_t>(macIn[2]) + (static_cast<uint32_t>(macIn[3]) << 8) + (static_cast<uint32_t>(macIn[4]) << 16)) * 100;
        txChannels.channel[channelIndex].minimumDataRateIndex = macIn[5] & 0x0F;
        txChannels.channel[channelIndex].maximumDataRateIndex = (macIn[5] & 0xF0) >> 4;
        logging::snprintf(logging::source::lorawanMac, "NewChannelRequest for channel %u, frequency = %u, minimumDataRate = %u, maximumDataRate = %u \n", channelIndex, txChannels.channel[channelIndex].frequencyInHz, txChannels.channel[channelIndex].minimumDataRateIndex, txChannels.channel[channelIndex].maximumDataRateIndex);
    } else {
        logging::snprintf(logging::source::error, "NewChannelRequest for channel %u \n", channelIndex);
        // channels 0..2 are always active and newChannelRequest should be ignored on them... L2 Spec 1.0.4 line 1135
        // EU-868 only has 16 channels, 0..15
    }

    macIn.consume(newChannelRequestLength);

    constexpr uint32_t newChannelAnswerLength{2};                                                     // 2 bytes : commandId, status
    uint8_t answer[newChannelAnswerLength];                                                           //
    answer[0] = static_cast<uint8_t>(macCommand::newChannelAnswer);                                   //
    answer[1] = static_cast<uint8_t>(0b00000011);                                                     // The end-device accepts datarange and frequency
    macOut.append(answer, newChannelAnswerLength);                                                    //
    logging::snprintf(logging::source::lorawanMac, "NewChannelAnswer : 0x%02X \n", answer[1]);        //
}

void LoRaWAN::processReceiveTimingSetupRequest() {
    constexpr uint32_t receiveTimingSetupRequestLength{2};
    uint32_t receivedRx1Delay = macIn[1] & 0x0F;
    if (receivedRx1Delay == 0) {
        receivedRx1Delay = 1;        // a value of 0, also results in a delay of 1 second. Link Layer Spec V1.0.4, line 1237
    }
    rx1DelayInSeconds = receivedRx1Delay;
    logging::snprintf(logging::source::lorawanMac, "ReceiveTimingSetupRequest : rx1Delay set to %u\n", rx1DelayInSeconds);        //
    settingsCollection::save(rx1DelayInSeconds, settingsCollection::settingIndex::rx1Delay);
    macIn.consume(receiveTimingSetupRequestLength);

    constexpr uint32_t receiveTimingSetupAnswerLength{1};
    uint8_t answer[receiveTimingSetupAnswerLength];
    answer[0] = static_cast<uint8_t>(macCommand::receiveTimingSetupAnswer);
    macOut.append(answer, receiveTimingSetupAnswerLength);
    logging::snprintf(logging::source::lorawanMac, "ReceiveTimingSetupAnswer \n");
}

void LoRaWAN::processTransmitParameterSetupRequest() {
    // Not implemented in EU-868 Region : see Regional Parameters 1.0.3, line 334
    macIn.consume(2);
    logging::snprintf(logging::source::lorawanMac, "TransmitParameterSetupRequest \n");
}

void LoRaWAN::processDownlinkChannelRequest() {
    uint8_t channelIndex = macIn[1];
    uint32_t frequency   = (static_cast<uint8_t>(macIn[2]) + (static_cast<uint8_t>(macIn[3]) << 8) + (static_cast<uint8_t>(macIn[4]) << 16)) * 100;
    macIn.consume(5);
    logging::snprintf(logging::source::lorawanMac, "DownlinkChannelRequest : 0x%02X, %u \n", channelIndex, frequency);
    // TODO : update channel info

    uint8_t answer[2];
    answer[0] = static_cast<uint8_t>(macCommand::downlinkChannelAnswer);
    answer[1] = static_cast<uint8_t>(0b00000011);        // The end-device accepts the frequency
    macOut.append(answer, 2);
    logging::snprintf(logging::source::lorawanMac, "DownlinkChannelAnswer : 0x%02X \n", answer[1]);
}

void LoRaWAN::processDeviceTimeAnswer() {
    uint32_t gpsTime = static_cast<uint32_t>(macIn[1]) + (static_cast<uint32_t>(macIn[2]) << 8U) + (static_cast<uint32_t>(macIn[3]) << 16U) + (static_cast<uint32_t>(macIn[4]) << 24U);        // macIn[5] contains subsecond time, but we don't use it for the time being
    static constexpr uint32_t unixToGpsOffset{315964800};
    static constexpr uint32_t leapSecondsOffset{18};        // TODO : get this from nvs setting, so we can update it when needed
    // NOTE : this time is the time at the end of the uplink -> we need to add the rx1Delay and optionally the rx2Delay  + if possible the duration of the RxMessage to get accurate time..
    // I need a mock for the RTC, so I can test this code
    time_t unixTime = gpsTime + unixToGpsOffset - leapSecondsOffset;
    struct tm* timeInfo;
    timeInfo = gmtime(&unixTime);
    logging::snprintf(logging::source::lorawanMac, "DeviceTimeAnswer : %s", asctime(timeInfo));

    macIn.consume(6);
}

#pragma endregion

uint32_t LoRaWAN::getMaxApplicationPayloadLength() {
    return (theDataRates.theDataRates[currentDataRateIndex].maximumPayloadLength - macOut.getLevel());
}

void LoRaWAN::sendUplink(uint8_t theFramePort, const uint8_t applicationData[], uint32_t applicationDataLength) {
    if ((theFramePort == 0) && (applicationDataLength > 0)) {
        logging::snprintf(logging::source::error, "Error : cannot send application payload on framePort 0\n");
        return;
    }

    sx126x::initializeInterface();
    sx126x::initializeRadio();


    if (theFramePort == 0) {
        setOffsetsAndLengthsTx(macOut.getLevel());
        insertHeaders(nullptr, 0, macOut.getLevel(), 0);
        insertPayload(macOut.asUint8Ptr(), macOut.getLevel());        // TODO : test the length of the MAC stuff we are going to send, so it does not exceed the maximum allowed length
        encryptDecryptPayload(networkKey, linkDirection::uplink);
    } else {
        setOffsetsAndLengthsTx(applicationDataLength, macOut.getLevel());
        insertHeaders(macOut.asUint8Ptr(), macOut.getLevel(), applicationDataLength, theFramePort);
        if (applicationDataLength > 0) {
            insertPayload(applicationData, applicationDataLength);
            encryptDecryptPayload(applicationKey, linkDirection::uplink);
        }
    }
    insertBlockB0(linkDirection::uplink, uplinkFrameCount);
    insertMic();

    // 2. Configure the radio, and start stateMachine for transmitting the payload
    txChannels.selectRandomChannelIndex();        // randomize the channel index
    uint32_t txFrequency = txChannels.channel[txChannels.getCurrentChannelIndex()].frequencyInHz;
    spreadingFactor csf  = theDataRates.theDataRates[currentDataRateIndex].theSpreadingFactor;
    sx126x::configForTransmit(csf, txFrequency, rawMessage + macHeaderOffset, loRaPayloadLength);

    if (logging::isActive(logging::source::lorawanMac)) {
        dumpTransmitSettings();
        dumpRawMessage();
    }

    goTo(txRxCycleState::waitForRandomTimeBeforeTransmit);

    // 3. txRxCycle is started..
    uplinkFrameCount++;
    settingsCollection::save(uplinkFrameCount.toUint32(), settingsCollection::settingIndex::uplinkFrameCounter);
    settingsCollection::save(downlinkFrameCount.toUint32(), settingsCollection::settingIndex::downlinkFrameCounter);
    removeNonStickyMacStuff();
}

void LoRaWAN::getReceivedDownlinkMessage() {
    // sx126x::getReceivedMessage();
    //  downlinkMessage.processDownlinkMessage(applicationPayloadReceived);
}

messageType LoRaWAN::decodeMessage() {
    // 1. read the raw message from the radio receiveBuffer
    uint8_t response[2];
    sx126x::executeGetCommand(sx126x::command::getRxBufferStatus, response, 2);
    loRaPayloadLength = response[0];
    sx126x::readBuffer(rawMessage + b0BlockLength, loRaPayloadLength);
    setOffsetsAndLengthsRx(loRaPayloadLength);
    dumpRawMessage();

    // 2. Extract & guess the downLinkFrameCount, as we need this to check the MIC
    uint16_t receivedDownlinkFramecount = receivedFramecount();
    frameCount guessedDownlinkFramecount;
    guessedDownlinkFramecount = downlinkFrameCount;
    guessedDownlinkFramecount.guessFromUint16(receivedDownlinkFramecount);
    logging::snprintf(logging::source::lorawanMac, "receivedFramecount = %u, lastFramecount = %u, guessedFramecount = %u\n", receivedDownlinkFramecount, downlinkFrameCount.toUint32(), guessedDownlinkFramecount.toUint32());

    // 3. Check the MIC
    uint32_t _receivedMic = receivedMic();
    insertBlockB0(linkDirection::downlink, guessedDownlinkFramecount);
    uint32_t calculatedMic = calculateMic();
    if (_receivedMic != calculatedMic) {
        if (logging::isActive(logging::source::error)) {
            logging::snprintf("Error : invalid MIC, loraPayload = ");
            for (uint8_t i = 0; i < loRaPayloadLength; i++) {
                logging::snprintf("%02X ", rawMessage[i + b0BlockLength]);
            }
            logging::snprintf("\n");
        }
        return messageType::invalid;
    }

    // 4. Extract the deviceAddress, to check if packet is addressed to this node
    deviceAddress _receivedDeviceAddress(receivedDeviceAddress());
    if (_receivedDeviceAddress != DevAddr) {
        logging::snprintf(logging::source::lorawanMac, "Msg for other device : %08X\n", _receivedDeviceAddress.asUint32);        // TODO : also log received deviceAddress
        return messageType::invalid;
    }

    // 5. check if the frameCount is valid
    if (!isValidDownlinkFrameCount(guessedDownlinkFramecount)) {
        logging::snprintf(logging::source::error, "Error : invalid downlinkFrameCount : received %u, current %u\n", guessedDownlinkFramecount.toUint32(), downlinkFrameCount.toUint32());
        return messageType::invalid;
    }

    // 6. Seems a valid message, so update the downlinkFrameCount to what we've received (not just incrementing it, as there could be gaps in the sequence due to lost packets)
    downlinkFrameCount = guessedDownlinkFramecount;
    settingsCollection::save(downlinkFrameCount.toUint32(), settingsCollection::settingIndex::downlinkFrameCounter);

    // 6.5 If we had sticky macOut stuff, we can clear that now, as we did receive a valid downlink
    macOut.initialize();

    // 7. If we have MAC stuff in the frameOptions, extract them
    if (frameOptionsLength > 0) {
        macIn.append(rawMessage + frameOptionsOffset, frameOptionsLength);
    }

    // 8. If there is any framePayload, decrypt if
    if (framePayloadLength > 0) {
        if (rawMessage[framePortOffset] == 0) {
            encryptDecryptPayload(networkKey, linkDirection::downlink);
            macIn.append(rawMessage + framePayloadOffset, framePayloadLength);
            dumpRawMessagePayload();
            return messageType::lorawanMac;
        } else {
            encryptDecryptPayload(applicationKey, linkDirection::downlink);
            dumpRawMessagePayload();
            return messageType::application;
        }
    } else {
        return messageType::lorawanMac;
    }
}

uint32_t LoRaWAN::getReceiveTimeout(spreadingFactor aSpreadingFactor) {
    static constexpr uint32_t baseTimeout{640};
    switch (aSpreadingFactor) {
        case spreadingFactor::SF7:
            return 4 * baseTimeout;
        case spreadingFactor::SF8:
            return 8 * baseTimeout;
        case spreadingFactor::SF9:
            return 16 * baseTimeout;
        case spreadingFactor::SF10:
            return 32 * baseTimeout;
        case spreadingFactor::SF11:
            return 64 * baseTimeout;
        case spreadingFactor::SF12:
        default:
            return 128 * baseTimeout;
    }
}

bool LoRaWAN::isValidDownlinkFrameCount(frameCount testFrameCount) {
    if (downlinkFrameCount.toUint32() == 0) {
        return true;        // no downlink received yet, so any frameCount is valid
    } else {
        return (testFrameCount > downlinkFrameCount);
    }
}

void LoRaWAN::checkNetwork() {
    macOut.append(static_cast<uint8_t>(macCommand::linkCheckRequest));
}

uint32_t LoRaWAN::randomNumber() {
#ifndef generic
    uint32_t result{0};
    HAL_RNG_GenerateRandomNumber(&hrng, &result);
    return result;
#else
    // result = rand();
    return 0;
#endif
}

void LoRaWAN::dumpMacOut() {
    if (!logging::isActive() || !logging::isActive(logging::source::lorawanMac)) {
        return;
    }
    uint32_t tmpMacOutLevel = macOut.getLevel();
    logging::snprintf("MAC output [%d] = ", tmpMacOutLevel);
    for (uint32_t i = 0; i < tmpMacOutLevel; i++) {
        logging::snprintf("%02X ", macOut[i]);
    }
    logging::snprintf("\n");
}

void LoRaWAN::dumpMacIn() {
    if (!logging::isActive() || !logging::isActive(logging::source::lorawanMac)) {
        return;
    }
    uint32_t tmpMacInLevel = macIn.getLevel();
    logging::snprintf("MAC input [%d] = ", tmpMacInLevel);
    for (uint32_t i = 0; i < tmpMacInLevel; i++) {
        logging::snprintf("%02X ", macIn[i]);
    }
    logging::snprintf("\n");
}

void LoRaWAN::dumpConfig() {
    if (!logging::isActive() || !logging::isActive(logging::source::lorawanMac)) {
        return;
    }
    logging::snprintf("LoRaWAN Config :\n");
    logging::snprintf("  devAddr        = 0x%04X\n", DevAddr.asUint32);
    char tmpKeyAsHexAscii[33];
    hexAscii::byteArrayToHexString(tmpKeyAsHexAscii, applicationKey.asBytes(), 16);
    logging::snprintf("  applicationKey = %s\n", tmpKeyAsHexAscii);
    hexAscii::byteArrayToHexString(tmpKeyAsHexAscii, networkKey.asBytes(), 16);
    logging::snprintf("  networkKey     = %s\n", tmpKeyAsHexAscii);
}

void LoRaWAN::dumpState() {
    if (!logging::isActive() || !logging::isActive(logging::source::lorawanMac)) {
        return;
    }
    logging::snprintf("LoRaWAN State :\n");
    logging::snprintf("  uplinkFrameCount   = %u\n", uplinkFrameCount.toUint32());
    logging::snprintf("  downlinkFrameCount = %u\n", downlinkFrameCount.toUint32());
    logging::snprintf("  dataRateIndex      = %u\n", currentDataRateIndex);
    logging::snprintf("  rx1Delay           = %u [s]\n", rx1DelayInSeconds);
}

void LoRaWAN::dumpChannels() {
    if (!logging::isActive() || !logging::isActive(logging::source::lorawanMac)) {
        return;
    }
    logging::snprintf("LoRaWAN Channels :\n");
    for (uint32_t channelIndex = 0; channelIndex < loRaTxChannelCollection::maxNmbrChannels; channelIndex++) {
        logging::snprintf("  Tx channel[%02u] : frequency = %09u [Hz], minDR = %u, maxDR = %u\n", channelIndex, txChannels.channel[channelIndex].frequencyInHz, txChannels.channel[channelIndex].minimumDataRateIndex, txChannels.channel[channelIndex].maximumDataRateIndex);
    }
    logging::snprintf("  Rx2 channel    : frequency = %09u [Hz], rx1DROffset = %u, rx2DR = %u\n", rx2FrequencyInHz, rx1DataRateOffset, rx2DataRateIndex);
}

void LoRaWAN::dumpRawMessage() {
    if (!logging::isActive() || !logging::isActive(logging::source::lorawanData)) {
        return;
    }
    dumpRawMessageHeader();
    if (framePortLength > 0) {
        logging::snprintf("[%03u]      : FramePort %02X\n", (framePortOffset - macHeaderOffset), rawMessage[framePortOffset]);
    }
    dumpRawMessagePayload();
    logging::snprintf("[%03u..%03u] : MIC %02X %02X %02X %02X\n\n", (micOffset - macHeaderOffset), (micOffset - macHeaderOffset + 4), rawMessage[micOffset], rawMessage[micOffset + 1], rawMessage[micOffset + 2], rawMessage[micOffset + 3]);
}

void LoRaWAN::dumpRawMessageHeader() {
    if (!logging::isActive() || !logging::isActive(logging::source::lorawanData)) {
        return;
    }
    logging::snprintf("LoRaWAN msg :\n");
    logging::snprintf("[000]      : MacHeader %02X \n", rawMessage[macHeaderOffset]);
    logging::snprintf("[001..004] : DevAddr %02X %02X %02X %02X\n", rawMessage[deviceAddressOffset], rawMessage[deviceAddressOffset + 1], rawMessage[deviceAddressOffset + 2], rawMessage[deviceAddressOffset + 3]);
    logging::snprintf("[005]      : FCtrl %02X\n", rawMessage[frameControlOffset]);
    logging::snprintf("[006..007] : FCnt %02X %02X\n", rawMessage[frameCountOffset], rawMessage[frameCountOffset + 1]);
    if (frameOptionsLength > 0) {
        logging::snprintf("[008..%03u] : FrameOptions ", 8 + frameOptionsLength);
        for (uint8_t i = 0; i < frameOptionsLength; i++) {
            logging::snprintf("%02X ", rawMessage[frameOptionsOffset + i]);
        }
        logging::snprintf("\n");
    }
}

void LoRaWAN::dumpRawMessagePayload() {
    if (!logging::isActive() || !logging::isActive(logging::source::lorawanData)) {
        return;
    }
    if (framePayloadLength > 0) {
        logging::snprintf("[%03u..%03u] : FramePayload[%u]  ", (framePayloadOffset - macHeaderOffset), (framePayloadOffset + framePayloadLength - (macHeaderOffset + 1)), framePayloadLength);
        for (uint8_t i = 0; i < framePayloadLength; i++) {
            logging::snprintf("%02X ", rawMessage[framePayloadOffset + i]);
        }
        logging::snprintf("\n");
    }
}

void LoRaWAN::dumpTransmitSettings() {
    if (!logging::isActive() || !logging::isActive(logging::source::lorawanData)) {
        return;
    }

    uint32_t txFrequency = txChannels.channel[txChannels.getCurrentChannelIndex()].frequencyInHz;
    spreadingFactor csf  = theDataRates.theDataRates[currentDataRateIndex].theSpreadingFactor;
    logging::snprintf("Uplink :\n");
    logging::snprintf("  dataRate = %u\n", currentDataRateIndex);
    logging::snprintf("  spreadingFactor = %s\n", toString(csf));
    logging::snprintf("  channel = %u\n", txChannels.getCurrentChannelIndex());
    logging::snprintf("  frequency = %u\n\n", txFrequency);
}

void LoRaWAN::dumpLinkCheckAnswer() {}
void LoRaWAN::dumpLinkAdaptiveDataRateRequest() {}
void LoRaWAN::dumpDutyCycleRequest() {}
void LoRaWAN::dumpDeviceStatusRequest() {}
void LoRaWAN::dumpNewChannelRequest() {}
void LoRaWAN::dumpNewChannelRequest(uint32_t channelIndex, uint32_t frequency, uint32_t minimumDataRate, uint32_t maximumDataRate) {}
void LoRaWAN::dumpReceiveParameterSetupRequest() {}
void LoRaWAN::dumpReceiveTimingSetupRequest() {}
void LoRaWAN::dumpReceiveTimingSetupRequest(uint32_t rx1Delay) {}
void LoRaWAN::dumpTransmitParameterSetupRequest() {}
void LoRaWAN::dumpDownlinkChannelRequest() {}
void LoRaWAN::dumpDeviceTimeAnswer() {}

// if (logging::isActive(logging::source::lorawanMac)) {
//     logging::snprintf("Received Downlink : framePayloadLength = %u, frameOptionsLength = %u\n", framePayloadLength, frameOptionsLength);
//     logging::snprintf("  LoRa msg = ");
//     for (uint8_t i = 0; i < loRaPayloadLength; i++) {
//         logging::snprintf("%02X ", rawMessage[i + b0BlockLength]);
//     }
//     logging::snprintf("\n");
// }
