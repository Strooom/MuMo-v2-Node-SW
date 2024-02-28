#include <lorawan.hpp>
#include <sx126x.hpp>
#include <logging.hpp>
#include <circularbuffer.hpp>
#include <settingscollection.hpp>
#include <applicationevent.hpp>
#include <maccommand.hpp>
#include <aesblock.hpp>
#include <stm32wle5_aes.hpp>
#include <hexascii.hpp>
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
frameCount LoRaWAN::uplinkFrameCount;
frameCount LoRaWAN::downlinkFrameCount;

dataRates LoRaWAN::theDataRates;
uint32_t LoRaWAN::currentDataRateIndex{0};
loRaTxChannelCollection LoRaWAN::txChannels;
uint32_t LoRaWAN::rx1DelayInSeconds{1};
uint32_t LoRaWAN::rx2DataRateIndex{0};
uint32_t LoRaWAN::rx2FrequencyInHz{869525000};
uint32_t LoRaWAN::rx1DataRateOffset{0};

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

uint32_t LoRaWAN::nmbrOfBytesToPad{};

txRxCycleState LoRaWAN::theTxRxCycleState{txRxCycleState::idle};        // state variable tracking the TxRxCycle state machine : TODO : provide a getter()

linearBuffer<64> LoRaWAN::macIn;         // buffer holding the received MAC requests and/or answers
linearBuffer<64> LoRaWAN::macOut;        // buffer holding the MAC requests and/or answers to be sent

#pragma endregion
#pragma region Global Objects & Variables

extern circularBuffer<applicationEvent, 16U> applicationEventBuffer;

#pragma endregion
#pragma region 0 : General

void LoRaWAN::initialize() {
    theTxRxCycleState = txRxCycleState::idle;
    macIn.initialize();
    macOut.initialize();
    clearRawMessage();
    restoreConfig();
    restoreState();
    restoreChannels();

    dumpConfig();
    dumpState();
    dumpChannels();

    sx126x::initialize();
}

void LoRaWAN::saveConfig() {
    settingsCollection::save(DevAddr.asUint32, settingsCollection::settingIndex::DevAddr);
    settingsCollection::saveByteArray(applicationKey.asBytes(), settingsCollection::settingIndex::applicationSessionKey);
    settingsCollection::saveByteArray(networkKey.asBytes(), settingsCollection::settingIndex::networkSessionKey);
}

void LoRaWAN::saveState() {
    settingsCollection::save(uplinkFrameCount.toUint32(), settingsCollection::settingIndex::uplinkFrameCounter);
    settingsCollection::save(downlinkFrameCount.toUint32(), settingsCollection::settingIndex::downlinkFrameCounter);
    settingsCollection::save(static_cast<uint8_t>(rx1DelayInSeconds), settingsCollection::settingIndex::rx1Delay);
    settingsCollection::save(static_cast<uint8_t>(currentDataRateIndex), settingsCollection::settingIndex::dataRate);
}

void LoRaWAN::saveChannels() {
    // txChannels
    uint8_t tmpChannelData[loRaTxChannelCollection::maxNmbrChannels * loRaChannel::sizeInBytes];
    for (uint32_t channelIndex = 0; channelIndex < loRaTxChannelCollection::maxNmbrChannels; channelIndex++) {
        txChannels.channel[channelIndex].toBytes(tmpChannelData + (channelIndex * loRaChannel::sizeInBytes));
    }
    settingsCollection::saveByteArray(tmpChannelData, settingsCollection::settingIndex::txChannels);

    // rxChannel
    // TODO : implement this
    settingsCollection::saveByteArray(tmpChannelData, settingsCollection::settingIndex::rxChannel);
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
    uplinkFrameCount     = settingsCollection::read<uint32_t>(settingsCollection::settingIndex::uplinkFrameCounter);
    downlinkFrameCount   = settingsCollection::read<uint32_t>(settingsCollection::settingIndex::downlinkFrameCounter);
    rx1DelayInSeconds    = settingsCollection::read<uint8_t>(settingsCollection::settingIndex::rx1Delay);
    currentDataRateIndex = settingsCollection::read<uint8_t>(settingsCollection::settingIndex::dataRate);
}

