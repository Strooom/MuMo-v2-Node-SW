// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include <cstring>
#include "nvs.h"
#include "sx126x.h"
#include "eventbuffer.h"
#include "applicationevent.h"
#include "lorawan.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

// These objects are needed for the LoRaWAN class to compile but they are not used in this tests
nonVolatileStorage nvs;
eventBuffer<loRaWanEvent, 16U> loraWanEventBuffer;
eventBuffer<applicationEvent, 16U> applicationEventBuffer;
sx126x theRadio;

// Test Messages
// 1. unconfirmed uplink, no frameOptions
constexpr uint32_t testMsg0Length      = 17;
const uint8_t testMsg0[testMsg0Length] = {
    0x40,                          // macHeader : uplink unconfirmed
    0x04, 0x03, 0x02, 0x01,        // deviceAddress = 0x01020304
    0x00,                          // frameControl, fOptsLen = 0
    0x12, 0x11,                    // frameCount = 0x1112
    0x16,                          // framePort = 0x16
    0x31, 0x32, 0x33, 0x34,        // framePayload, 4 bytes
    0xB8, 0x59, 0x92, 0x49         // mic
};

// 2. unconfirmed uplink, with 1 byte frameOptions
constexpr uint32_t testMsg1Length      = 18;
const uint8_t testMsg1[testMsg1Length] = {
    0x40,                          // macHeader : uplink unconfirmed
    0x04, 0x03, 0x02, 0x01,        // deviceAddress = 0x01020304
    0x01,                          // frameControl, fOptsLen = 1
    0x12, 0x11,                    // frameCount = 0x1112
    0x20,                          // frameOptions : 0x20
    0x16,                          // framePort = 0x16
    0x31, 0x32, 0x33, 0x34,        // framePayload : 4 bytes
    0xB8, 0x59, 0x92, 0x49         // mic
};

// 2. unconfirmed uplink, with only 1 byte frameOptions, no framePayload
constexpr uint32_t testMsg2Length      = 13;
const uint8_t testMsg2[testMsg2Length] = {
    0x40,                          // macHeader : uplink unconfirmed
    0x04, 0x03, 0x02, 0x01,        // deviceAddress = 0x01020304
    0x01,                          // frameControl, fOptsLen = 1
    0x12, 0x11,                    // frameCount = 0x1112
    0x20,                          // frameOptions : 0x20
    0xB8, 0x59, 0x92, 0x49         // mic
};

// constexpr uint32_t testVectorLength        = 23;
// const uint8_t testVector[testVectorLength] = {

//     0x40,                                                              // macHeader : uplink unconfirmed
//     0x92, 0x3B, 0x0B, 0x26,                                            // deviceAddress = 0x260B3B92
//     0x00,                                                              // frameControl
//     0x39, 0x07,                                                        // FrameCount = 0x0739 = 1849
//     0x10,                                                              // framePort = 16
//     0x49, 0xE8, 0xFA, 0x19, 0xED, 0x8F, 0xE2, 0x5C, 0x48, 0xB1,        // framePayload, 10 bytes, encrypted
//     0xB8, 0x59, 0x92, 0x49                                             // mic
// };

// constexpr uint32_t testVector2Length         = 27;
// const uint8_t testVector2[testVector2Length] = {
//     0x40,                                                              // macHeader : uplink unconfirmed
//     0x92, 0x3B, 0x0B, 0x26,                                            // deviceAddress = 0x260B3B92
//     0x04,                                                              // frameControl with frameOptionsLength = 4
//     0x39, 0x07,                                                        // FrameCount = 0x0739 = 1849
//     0x01, 0x02, 0x03, 0x04,                                            // frameOptions data
//     0x10,                                                              // framePort = 16
//     0x49, 0xE8, 0xFA, 0x19, 0xED, 0x8F, 0xE2, 0x5C, 0x48, 0xB1,        // framePayload, 10 bytes, encrypted
//     0xB8, 0x59, 0x92, 0x49                                             // mic
// };


