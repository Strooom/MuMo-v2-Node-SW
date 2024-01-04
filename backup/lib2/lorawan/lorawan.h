// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include <cstring>

#include "frameport.h"
#include "messagetype.h"
#include "txrxcycle.h"
#include "macheader.h"
#include "deviceaddress.h"
#include "aeskey.h"
#include "framecount.h"
#include "dutycycle.h"
#include "mic.h"
#include "datarate.h"
#include "channelcollection.h"
#include "transmitpower.h"
#include "linkdirection.h"
#include "spreadingfactor.h"
#include "linearbuffer.h"
#include "payloadencoder.h"

class LoRaWAN {
  public:
    static void initialize();                                                                                     // initialize the LoRaWAN layer
    static void run();                                                                                            // run the LoRaWAN MAC layer
    static void handleEvents();                                                                                   // handles events (timeouts and tx/rxComplete events)
    static bool isReadyToTransmit();                                                                              // is the LoRaWAN layer in a state, ready to transmit a message?
    static uint32_t getMaxApplicationPayloadLength();                                                             // how many [bytes] can the application send in one message? Depends on active dataRate and amount of MAC stuff waiting to be transmitted
    static void sendUplink(framePort aFramePort = 0, const uint8_t data[] = nullptr, uint32_t length = 0);        // send an uplink message,
    static void getDownlinkMessage();                                                                             //
    static void checkNetwork();

    static void logSettings();        // log the current settings of the LoRaWAN layer
    static void logState();           // log the current state of the LoRaWAN layer

    static txRxCycleState theTxRxCycleState;        // state variable tracking the TxRxCycle state machine : TODO : provide a getter()

#ifndef unitTesting
  private:
#endif

    static void goTo(txRxCycleState newState);        // move the state of the TxRxCycle state machine - handles exit old state actions and entry new state actions

    // macHeader MHDR;
    static deviceAddress DevAddr;
    static aesKey applicationKey;
    static aesKey networkKey;
    static frameCount uplinkFrameCount;
    static frameCount downlinkFrameCount;

    // dutyCycle theDutyCycle;
    //  messageIntegrityCode mic;

    static dataRates theDataRates;
    static uint32_t currentDataRateIndex;
    static loRaChannelCollection theChannels;
    static uint32_t rx1Delay;        // in [seconds]

    // transmitPower theTransmitPower{transmitPower::max};
    // static constexpr uint32_t maxRandomDelayBeforeTx{16384U};        // offset in the rawMessage where the header(s) starts
    static constexpr uint32_t maxRandomDelayBeforeTx{1024U};        // test-version

    // #################################################
    // ### Encoding and Decoding of LoRaWAN messages ###
    // #################################################

    static constexpr uint32_t maxLoRaPayloadLength{256};                         // limited to length of databuffer in SX126x
    static constexpr uint32_t b0BlockLength{16};                                 // length of the so-called B0 block, in front of the LoRa payload, needed to calculate MIC
    static constexpr uint32_t macHeaderLength{1};                                // length of MHDR in [bytes]
    static constexpr uint32_t deviceAddressLength{deviceAddress::lengthInBytes};        // length of DevAddr in [bytes]
    static constexpr uint32_t frameControlLength{1};                             // length of FCtrl in [bytes]
    static constexpr uint32_t frameCountLSHLength{2};                            // frameCount Least Significant Halve - truncated to 2 LSbytes only
    static constexpr uint32_t micLength{messageIntegrityCode::length};           // length of MIC in [bytes]

    static uint8_t rawMessage[maxLoRaPayloadLength + b0BlockLength];        // in this buffer, the message is contructed (Tx - Uplink) or decoded (Rx - Downlink)

    static uint32_t macPayloadLength;          //
    static uint32_t framePortLength;           // total length of FPort in [bytes] 1 if present, 0 if not present
    static uint32_t frameOptionsLength;        // this is not static, it can vary between 0 and 15
    static uint32_t frameHeaderLength;         // this is not static, it can vary between 7 and 22
    static uint32_t framePayloadLength;        // length of the application (or MAC layer) payload. excludes header, port and mic
    static uint32_t loRaPayloadLength;         // length of the data in the radio Tx/Rx buffer