void LoRaWAN::restoreChannels() {
    // txChannels
    uint8_t tmpChannelData[loRaTxChannelCollection::maxNmbrChannels * loRaChannel::sizeInBytes];
    settingsCollection::readByteArray(tmpChannelData, settingsCollection::settingIndex::txChannels);

    for (uint32_t channelIndex = 0; channelIndex < loRaTxChannelCollection::maxNmbrChannels; channelIndex++) {
        txChannels.channel[channelIndex].fromBytes(tmpChannelData + (channelIndex * loRaChannel::sizeInBytes));
    }

    // rxChannel
    settingsCollection::readByteArray(tmpChannelData, settingsCollection::settingIndex::rxChannel);

    // TODO : restore rxChannel settings
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
    memcpy(rawMessage + framePayloadOffset, data, length);
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
    rawMessage[0]  = 0x49;                                      // see LoRaWAN® L2 1.0.4 Specification - line 808
    rawMessage[1]  = 0;                                         //
    rawMessage[2]  = 0;                                         //
    rawMessage[3]  = 0;                                         //
    rawMessage[4]  = 0;                                         //
    rawMessage[5]  = static_cast<uint8_t>(theDirection);        // 0x00 for uplink, 0x01 for downlink
    rawMessage[6]  = DevAddr.asUint8[0];                        // LSByte
    rawMessage[7]  = DevAddr.asUint8[1];                        //
    rawMessage[8]  = DevAddr.asUint8[2];                        //
    rawMessage[9]  = DevAddr.asUint8[3];                        // MSByte
    rawMessage[10] = aFrameCount[0];                            // LSByte
    rawMessage[11] = aFrameCount[1];                            //
    rawMessage[12] = aFrameCount[2];                            //
    rawMessage[13] = aFrameCount[3];                            // MSByte
    rawMessage[14] = 0;                                         //
    rawMessage[15] = micOffset;                                 // number of bytes over which the MIC is applied
}

void LoRaWAN::padForMicCalculation() {
    nmbrOfBytesToPad = aesBlock::calculateNmbrOfBytesToPad(loRaPayloadLength - 4);
    if (nmbrOfBytesToPad > 0) {
        rawMessage[micOffset] = 0x80;
    }
    for (uint32_t index = micOffset + 1; index < (micOffset + nmbrOfBytesToPad); index++) {
        rawMessage[index] = 0x00;
    }
}

uint16_t LoRaWAN::getReceivedFramecount() {
    return (static_cast<uint16_t>(rawMessage[frameCountOffset]) + (static_cast<uint16_t>(rawMessage[frameCountOffset + 1]) << 8));
}

uint32_t LoRaWAN::getReceivedDeviceAddress() {
    return (static_cast<uint32_t>(rawMessage[deviceAddressOffset]) + (static_cast<uint32_t>(rawMessage[deviceAddressOffset + 1]) << 8) + (static_cast<uint32_t>(rawMessage[deviceAddressOffset + 2]) << 16) + (static_cast<uint32_t>(rawMessage[deviceAddressOffset + 3]) << 24));
}