void test_offsetsTx0() {        // 1. case with no frameOptions

    LoRaWAN theNetwork;
    memcpy(theNetwork.rawMessage + theNetwork.b0BlockLength, testMsg0, testMsg0Length);        // Write the testMsg into rawMessage

    theNetwork.setOffsetsAndLengthsTx(4U);                                                     // calculate offsets and lengths for payloadLength = 4 and no frameOptions
    // Check all calculated lengths
    TEST_ASSERT_EQUAL_UINT32(4U, theNetwork.framePayloadLength);                         //
    TEST_ASSERT_EQUAL_UINT32(0U, theNetwork.frameOptionsLength);                         //
    TEST_ASSERT_EQUAL_UINT32(7U, theNetwork.frameHeaderLength);                          //
    TEST_ASSERT_EQUAL_UINT32(1U, theNetwork.framePortLength);                            //
    TEST_ASSERT_EQUAL_UINT32((testMsg0Length - 5U), theNetwork.macPayloadLength);        //
    TEST_ASSERT_EQUAL_UINT32(testMsg0Length, theNetwork.loRaPayloadLength);              //

    // Check all calculated offsets
    TEST_ASSERT_EQUAL_UINT32(theNetwork.b0BlockLength + 8, theNetwork.framePortOffset);           //
    TEST_ASSERT_EQUAL_UINT32(theNetwork.b0BlockLength + 9, theNetwork.framePayloadOffset);        //
    TEST_ASSERT_EQUAL_UINT32(theNetwork.b0BlockLength + 13, theNetwork.micOffset);                // 9 for all headers, 4 for payload

    // Now let's double check and test some of the contents at the offsets
    TEST_ASSERT_EQUAL_UINT32(0x40, theNetwork.rawMessage[theNetwork.macHeaderOffset]);            // macHeader
    TEST_ASSERT_EQUAL_UINT32(0x04, theNetwork.rawMessage[theNetwork.deviceAddressOffset]);        // first byte (LSB) of DevAddr
    TEST_ASSERT_EQUAL_UINT32(0, theNetwork.rawMessage[theNetwork.frameControlOffset]);            // frameControl
    TEST_ASSERT_EQUAL_UINT32(0x12, theNetwork.rawMessage[theNetwork.frameCountOffset]);           // first byte (LSB) of frameCount
    TEST_ASSERT_EQUAL_UINT32(0xB8, theNetwork.rawMessage[theNetwork.micOffset]);                  // first byte of MIC
}

void test_offsetsTx1() {        // 1. case with no frameOptions

    LoRaWAN theNetwork;
    memcpy(theNetwork.rawMessage + theNetwork.b0BlockLength, testMsg1, testMsg1Length);        // Write the testMsg into rawMessage

    theNetwork.setOffsetsAndLengthsTx(4U, 1U);                                                 // calculate offsets and lengths for payloadLength = 4 and frameOptionsLength = 1
    // Check all calculated lengths
    TEST_ASSERT_EQUAL_UINT32(4U, theNetwork.framePayloadLength);                         //
    TEST_ASSERT_EQUAL_UINT32(1U, theNetwork.frameOptionsLength);                         //
    TEST_ASSERT_EQUAL_UINT32(8U, theNetwork.frameHeaderLength);                          //
    TEST_ASSERT_EQUAL_UINT32(1U, theNetwork.framePortLength);                            //
    TEST_ASSERT_EQUAL_UINT32((testMsg1Length - 5U), theNetwork.macPayloadLength);        //
    TEST_ASSERT_EQUAL_UINT32(testMsg1Length, theNetwork.loRaPayloadLength);              //

    // Check all calculated offsets
    TEST_ASSERT_EQUAL_UINT32(theNetwork.b0BlockLength + 9, theNetwork.framePortOffset);            //
    TEST_ASSERT_EQUAL_UINT32(theNetwork.b0BlockLength + 10, theNetwork.framePayloadOffset);        //
    TEST_ASSERT_EQUAL_UINT32(theNetwork.b0BlockLength + 14, theNetwork.micOffset);                 // 10 for all headers, 4 for payload

    // Now let's double check and test some of the contents at the offsets
    TEST_ASSERT_EQUAL_UINT32(0x40, theNetwork.rawMessage[theNetwork.macHeaderOffset]);            // macHeader
    TEST_ASSERT_EQUAL_UINT32(0x04, theNetwork.rawMessage[theNetwork.deviceAddressOffset]);        // first byte (LSB) of DevAddr
    TEST_ASSERT_EQUAL_UINT32(0x01, theNetwork.rawMessage[theNetwork.frameControlOffset]);         // frameControl
    TEST_ASSERT_EQUAL_UINT32(0x12, theNetwork.rawMessage[theNetwork.frameCountOffset]);           // first byte (LSB) of frameCount
    TEST_ASSERT_EQUAL_UINT32(0xB8, theNetwork.rawMessage[theNetwork.micOffset]);                  // first byte of MIC
}