    static constexpr uint32_t macHeaderOffset{b0BlockLength};                                                                                              // offset in the rawMessage where the header(s) starts
    static constexpr uint32_t deviceAddressOffset{b0BlockLength + macHeaderLength};                                                                        // offset in the rawMessage where the deviceAddress is found
    static constexpr uint32_t frameControlOffset{b0BlockLength + macHeaderLength + deviceAddressLength};                                                   // offset in the rawMessage where the frameControl is found
    static constexpr uint32_t frameCountOffset{b0BlockLength + macHeaderLength + deviceAddressLength + frameControlLength};                                // offset in the rawMessage where the frameCountLSH is found
    static constexpr uint32_t frameOptionsOffset{b0BlockLength + macHeaderLength + deviceAddressLength + frameControlLength + frameCountLSHLength};        // offset in the rawMessage where the frameOptions are found

    static uint32_t framePortOffset;           //
    static uint32_t framePayloadOffset;        //
    static uint32_t micOffset;                 //

    // static constexpr uint32_t macInOutLength{256};
    static linearBuffer<64> macIn;         // buffer holding the received MAC requests and/or answers
    static linearBuffer<64> macOut;        // buffer holding the MAC requests and/or answers to be sent

    // ################################################################
    // ### Helper functions for constructing an uplink message - Tx ###
    // ################################################################

    static void setOffsetsAndLengthsTx(uint32_t framePayloadLength, uint32_t frameOptionsLength = 0);                                                                      // calculate all offsets and lengths in rawMessage starting from the to be transmitted application payload length
    static void insertPayload(const uint8_t data[], const uint32_t length);                                                                                                // copy application payload to correct position in the rawMessage buffer
    static void encryptPayload(aesKey& theKey);                                                                                                                            // encrypt the payload in the rawMessage buffer
    static void decryptPayload(aesKey& theKey);                                                                                                                            // decrypt the payload in the rawMessage buffer
    static void insertHeaders(const uint8_t theFrameOptions[], const uint32_t theFrameOptionslength, const uint32_t theFramePayloadLength, framePort theFramePort);        // prepend the header to the rawMessage buffer
    static void insertBlockB0(linkDirection theDirection, deviceAddress& anAddress, frameCount& aFrameCounter, uint32_t micPayloadLength);                                 //
    static void insertMic();                                                                                                                                               //

    // #############################################################
    // ### Helper functions for decoding a downlink message - Rx ###
    // #############################################################

    static void setOffsetsAndLengthsRx(uint32_t loRaPayloadLength);                                                                                                                   // calculate all offsets and lengths in rawMessage by decoding the received LoRa payload
    static uint16_t getReceivedFramecount() { return (static_cast<uint16_t>(rawMessage[frameCountOffset]) + (static_cast<uint16_t>(rawMessage[frameCountOffset + 1]) << 8)); }        //
    static bool isValidMic();                                                                                                                                                         //
    static bool isValidDevAddr(deviceAddress testAddress);                                                                                                                            //
    static bool isValidDownlinkFrameCount(frameCount testFrameCount);                                                                                                                 //
    static messageType decodeMessage();                                                                                                                                               //

    static uint32_t getRandomNumber();               //
    static void startTimer(uint32_t timeOut);        // timeOut in [ticks] from the 2048 Hz clock driving LPTIM1
    static void stopTimer();                         //

    static void prepareBlockAi(uint8_t* aBlock, linkDirection theDirection, deviceAddress& anAddress, frameCount& aFrameCounter, uint32_t blockIndex);
    static void prepareBlockAi(aesBlock& theBlock, linkDirection theDirection, deviceAddress& anAddress, frameCount& aFrameCounter, uint32_t blockIndex);
    static uint32_t getReceiveTimeout(spreadingFactor aSpreadingfactor);        //

    static void processMacContents();                                                                                                           // process the contents of the macIn buffer, output goes to macOut buffer
    static void processNewChannelRequest(uint32_t channelIndex, uint32_t frequency, uint32_t minimumDataRate, uint32_t maximumDataRate);        //
    static void processReceiveTimingSetupRequest(uint32_t rx1Delay);

    static void removeNonStickyMacStuff();                                                                         // removes all non-sticky MAC stuff from macOut after transmitting them. sticky MAC stuff is kept until a downlink is received
    static uint32_t calculateMaxTransmitTimeout(uint32_t currentDataRateIndex, uint32_t loRaPayloadLength);        // TODO : provide an upper limit for the timeout
};