uint32_t LoRaWAN::getReceivedMic() {
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

void LoRaWAN::encryptPayload(aesKey& theKey) {
    uint32_t nmbrOfBlocks            = aesBlock::nmbrOfBlocksFromBytes(framePayloadLength);
    uint32_t incompleteLastBlockSize = aesBlock::incompleteLastBlockSizeFromBytes(framePayloadLength);
    bool hasIncompleteBlock          = aesBlock::hasIncompleteLastBlockFromBytes(framePayloadLength);

#ifdef HARDWARE_AES

    stm32wle5_aes::initialize(aesMode::CTR);
    aesBlock A1;
    prepareBlockAi(A1, linkDirection::uplink, 1);
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

    for (uint32_t blockIndex = 0x00; blockIndex < nmbrOfBlocks; blockIndex++) {
        prepareBlockAi(theBlock, linkDirection::uplink, (blockIndex + 1));
        theBlock.encrypt(theKey);

        if (hasIncompleteBlock && (blockIndex == (nmbrOfBlocks - 1))) {
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

void LoRaWAN::decryptPayload(aesKey& theKey) {
    uint32_t nmbrOfBlocks            = aesBlock::nmbrOfBlocksFromBytes(framePayloadLength);
    uint32_t incompleteLastBlockSize = aesBlock::incompleteLastBlockSizeFromBytes(framePayloadLength);
    bool hasIncompleteBlock          = aesBlock::hasIncompleteLastBlockFromBytes(framePayloadLength);

#ifdef HARDWARE_AES
    stm32wle5_aes::initialize(aesMode::CTR);
    aesBlock A1;
    prepareBlockAi(A1, linkDirection::downlink, 1);
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

    for (uint32_t blockIndex = 0x00; blockIndex < nmbrOfBlocks; blockIndex++) {
        prepareBlockAi(theBlock, linkDirection::downlink, (blockIndex + 1));
        theBlock.encrypt(theKey);

        if (hasIncompleteBlock && (blockIndex == (nmbrOfBlocks - 1))) {
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
    uint32_t payloadLength = b0BlockLength + loRaPayloadLength - micLength;
    uint32_t nmbrOfBlocks  = aesBlock::nmbrOfBlocksFromBytes(payloadLength);

#ifdef HARDWARE_AES
    stm32wle5_aes::initialize(aesMode::CBC);
    stm32wle5_aes::setKey(networkKey);
    stm32wle5_aes::setInitializationVector(input); // TODO 
    stm32wle5_aes::enable();
    aesBlock tmpBlock;
    for (uint32_t blockIndex = 0; blockIndex < nmbrOfBlocks; blockIndex++) {
        uint8_t* tmpOffset;
        tmpOffset = rawMessage + (blockIndex * 16);
        tmpBlock.setFromByteArray(tmpOffset);
        stm32wle5_aes::write(tmpBlock);
        while (!stm32wle5_aes::isComputationComplete()) {
        }
        stm32wle5_aes::read(tmpBlock);
        stm32wle5_aes::clearComputationComplete();
    }
    uint32_t mic = tmpBlock.asWords()[0];
    stm32wle5_aes::disable();

#else
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
            memcpy(outputAsBytes, outputBlock.asBytes(), 16);

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
        outputBlock.XOR(Old_Data);
        outputBlock.encrypt(networkKey);
    }
    uint32_t mic = ((outputBlock.asBytes()[0] << 24) + (outputBlock.asBytes()[1] << 16) + (outputBlock.asBytes()[2] << 8) + (outputBlock.asBytes()[3]));
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

void LoRaWAN::run() {
    if ((macOut.getLevel() > 15) && isIdle()) {        // if we have more than 15 bytes of MAC stuff, we need a separate uplink message (cannot piggyback on a data message), so we send the msg now
        sendUplink();                                  // start an uplink cycle with the MAC stuff on port 0
        removeNonStickyMacStuff();                     // after all MAC stuff was sent, remove it from the macOut buffer, except for the sticky MAC stuff, which is only removed after receiving a donwlink
    }
}

txRxCycleState LoRaWAN::getState() {
    return theTxRxCycleState;
}

bool LoRaWAN::isIdle() {
    return (theTxRxCycleState == txRxCycleState::idle);
}

void LoRaWAN::handleEvents(applicationEvent theEvent) {
    logging::snprintf(logging::source::lorawanEvents, "LoRaWAN event [%u / %s] in state [%u / %s] \n", static_cast<uint8_t>(theEvent), toString(theEvent), static_cast<uint8_t>(theTxRxCycleState), toString(theTxRxCycleState));

    switch (theTxRxCycleState) {
        case txRxCycleState::idle:
            logging::snprintf(logging::source::error, "Error : received event [%u / %s] in state [%u / %s] \n", static_cast<uint8_t>(theEvent), toString(theEvent), static_cast<uint8_t>(theTxRxCycleState), toString(theTxRxCycleState));
            break;

        case txRxCycleState::waitForCadEnd:
            switch (theEvent) {
                case applicationEvent::sx126xCadEnd:
                    break;
                default:
                    logging::snprintf(logging::source::error, "Error : received event [%u / %s] in state [%u / %s] \n", static_cast<uint8_t>(theEvent), toString(theEvent), static_cast<uint8_t>(theTxRxCycleState), toString(theTxRxCycleState));
                    goTo(txRxCycleState::idle);
                    break;
            }
            break;

        case txRxCycleState::waitForRandomTimeBeforeTransmit:
            switch (theEvent) {
                case applicationEvent::lowPowerTimerExpired:
                    sx126x::startTransmit(128000U);
                    goTo(txRxCycleState::waitForTxComplete);
                    break;
                default:
                    logging::snprintf(logging::source::error, "Error : received event [%u / %s] in state [%u / %s] \n", static_cast<uint8_t>(theEvent), toString(theEvent), static_cast<uint8_t>(theTxRxCycleState), toString(theTxRxCycleState));
                    goTo(txRxCycleState::idle);
                    break;
            }
            break;

        case txRxCycleState::waitForTxComplete:
            switch (theEvent) {
                case applicationEvent::sx126xTxComplete: {
                    uint32_t timerLoadValue = (rx1DelayInSeconds * 2048) - 64;        // 2048 is a full second. 32 is some time lost in starting and stopping the timer TODO : make this delta a constexpr member of the class
                    startTimer(timerLoadValue);
                    // 2048 would be 1.0s @ 2KHz timer, but I measured 1.012s (some overhead is involved)
                    // 2016 resulting in 996 ms delay measured on the scope
                    // 2020 resulting in 998 ms delay measured on the scope
                    goTo(txRxCycleState::waitForRx1Start);
                } break;
                case applicationEvent::sx126xTimeout:
                    logging::snprintf(logging::source::error, "Error : received event [%u / %s] in state [%u / %s] \n", static_cast<uint8_t>(theEvent), toString(theEvent), static_cast<uint8_t>(theTxRxCycleState), toString(theTxRxCycleState));
                    goTo(txRxCycleState::idle);
                    break;
                default:
                    logging::snprintf(logging::source::error, "Error : received event [%u / %s] in state [%u / %s] \n", static_cast<uint8_t>(theEvent), toString(theEvent), static_cast<uint8_t>(theTxRxCycleState), toString(theTxRxCycleState));
                    goTo(txRxCycleState::idle);
                    break;
            }
            break;

        case txRxCycleState::waitForRx1Start:
            switch (theEvent) {
                case applicationEvent::lowPowerTimerExpired: {
                    stopTimer();
                    startTimer(2048U);        // 1 second from now until Rx2Start
                    uint32_t rxFrequency = txChannels.channel[txChannels.getCurrentChannelIndex()].frequencyInHz;
                    uint32_t rxTimeout   = getReceiveTimeout(theDataRates.theDataRates[currentDataRateIndex].theSpreadingFactor);
                    sx126x::configForReceive(theDataRates.theDataRates[currentDataRateIndex].theSpreadingFactor, rxFrequency);
                    sx126x::startReceive(rxTimeout);
                    goTo(txRxCycleState::waitForRx1CompleteOrTimeout);
                } break;
                default:
                    logging::snprintf(logging::source::error, "Error : received event [%u / %s] in state [%u / %s] \n", static_cast<uint8_t>(theEvent), toString(theEvent), static_cast<uint8_t>(theTxRxCycleState), toString(theTxRxCycleState));
                    break;
            }
            break;

        case txRxCycleState::waitForRx1CompleteOrTimeout:
            switch (theEvent) {
                case applicationEvent::sx126xRxComplete: {
                    messageType receivedMessageType = decodeMessage();
                    logging::snprintf(logging::source::applicationEvents, "Received LoRaWAN  %s msg in Rx1\n", toString(receivedMessageType));
                    switch (receivedMessageType) {
                        case messageType::application:
                            stopTimer();
                            processMacContents();
                            applicationEventBuffer.push(applicationEvent::downlinkApplicationPayloadReceived);
                            goTo(txRxCycleState::waitForRxMessageReadout);
                            break;
                        case messageType::lorawanMac:
                            stopTimer();
                            processMacContents();
                            goTo(txRxCycleState::idle);
                            break;
                        default:
                        case messageType::invalid:
                            goTo(txRxCycleState::waitForRx2Start);
                            break;
                    }
                } break;
                case applicationEvent::sx126xTimeout:
                    goTo(txRxCycleState::waitForRx2Start);
                    break;
                default:
                    logging::snprintf(logging::source::error, "Error : received event [%u / %s] in state [%u / %s] \n", static_cast<uint8_t>(theEvent), toString(theEvent), static_cast<uint8_t>(theTxRxCycleState), toString(theTxRxCycleState));
                    goTo(txRxCycleState::idle);
                    break;
            }
            break;

        case txRxCycleState::waitForRx2Start:
            switch (theEvent) {
                case applicationEvent::lowPowerTimerExpired: {
                    stopTimer();
                    uint32_t rxTimeout = getReceiveTimeout(spreadingFactor::SF9);
                    sx126x::configForReceive(spreadingFactor::SF9, rx2FrequencyInHz);
                    sx126x::startReceive(rxTimeout);
                    goTo(txRxCycleState::waitForRx2CompleteOrTimeout);
                } break;
                default:
                    logging::snprintf(logging::source::error, "Error : received event [%u / %s] in state [%u / %s] \n", static_cast<uint8_t>(theEvent), toString(theEvent), static_cast<uint8_t>(theTxRxCycleState), toString(theTxRxCycleState));
                    goTo(txRxCycleState::idle);
                    break;
            }
            break;

        case txRxCycleState::waitForRx2CompleteOrTimeout:
            switch (theEvent) {
                case applicationEvent::sx126xRxComplete: {
                    messageType receivedMessageType = decodeMessage();
                    logging::snprintf(logging::source::applicationEvents, "Received LoRaWAN  %s msg in Rx2\n", toString(receivedMessageType));
                    switch (receivedMessageType) {
                        case messageType::application:
                            processMacContents();
                            applicationEventBuffer.push(applicationEvent::downlinkApplicationPayloadReceived);
                            goTo(txRxCycleState::waitForRxMessageReadout);
                            break;
                        case messageType::lorawanMac:
                            processMacContents();
                            goTo(txRxCycleState::idle);
                            break;
                        default:
                        case messageType::invalid:
                            goTo(txRxCycleState::idle);
                            break;
                    }
                } break;
                case applicationEvent::sx126xTimeout:
                    goTo(txRxCycleState::idle);
                    break;
                default:
                    logging::snprintf(logging::source::error, "Error : received event [%u / %s] in state [%u / %s] \n", static_cast<uint8_t>(theEvent), toString(theEvent), static_cast<uint8_t>(theTxRxCycleState), toString(theTxRxCycleState));
                    goTo(txRxCycleState::idle);
                    break;
            }
            break;
        default:
            break;
    }
}

void LoRaWAN::goTo(txRxCycleState newState) {
    logging::snprintf(logging::source::applicationEvents, "LoRaWAN stateChange from [%d / %s] to [%d / %s]\n", theTxRxCycleState, toString(theTxRxCycleState), newState, toString(newState));
    // Tasks to do when leaving the current state
    switch (theTxRxCycleState) {
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
    theTxRxCycleState = newState;
    // Tasks to do when entering the new state
    switch (newState) {
        case txRxCycleState::idle:
            sx126x::goSleep(sx126x::sleepMode::warmStart);
            break;

        case txRxCycleState::waitForRandomTimeBeforeTransmit: {
            uint32_t randomDelayAsTicks = getRandomNumber() % maxRandomDelayBeforeTx;        // this results in a random delay of 0.. 8 seconds
            // float randomDelayAsFloat    = static_cast<float>(randomDelayAsTicks) / 2048.0f;        // convert to seconds
            // logging::snprintf(logging::source::lorawanTiming, "LoRaWAN random delay before transmit : %u ticks, %f seconds\n", randomDelayAsTicks, randomDelayAsFloat);        //
            startTimer(randomDelayAsTicks);        //
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

void LoRaWAN::startTimer(uint32_t timeOut) {
#ifndef generic
    HAL_LPTIM_SetOnce_Start_IT(&hlptim1, 0xFFFF, timeOut);
    // logging::snprintf(logging::source::lorawanTiming, "started = %u\n", HAL_GetTick());
#endif
}

void LoRaWAN::stopTimer() {
#ifndef generic
    // logging::snprintf(logging::source::lorawanTiming, "stopped = %u\n", HAL_GetTick());
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
    uint8_t downLinkSettings = macIn[1];
    uint32_t frequency       = (static_cast<uint32_t>(macIn[2]) + (static_cast<uint32_t>(macIn[3]) << 8) + (static_cast<uint32_t>(macIn[4]) << 16)) * 100;
    macIn.consume(receiveParameterSetupRequestLength);
    logging::snprintf(logging::source::lorawanMac, "ReceiveParameterSetupRequest : 0x%02X, %u \n", downLinkSettings, frequency);

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
    constexpr uint32_t newChannelRequestLength{6};                                                                                                                // 6 bytes : commandId, channelIndex, frequency[3], DRrange
    uint32_t channelIndex    = macIn[1];                                                                                                                          //
    uint32_t frequency       = (static_cast<uint32_t>(macIn[2]) + (static_cast<uint32_t>(macIn[3]) << 8) + (static_cast<uint32_t>(macIn[4]) << 16)) * 100;        //
    uint32_t minimumDataRate = macIn[5] & 0x0F;                                                                                                                   //
    uint32_t maximumDataRate = (macIn[5] & 0xF0) >> 4;
    processNewChannelRequest(channelIndex, frequency, minimumDataRate, maximumDataRate);
    macIn.consume(newChannelRequestLength);

    constexpr uint32_t newChannelAnswerLength{2};                                                     // 2 bytes : commandId, status
    uint8_t answer[newChannelAnswerLength];                                                           //
    answer[0] = static_cast<uint8_t>(macCommand::newChannelAnswer);                                   //
    answer[1] = static_cast<uint8_t>(0b00000011);                                                     // The end-device accepts datarange and frequency
    macOut.append(answer, newChannelAnswerLength);                                                    //
    logging::snprintf(logging::source::lorawanMac, "NewChannelAnswer : 0x%02X \n", answer[1]);        //
}

void LoRaWAN::processNewChannelRequest(uint32_t channelIndex, uint32_t frequency, uint32_t minimumDataRate, uint32_t maximumDataRate) {
    if ((channelIndex < 3) || (channelIndex > 15)) {
        logging::snprintf(logging::source::error, "NewChannelRequest for channel %u \n", channelIndex);
        // channels 0..2 are always active and newChannelRequest should be ignored on them... L2 Spec 1.0.4 line 1135
        // EU-868 only has 16 channels, 0..15
        return;
    }
    logging::snprintf(logging::source::lorawanMac, "NewChannelRequest for channel %u, frequency = %u, minimumDataRate = %u, maximumDataRate = %u \n", channelIndex, frequency, minimumDataRate, maximumDataRate);
    txChannels.channel[channelIndex].frequencyInHz        = frequency;        // NOTE : frequency of 0 deactivates channel..
    txChannels.channel[channelIndex].minimumDataRateIndex = minimumDataRate;
    txChannels.channel[channelIndex].maximumDataRateIndex = maximumDataRate;
    // TODO : save the new channel info in the NVS
}

void LoRaWAN::processReceiveTimingSetupRequest() {
    constexpr uint32_t receiveTimingSetupRequestLength{2};        // 2 bytes : commandId, delay
    uint32_t receivedRx1Delay = macIn[1] & 0x0F;                  // TODO : a value of 0 means also 1 second
    processReceiveTimingSetupRequest(receivedRx1Delay);
    macIn.consume(receiveTimingSetupRequestLength);        //

    constexpr uint32_t receiveTimingSetupAnswerLength{1};        // 1 byte : commandId
    uint8_t answer[receiveTimingSetupAnswerLength];
    answer[0] = static_cast<uint8_t>(macCommand::receiveTimingSetupAnswer);
    macOut.append(answer, receiveTimingSetupAnswerLength);
    logging::snprintf(logging::source::lorawanMac, "ReceiveTimingSetupAnswer \n");
}

void LoRaWAN::processReceiveTimingSetupRequest(uint32_t receivedRx1Delay) {
    if (receivedRx1Delay == 0) {
        receivedRx1Delay = 1;        // a value of 0, also results in a delay of 1 second. Link Layer Spec V1.0.4, line 1237
    }
    logging::snprintf(logging::source::lorawanMac, "ReceiveTimingSetupRequest : rx1Delay changed from %u to %u\n", rx1DelayInSeconds, receivedRx1Delay);        //
    rx1DelayInSeconds = receivedRx1Delay;
    settingsCollection::save(rx1DelayInSeconds, settingsCollection::settingIndex::rx1Delay);
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

    // Wake the radio up..
    sx126x::initializeInterface();        // does HAL_SUBGHZ_Init
    sx126x::initializeRadio();            // writes all config registers.. should not be needed if config is retained

    if (theFramePort == 0) {
        // uplink message with framePort 0, containing no frameOptions and framePayload is all MAC stuff, encrypted with networkKey
        setOffsetsAndLengthsTx(macOut.getLevel());                    // sending all MAC stuff in the framePayload, so frameOptions are empty and frameOptionsLength is 0
        insertHeaders(nullptr, 0, macOut.getLevel(), 0);              //
        insertPayload(macOut.asUint8Ptr(), macOut.getLevel());        // TODO : test the length of the MAC stuff we are going to send, so it does not exceed the maximum allowed length
        encryptPayload(networkKey);                                   //
    } else {
        // uplink with application payload, encrypted with applicationKey. Optionally up to 15 bytes of (unencrypted) frameOptions in the header
        setOffsetsAndLengthsTx(applicationDataLength, macOut.getLevel());                                  // sending application payload + <= 15 bytes , so frameOptionsLength is 15
        insertHeaders(macOut.asUint8Ptr(), macOut.getLevel(), applicationDataLength, theFramePort);        //
        insertPayload(applicationData, applicationDataLength);                                             //
        encryptPayload(applicationKey);                                                                    //
    }
    insertBlockB0(linkDirection::uplink, uplinkFrameCount);
    insertMic();

    // 2. Configure the radio, and start stateMachine for transmitting the payload
    txChannels.selectRandomChannelIndex();        // randomize the channel index
    uint32_t txFrequency = txChannels.channel[txChannels.getCurrentChannelIndex()].frequencyInHz;
    spreadingFactor csf  = theDataRates.theDataRates[currentDataRateIndex].theSpreadingFactor;
    sx126x::configForTransmit(csf, txFrequency, rawMessage + macHeaderOffset, loRaPayloadLength);

    if (logging::isActive(logging::source::lorawanMac)) {
        logging::snprintf("Scheduled Uplink : channel = %u, frequency = %u, dataRate = %u, framePayloadLength = %u, frameOptionsLength = %u\n", txChannels.getCurrentChannelIndex(), txFrequency, currentDataRateIndex, framePayloadLength, frameOptionsLength);
        logging::snprintf("- LoRa msg = ");
        for (uint8_t i = 0; i < loRaPayloadLength; i++) {
            logging::snprintf("%02X ", rawMessage[i + b0BlockLength]);
        }
        logging::snprintf("\n");
    }

    goTo(txRxCycleState::waitForRandomTimeBeforeTransmit);

    // 3. txRxCycle is started..
    uplinkFrameCount++;               //
                                      //    nonVolatileStorage::write(static_cast<uint32_t>(nvsMap::blockIndex::uplinkFrameCounter), uplinkFrameCount.asUint32);        // TODO : update the setting/context and let the NVS do things under the hood
    removeNonStickyMacStuff();        //
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
    sx126x::readBuffer(rawMessage + b0BlockLength, loRaPayloadLength);        // This reads the full LoRaWAN payload into rawMessage buffer, at an offset so the B0 block still fits in front
    setOffsetsAndLengthsRx(loRaPayloadLength);

    if (logging::isActive(logging::source::lorawanMac)) {
        logging::snprintf("Received Downlink : framePayloadLength = %u, frameOptionsLength = %u\n", framePayloadLength, frameOptionsLength);
        logging::snprintf("- LoRa msg = ");
        for (uint8_t i = 0; i < loRaPayloadLength; i++) {
            logging::snprintf("%02X ", rawMessage[i + b0BlockLength]);
        }
        logging::snprintf("\n");
    }

    // 2. Extract & guess the downLinkFrameCount, as we need this to check the MIC
    uint16_t receivedDownlinkFramecount = getReceivedFramecount();
    frameCount guessedDownlinkFramecount;
    guessedDownlinkFramecount = downlinkFrameCount;
    guessedDownlinkFramecount.guessFromUint16(receivedDownlinkFramecount);
    logging::snprintf(logging::source::lorawanMac, "receivedFramecount = %u, lastFramecount = %u, guessedFramecount = %u\n", receivedDownlinkFramecount, downlinkFrameCount.toUint32(), guessedDownlinkFramecount.toUint32());

    // 3. Check the MIC
    uint32_t receivedMic = getReceivedMic();
    insertBlockB0(linkDirection::downlink, guessedDownlinkFramecount);
    uint32_t calculatedMic = calculateMic();
    if (receivedMic != calculatedMic) {
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
    deviceAddress receivedDeviceAddress(getReceivedDeviceAddress());
    if (receivedDeviceAddress != DevAddr) {
        logging::snprintf(logging::source::lorawanMac, "Msg for other device : %08X\n", receivedDeviceAddress.asUint32);        // TODO : also log received deviceAddress
        return messageType::invalid;
    }

    // 5. check if the frameCount is valid
    if (!isValidDownlinkFrameCount(guessedDownlinkFramecount)) {
        logging::snprintf(logging::source::error, "Error : invalid downlinkFrameCount : received %u, current %u\n", guessedDownlinkFramecount.toUint32(), downlinkFrameCount.toUint32());
        return messageType::invalid;
    }

    // 6. Seems a valid message, so update the downlinkFrameCount to what we've received (not just incrementing it, as there could be gaps in the sequence due to lost packets)
    downlinkFrameCount = guessedDownlinkFramecount;
    // nonVolatileStorage::write(static_cast<uint32_t>(nvsMap::blockIndex::downlinkFrameCounter), downlinkFrameCount.asUint32); TODO : reactivate

    // 6.5 If we had sticky macOut stuff, we can clear that now, as we did receive a valid downlink
    macOut.initialize();

    // 7. If we have MAC stuff in the frameOptions, extract them
    if (frameOptionsLength > 0) {
        macIn.append(rawMessage + frameOptionsOffset, frameOptionsLength);
        // TODO : we could remember this with a flag, because the combination of MAC stuff in frameOptions and in framePayload (with framePort == 0) is not allowed
    }

    // 8. If there is any framePayload, decrypt if
    if (framePayloadLength > 0) {
        if (rawMessage[framePortOffset] == 0) {
            decryptPayload(networkKey);
            macIn.append(rawMessage + framePayloadOffset, framePayloadLength);
            return messageType::lorawanMac;
        } else {
            decryptPayload(applicationKey);
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
        return (testFrameCount.toUint32() > downlinkFrameCount.toUint32());        // TODO : overload > operator
    }
}

void LoRaWAN::checkNetwork() {
    macOut.append(static_cast<uint8_t>(macCommand::linkCheckRequest));
}

uint32_t LoRaWAN::getRandomNumber() {
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
    if (logging::isActive(logging::source::lorawanMac)) {
        uint32_t tmpMacOutLevel = macOut.getLevel();
        logging::snprintf("MAC output [%d] = ", tmpMacOutLevel);
        for (uint32_t i = 0; i < tmpMacOutLevel; i++) {
            logging::snprintf("%02X ", macOut[i]);
        }
        logging::snprintf("\n");
    }
}

void LoRaWAN::dumpMacIn() {
    if (logging::isActive(logging::source::lorawanMac)) {
        uint32_t tmpMacInLevel = macIn.getLevel();
        logging::snprintf("MAC input [%d] = ", tmpMacInLevel);
        for (uint32_t i = 0; i < tmpMacInLevel; i++) {
            logging::snprintf("%02X ", macIn[i]);
        }
        logging::snprintf("\n");
    }
}

void LoRaWAN::dumpConfig() {
    if (logging::isActive(logging::source::lorawanMac)) {
        logging::snprintf("LoRaWAN Config :\n");
        logging::snprintf("- devAddr = %04X\n", DevAddr.asUint32);
        char tmpKeyAsHexAscii[33];
        hexAscii::byteArrayToHexString(tmpKeyAsHexAscii, applicationKey.asBytes(), 16);
        logging::snprintf("- applicationKey = %s\n", tmpKeyAsHexAscii);
        hexAscii::byteArrayToHexString(tmpKeyAsHexAscii, networkKey.asBytes(), 16);
        logging::snprintf("- networkKey = %s\n", tmpKeyAsHexAscii);
    }
}

void LoRaWAN::dumpState() {
    if (logging::isActive(logging::source::lorawanMac)) {
        logging::snprintf("LoRaWAN State :\n");
        logging::snprintf("- uplinkFrameCount = %u\n", uplinkFrameCount.toUint32());
        logging::snprintf("- downlinkFrameCount = %u\n", downlinkFrameCount.toUint32());
        logging::snprintf("- dataRateIndex = %u\n", currentDataRateIndex);
        logging::snprintf("- rx1Delay = %u [s]\n", rx1DelayInSeconds);
    }
}

void LoRaWAN::dumpChannels() {
    if (logging::isActive(logging::source::lorawanMac)) {
        logging::snprintf("LoRaWAN Channels :\n");
        for (uint32_t channelIndex = 0; channelIndex < loRaTxChannelCollection::maxNmbrChannels; channelIndex++) {
            logging::snprintf("- Tx channel[%u] : frequency = %u [Hz], minDR = %u, maxDR = %u\n", channelIndex, txChannels.channel[channelIndex].frequencyInHz, txChannels.channel[channelIndex].minimumDataRateIndex, txChannels.channel[channelIndex].maximumDataRateIndex);
        }
        logging::snprintf("- Rx2 channel : frequency = %u [Hz], rx1DROffset = %u, rx2DR = %u\n", rx2FrequencyInHz, rx1DataRateOffset, rx2DataRateIndex);
    }
}