void test_offsetsTx2() {        // 1. case with no frameOptions

    LoRaWAN theNetwork;
    memcpy(theNetwork.rawMessage + theNetwork.b0BlockLength, testMsg2, testMsg2Length);        // Write the testMsg into rawMessage

    theNetwork.setOffsetsAndLengthsTx(0, 1U);                                                  // calculate offsets and lengths for payloadLength = 0 and frameOptionsLength = 1
    // Check all calculated lengths
    TEST_ASSERT_EQUAL_UINT32(0, theNetwork.framePayloadLength);                          //
    TEST_ASSERT_EQUAL_UINT32(1U, theNetwork.frameOptionsLength);                         //
    TEST_ASSERT_EQUAL_UINT32(8U, theNetwork.frameHeaderLength);                          //
    TEST_ASSERT_EQUAL_UINT32(0, theNetwork.framePortLength);                             //
    TEST_ASSERT_EQUAL_UINT32((testMsg2Length - 5U), theNetwork.macPayloadLength);        //
    TEST_ASSERT_EQUAL_UINT32(testMsg2Length, theNetwork.loRaPayloadLength);              //

    // Check all calculated offsets
    TEST_ASSERT_EQUAL_UINT32(theNetwork.b0BlockLength + 9, theNetwork.micOffset);        // 9 for all headers, no payload

    // Now let's double check and test some of the contents at the offsets
    TEST_ASSERT_EQUAL_UINT32(0x40, theNetwork.rawMessage[theNetwork.macHeaderOffset]);            // macHeader
    TEST_ASSERT_EQUAL_UINT32(0x04, theNetwork.rawMessage[theNetwork.deviceAddressOffset]);        // first byte (LSB) of DevAddr
    TEST_ASSERT_EQUAL_UINT32(0x01, theNetwork.rawMessage[theNetwork.frameControlOffset]);         // frameControl
    TEST_ASSERT_EQUAL_UINT32(0x12, theNetwork.rawMessage[theNetwork.frameCountOffset]);           // first byte (LSB) of frameCount
    TEST_ASSERT_EQUAL_UINT32(0xB8, theNetwork.rawMessage[theNetwork.micOffset]);                  // first byte of MIC
}

void test_insertFramePayload0() {
    LoRaWAN theNetwork;
    constexpr uint32_t testPayloadBytesLength{10U};
    const uint8_t testPayloadBytes[testPayloadBytesLength] = {0x02, 0x01, 0xA8, 0xF4, 0x48, 0x64, 0xA4, 0x70, 0x55, 0x40};
    theNetwork.setOffsetsAndLengthsTx(testPayloadBytesLength);        // no frameOptions
    theNetwork.insertPayload(testPayloadBytes, testPayloadBytesLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testPayloadBytes, theNetwork.rawMessage + theNetwork.framePayloadOffset, testPayloadBytesLength);
}

