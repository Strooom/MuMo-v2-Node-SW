// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <cstdint>
#include <cstring>
#include <applicationevent.hpp>
#include <aeskey.hpp>
#include <messagetype.hpp>
#include <txrxcyclestate.hpp>
#include <macheader.hpp>
#include <deviceaddress.hpp>
#include <framecount.hpp>
#include <mic.hpp>
#include <datarate.hpp>
#include <txchannelcollection.hpp>
#include <transmitpower.hpp>
#include <linkdirection.hpp>
#include <spreadingfactor.hpp>
#include <linearbuffer.hpp>
#include <maccommand.hpp>

class LoRaWAN {
  public:
    static void initialize();

    static void restoreConfig();
    static void restoreState();
    static void restoreChannels();

    static void saveConfig();
    static void saveState();
    static void saveChannels();

    static void handleEvents(applicationEvent theEvent);
    static uint32_t getMaxApplicationPayloadLength();
    static void sendUplink(uint8_t framePort, const uint8_t payload[], uint32_t payloadLength);
    static void appendMacCommand(macCommand theMacCommand);
    static void getReceivedDownlinkMessage();
    static txRxCycleState getState();
    static bool isIdle();
    static void checkNetwork();

    static void dumpConfig();
    static void dumpState();
    static void dumpChannels();

    static void dumpMacIn();
    static void dumpMacOut();
    static void dumpRawMessage();
    static void dumpRawMessageHeader();
    static void dumpRawMessagePayload();
    static void dumpTransmitSettings();
    static void dumpLinkCheckAnswer();
    static void dumpLinkAdaptiveDataRateRequest();
    static void dumpDutyCycleRequest();
    static void dumpDeviceStatusRequest();
    static void dumpNewChannelRequest();
    static void dumpNewChannelRequest(uint32_t channelIndex, uint32_t frequency, uint32_t minimumDataRate, uint32_t maximumDataRate);
    static void dumpReceiveParameterSetupRequest();
    static void dumpReceiveTimingSetupRequest();
    static void dumpReceiveTimingSetupRequest(uint32_t rx1Delay);
    static void dumpTransmitParameterSetupRequest();
    static void dumpDownlinkChannelRequest();
    static void dumpDeviceTimeAnswer();

    static void correctDevAddrEndianness();

    static uint32_t margin;
    static uint32_t gatewayCount;
    static uint32_t currentDataRateIndex;
    static deviceAddress DevAddr;
    static aesKey applicationKey;
    static aesKey networkKey;
    static frameCount uplinkFrameCount;
    static frameCount downlinkFrameCount;
    static uint32_t rx1DelayInSeconds;

#ifndef unitTesting
  private:
#endif

    static txRxCycleState state;
    static void goTo(txRxCycleState newState);

    static dataRates theDataRates;
    static uint32_t rx1DataRateOffset;
    static uint32_t rx2DataRateIndex;
    static uint32_t rx2FrequencyInHz;
    static loRaTxChannelCollection txChannels;
    static constexpr uint32_t maxRandomDelayBeforeTx{4096U};

    // #################################################
    // ### Encoding and Decoding of LoRaWAN messages ###
    // #################################################

    static constexpr uint32_t maxLoRaPayloadLength{256};
    static constexpr uint32_t b0BlockLength{16};
    static constexpr uint32_t macHeaderLength{1};
    static constexpr uint32_t deviceAddressLength{deviceAddress::lengthInBytes};
    static constexpr uint32_t frameControlLength{1};
    static constexpr uint32_t frameCountLSHLength{2};
    static constexpr uint32_t micLength{4};

    static uint32_t macPayloadLength;
    static uint32_t framePortLength;
    static uint32_t frameOptionsLength;        // this can vary between 0 and 15
    static uint32_t frameHeaderLength;         // this can vary between 7 and 22
    static uint32_t framePayloadLength;
    static uint32_t loRaPayloadLength;

    static constexpr uint32_t loRaPayloadOffset{b0BlockLength};
    static constexpr uint32_t macHeaderOffset{b0BlockLength};
    static constexpr uint32_t deviceAddressOffset{b0BlockLength + macHeaderLength};
    static constexpr uint32_t frameControlOffset{b0BlockLength + macHeaderLength + deviceAddressLength};
    static constexpr uint32_t frameCountOffset{b0BlockLength + macHeaderLength + deviceAddressLength + frameControlLength};
    static constexpr uint32_t frameOptionsOffset{b0BlockLength + macHeaderLength + deviceAddressLength + frameControlLength + frameCountLSHLength};

    static uint32_t framePortOffset;
    static uint32_t framePayloadOffset;
    static uint32_t micOffset;

    static constexpr uint32_t rawMessageLength{b0BlockLength + maxLoRaPayloadLength + 15};
    static uint8_t rawMessage[rawMessageLength];
    static void clearRawMessage();

    // ################################################################
    // ### Helper functions for MAC layer                           ###
    // ################################################################

    static constexpr uint32_t macInOutLength{64};
    static linearBuffer<macInOutLength> macIn;
    static linearBuffer<macInOutLength> macOut;
    static void processMacContents();

    static void processLinkCheckAnswer();
    static void processLinkAdaptiveDataRateRequest();
    static void processDutyCycleRequest();
    static void processDeviceStatusRequest();
    static void processNewChannelRequest();
    static void processReceiveParameterSetupRequest();
    static void processReceiveTimingSetupRequest();
    static void processTransmitParameterSetupRequest();
    static void processDownlinkChannelRequest();
    static void processDeviceTimeAnswer();

    static void removeNonStickyMacStuff();

    // ################################################################
    // ### Helper functions for Message Integrity Code (MIC)        ###
    // ################################################################

    static aesBlock K1;
    static aesBlock K2;
    static void generateKeysK1K2();
    static uint32_t calculateMic();

    // ################################################################
    // ### Helper functions for constructing an uplink message - Tx ###
    // ################################################################

    static void setOffsetsAndLengthsTx(uint32_t framePayloadLength, uint32_t frameOptionsLength = 0);
    static void insertPayload(const uint8_t data[], const uint32_t length);
    static void encryptDecryptPayload(aesKey &theKey, linkDirection theDirection);
    static void insertHeaders(const uint8_t theFrameOptions[], const uint32_t theFrameOptionslength, const uint32_t theFramePayloadLength, uint8_t theFramePort);
    static void padForMicCalculation(const uint32_t messageLength);
    static void insertMic();
    static void insertMic(uint32_t aMic);

    // #############################################################
    // ### Helper functions for decoding a downlink message - Rx ###
    // #############################################################

    static void setOffsetsAndLengthsRx(uint32_t loRaPayloadLength);
    static uint16_t receivedFramecount();
    static uint32_t receivedDeviceAddress();
    static uint32_t receivedMic();
    static bool isValidDownlinkFrameCount(frameCount testFrameCount);
    static messageType decodeMessage();

    // #############################################################
    // ### Other Helper functions                                ###
    // #############################################################

    static uint32_t randomNumber();
    static void insertBlockB0(linkDirection theDirection, frameCount &aFrameCount);        // in downlink, the framecount is the received framecount, not necessarily the downlink framecount so I need to pass it as a parameter

    static void prepareBlockAi(aesBlock &theBlock, linkDirection theDirection, uint32_t blockIndex);
    static uint32_t getReceiveTimeout(spreadingFactor aSpreadingfactor);

    static uint32_t calculateMaxTransmitTimeout(uint32_t currentDataRateIndex, uint32_t loRaPayloadLength);
};