void test_insertFramePayload1() {
    LoRaWAN theNetwork;
    constexpr uint32_t testPayloadBytesLength{10U};
    const uint8_t testPayloadBytes[testPayloadBytesLength] = {0x02, 0x01, 0xA8, 0xF4, 0x48, 0x64, 0xA4, 0x70, 0x55, 0x40};
    theNetwork.setOffsetsAndLengthsTx(testPayloadBytesLength, 1U);        // with 1 byte of frameOptions
    theNetwork.insertPayload(testPayloadBytes, testPayloadBytesLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testPayloadBytes, theNetwork.rawMessage + theNetwork.framePayloadOffset, testPayloadBytesLength);
}

void test_encryptFramePayload() {
    LoRaWAN theNetwork;
    theNetwork.DevAddr.set(0x260B3B92);
    theNetwork.uplinkFrameCount.set(1850);
    theNetwork.applicationKey.setFromASCII("398F459FE521152FD5B014EA44428AC2");
    constexpr uint32_t testPayloadBytesLength{10U};
    const uint8_t testPayloadBytesBeforeEncryption[testPayloadBytesLength] = {0x02, 0x01, 0xA8, 0xF4, 0x48, 0x64, 0xA4, 0x70, 0x55, 0x40};
    const uint8_t testPayloadBytesAfterEncryption[testPayloadBytesLength]  = {0x61, 0x4A, 0xE1, 0x01, 0x75, 0xCC, 0x1B, 0x5A, 0x77, 0xFF};

    theNetwork.setOffsetsAndLengthsTx(testPayloadBytesLength);
    theNetwork.insertPayload(testPayloadBytesBeforeEncryption, testPayloadBytesLength);
    theNetwork.encryptDecryptPayload(theNetwork.applicationKey, linkDirection::uplink);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testPayloadBytesAfterEncryption, theNetwork.rawMessage + theNetwork.framePayloadOffset, testPayloadBytesLength);
}

void test_insertHeaders0() {
    constexpr uint32_t testPort = 16;
    const uint8_t testHeader[]  = {0x40, 0x92, 0x3B, 0x0B, 0x26, 0x00, 0x3A, 0x07, 0x10};
    LoRaWAN theNetwork;
    theNetwork.DevAddr.set(0x260B3B92);
    theNetwork.uplinkFrameCount.set(1850);
    theNetwork.insertHeaders(nullptr, 0, 1, testPort);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testHeader, theNetwork.rawMessage + theNetwork.macHeaderOffset, theNetwork.frameHeaderLength + theNetwork.macHeaderLength);
}

void test_insertHeaders1() {
    constexpr uint32_t testPort      = 16;
    const uint8_t testHeader[]       = {0x40, 0x92, 0x3B, 0x0B, 0x26, 0x01, 0x3A, 0x07, 0x20, 0x10};
    const uint8_t testFrameOptions[] = {0x20};
    LoRaWAN theNetwork;
    theNetwork.DevAddr.set(0x260B3B92);
    theNetwork.uplinkFrameCount.set(1850);
    theNetwork.insertHeaders(testFrameOptions, 1, 1, testPort);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testHeader, theNetwork.rawMessage + theNetwork.macHeaderOffset, theNetwork.frameHeaderLength + theNetwork.macHeaderLength);
}

void test_insertHeaders2() {
    const uint8_t testHeader[]       = {0x40, 0x92, 0x3B, 0x0B, 0x26, 0x01, 0x3A, 0x07, 0x20};
    const uint8_t testFrameOptions[] = {0x20};
    LoRaWAN theNetwork;
    theNetwork.DevAddr.set(0x260B3B92);
    theNetwork.uplinkFrameCount.set(1850);
    theNetwork.insertHeaders(testFrameOptions, 1, 0, 0);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testHeader, theNetwork.rawMessage + theNetwork.macHeaderOffset, theNetwork.frameHeaderLength + theNetwork.macHeaderLength);
}

void test_insertHeaders3() {
    const uint8_t testHeader[] = {0x40, 0x92, 0x3B, 0x0B, 0x26, 0x00, 0x3A, 0x07};
    LoRaWAN theNetwork;
    theNetwork.DevAddr.set(0x260B3B92);
    theNetwork.uplinkFrameCount.set(1850);
    theNetwork.insertHeaders(nullptr, 0, 0, 0);        // no frameOptions, no framePayload (so no framePort) - minimal packet with ACK etc..
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testHeader, theNetwork.rawMessage + theNetwork.macHeaderOffset, theNetwork.frameHeaderLength + theNetwork.macHeaderLength);
}

void test_insertBlockB0() {
    const uint8_t testBlockB0[16] = {0x49, 0x00, 0x00, 0x00, 0x00, 0x00, 0x92, 0x3B, 0x0B, 0x26, 0x3A, 0x07, 0x00, 0x00, 0x00, 0x13};
    LoRaWAN theNetwork;
    theNetwork.setOffsetsAndLengthsTx(10U);
    deviceAddress testAddress;
    testAddress.set(0x260B3B92);
    frameCount testFrameCount;
    testFrameCount.set(1850);
    theNetwork.insertBlockB0(linkDirection::uplink, testAddress, testFrameCount, (theNetwork.macHeaderLength + theNetwork.macPayloadLength));

    TEST_ASSERT_EQUAL_UINT8_ARRAY(testBlockB0, theNetwork.rawMessage, 16);
}

void test_insertMic() {
    LoRaWAN theNetwork;
    theNetwork.DevAddr.set(0x260B3B92);
    theNetwork.uplinkFrameCount.set(1850);
    theNetwork.applicationKey.setFromASCII("398F459FE521152FD5B014EA44428AC2");
    theNetwork.networkKey.setFromASCII("680AB79064FD273E52FBBF4FC6349B13");
    constexpr uint32_t testPayloadBytesLength{10U};
    const uint8_t testPayloadBytesBeforeEncryption[testPayloadBytesLength] = {0x02, 0x01, 0xA8, 0xF4, 0x48, 0x64, 0xA4, 0x70, 0x55, 0x40};
    const uint8_t testMic[4]                                               = {0xB5, 0x51, 0xEF, 0xDF};

    theNetwork.setOffsetsAndLengthsTx(testPayloadBytesLength);
    theNetwork.insertPayload(testPayloadBytesBeforeEncryption, testPayloadBytesLength);
    theNetwork.encryptDecryptPayload(theNetwork.applicationKey, linkDirection::uplink);
    theNetwork.insertHeaders(nullptr, 0, testPayloadBytesLength, 16U);
    theNetwork.insertBlockB0(linkDirection::uplink, theNetwork.DevAddr, theNetwork.uplinkFrameCount, (theNetwork.macHeaderLength + theNetwork.macPayloadLength));
    theNetwork.insertMic();

    for (uint32_t index = 0; index < 4; index++) {
        TEST_ASSERT_EQUAL_UINT32(testMic[index], theNetwork.rawMessage[theNetwork.micOffset + index]);
    }
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_offsetsTx0);
    RUN_TEST(test_offsetsTx1);
    RUN_TEST(test_offsetsTx2);
    RUN_TEST(test_insertFramePayload0);
    RUN_TEST(test_insertFramePayload1);
    RUN_TEST(test_encryptFramePayload);
    RUN_TEST(test_insertHeaders0);
    RUN_TEST(test_insertHeaders1);
    RUN_TEST(test_insertHeaders2);
    RUN_TEST(test_insertHeaders3);
    RUN_TEST(test_insertBlockB0);
    RUN_TEST(test_insertMic);
    UNITY_END();
}

// rawMessage BE =
// 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
// 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
// 0x02 0x01 0xA8 0xF4 0x48 0x64 0xA4 0x70 0x55 0x40
// 0x00 0x00 0x00 0x00

// rawMessage AE =
// 0x49 0x00 0x00 0x00 0x00 0x00 0x92 0x3B 0x0B 0x26 0x3A 0x07 0x00 0x00 0x00 0x13
// 0x40 0x92 0x3B 0x0B 0x26 0x00 0x3A 0x07 0x10
// 0x61 0x4A 0xE1 0x01 0x75 0xCC 0x1B 0x5A 0x77 0xFF
// 0xB5 0x51 0xEF 0xDF
