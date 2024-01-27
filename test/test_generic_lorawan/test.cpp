#include <unity.h>
#include <lorawan.hpp>
#include <circularbuffer.hpp>
#include <lorawanevent.hpp>
#include <applicationevent.hpp>
#include <settingscollection.hpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;
circularBuffer<loRaWanEvent, 16U> loraWanEventBuffer;

uint8_t mockSX126xDataBuffer[256];            // unitTesting mock for the LoRa Rx/Tx buffer, inside the SX126x
uint8_t mockSX126xRegisters[0x1000];          // unitTesting mock for the config registers, inside the SX126x
uint8_t mockSX126xCommandData[256][8];        // unitTesting mock for capturing the commands and their parameters to configure the SX126x

void setUp(void) {}
void tearDown(void) {}

void test_initialize() {
    TEST_ASSERT_EQUAL(0, LoRaWAN::uplinkFrameCount.asUint32);
    TEST_ASSERT_EQUAL(0, LoRaWAN::downlinkFrameCount.asUint32);
    TEST_ASSERT_EQUAL(1, LoRaWAN::rx1Delay);
    TEST_ASSERT_EQUAL(0, LoRaWAN::DevAddr.asUint32);

    // Populate non-volatile memory with some test data
    settingsCollection::save(settingsCollection::settingIndex::uplinkFrameCounter, 0x1234);
    settingsCollection::save(settingsCollection::settingIndex::downlinkFrameCounter, 0x5678);
    settingsCollection::save(settingsCollection::settingIndex::rx1Delay, 0x05);
    settingsCollection::save(settingsCollection::settingIndex::DevAddr, 0xABCD);

    LoRaWAN::initialize();

    // Check if all context is restored correctly
    TEST_ASSERT_EQUAL(0x1234, LoRaWAN::uplinkFrameCount.asUint32);
    TEST_ASSERT_EQUAL(0x5678, LoRaWAN::downlinkFrameCount.asUint32);
    TEST_ASSERT_EQUAL(0x5, LoRaWAN::rx1Delay);
    TEST_ASSERT_EQUAL(0xABCD, LoRaWAN::DevAddr.asUint32);
    // TODO : complete for all context
}

void test_constants() {
    TEST_ASSERT_EQUAL(16, LoRaWAN::b0BlockLength);
    TEST_ASSERT_EQUAL(1, LoRaWAN::macHeaderLength);
    TEST_ASSERT_EQUAL(4, LoRaWAN::deviceAddressLength);
    TEST_ASSERT_EQUAL(1, LoRaWAN::frameControlLength);
    TEST_ASSERT_EQUAL(2, LoRaWAN::frameCountLSHLength);
    TEST_ASSERT_EQUAL(4, LoRaWAN::micLength);

    TEST_ASSERT_EQUAL(16, LoRaWAN::macHeaderOffset);
    TEST_ASSERT_EQUAL(17, LoRaWAN::deviceAddressOffset);
    TEST_ASSERT_EQUAL(21, LoRaWAN::frameControlOffset);
    TEST_ASSERT_EQUAL(22, LoRaWAN::frameCountOffset);
    TEST_ASSERT_EQUAL(24, LoRaWAN::frameOptionsOffset);
}

void test_setOffsetsAndLengthsTx1() {
    TEST_ASSERT_EQUAL(0, LoRaWAN::macPayloadLength);
    TEST_ASSERT_EQUAL(1, LoRaWAN::framePortLength);
    TEST_ASSERT_EQUAL(0, LoRaWAN::frameOptionsLength);
    TEST_ASSERT_EQUAL(0, LoRaWAN::frameHeaderLength);
    TEST_ASSERT_EQUAL(0, LoRaWAN::framePayloadLength);
    TEST_ASSERT_EQUAL(0, LoRaWAN::loRaPayloadLength);

    uint32_t testFramePayloadLength{0};
    uint32_t testFrameOptionsLength{0};
    LoRaWAN::setOffsetsAndLengthsTx(testFramePayloadLength, testFrameOptionsLength);
    TEST_ASSERT_EQUAL(7, LoRaWAN::macPayloadLength);
    TEST_ASSERT_EQUAL(0, LoRaWAN::framePortLength);
    TEST_ASSERT_EQUAL(12, LoRaWAN::loRaPayloadLength);
    TEST_ASSERT_EQUAL(24, LoRaWAN::micOffset);
}

void test_setOffsetsAndLengthsTx2() {
    uint32_t testFramePayloadLength{32};
    uint32_t testFrameOptionsLength{0};
    LoRaWAN::setOffsetsAndLengthsTx(testFramePayloadLength, testFrameOptionsLength);
    TEST_ASSERT_EQUAL(40, LoRaWAN::macPayloadLength);
    TEST_ASSERT_EQUAL(1, LoRaWAN::framePortLength);
    TEST_ASSERT_EQUAL(45, LoRaWAN::loRaPayloadLength);
    TEST_ASSERT_EQUAL(57, LoRaWAN::micOffset);
}

void test_setOffsetsAndLengthsTx3() {
    uint32_t testFramePayloadLength{28};
    uint32_t testFrameOptionsLength{4};
    LoRaWAN::setOffsetsAndLengthsTx(testFramePayloadLength, testFrameOptionsLength);
    TEST_ASSERT_EQUAL(40, LoRaWAN::macPayloadLength);
    TEST_ASSERT_EQUAL(1, LoRaWAN::framePortLength);
    TEST_ASSERT_EQUAL(45, LoRaWAN::loRaPayloadLength);
    TEST_ASSERT_EQUAL(57, LoRaWAN::micOffset);
}

void test_isValidDevAddr() {
    LoRaWAN::DevAddr.asUint32 = 0x1234;
    deviceAddress testAddress1{0x1234};
    deviceAddress testAddress2{0x5678};
    TEST_ASSERT_TRUE(LoRaWAN::isValidDevAddr(testAddress1));
    TEST_ASSERT_FALSE(LoRaWAN::isValidDevAddr(testAddress2));
}

void test_setOffsetsAndLengthsRx1() {
    static constexpr uint32_t loRaPayloadLength{12};
    uint8_t receivedBytes[loRaPayloadLength] = {0b01100000, 0x78, 0x56, 0x34, 0x12, 0b00000000, 0x00, 0x00, 0xaa, 0xbb, 0xcc, 0xdd};        // minimal msg with no payload and no options
    memcpy(LoRaWAN::rawMessage + LoRaWAN::b0BlockLength, receivedBytes, loRaPayloadLength);
    LoRaWAN::setOffsetsAndLengthsRx(loRaPayloadLength);
    TEST_ASSERT_EQUAL(7, LoRaWAN::macPayloadLength);
    TEST_ASSERT_EQUAL(0, LoRaWAN::framePortLength);
    TEST_ASSERT_EQUAL(0, LoRaWAN::frameOptionsLength);

    TEST_ASSERT_EQUAL(7, LoRaWAN::frameHeaderLength);
    TEST_ASSERT_EQUAL(0, LoRaWAN::framePayloadLength);
    TEST_ASSERT_EQUAL(12, LoRaWAN::loRaPayloadLength);
    TEST_ASSERT_EQUAL(24, LoRaWAN::micOffset);
}

void test_setOffsetsAndLengthsRx2() {
    static constexpr uint32_t loRaPayloadLength{21};
    uint8_t receivedBytes[loRaPayloadLength] = {0b01100000, 0x78, 0x56, 0x34, 0x12, 0b00000100, 0x00, 0x00, 0x88, 0x77, 0x66, 0x55, 0x01, 0x10, 0x20, 0x30, 0x40, 0xaa, 0xbb, 0xcc, 0xdd};        // msg with payload and no frameoptions
    memcpy(LoRaWAN::rawMessage + LoRaWAN::b0BlockLength, receivedBytes, loRaPayloadLength);
    LoRaWAN::setOffsetsAndLengthsRx(loRaPayloadLength);
    TEST_ASSERT_EQUAL(16, LoRaWAN::macPayloadLength);
    TEST_ASSERT_EQUAL(1, LoRaWAN::framePortLength);
    TEST_ASSERT_EQUAL(4, LoRaWAN::frameOptionsLength);

    TEST_ASSERT_EQUAL(11, LoRaWAN::frameHeaderLength);
    TEST_ASSERT_EQUAL(4, LoRaWAN::framePayloadLength);
    TEST_ASSERT_EQUAL(33, LoRaWAN::micOffset);
}

void test_prepareBlockAiTx() {
    aesBlock testBlock;
    LoRaWAN::DevAddr.asUint32          = 0x12345678;
    LoRaWAN::uplinkFrameCount.asUint32 = 0xFFEEDDCC;
    uint32_t testBlockIndex{7};
    LoRaWAN::prepareBlockAi(testBlock, linkDirection::uplink, testBlockIndex);

    TEST_ASSERT_EQUAL_UINT8(0x01, testBlock[0]);
    TEST_ASSERT_EQUAL_UINT8(0x00, testBlock[1]);
    TEST_ASSERT_EQUAL_UINT8(0x00, testBlock[2]);
    TEST_ASSERT_EQUAL_UINT8(0x00, testBlock[3]);
    TEST_ASSERT_EQUAL_UINT8(0x00, testBlock[4]);
    TEST_ASSERT_EQUAL_UINT8(0x00, testBlock[5]);
    TEST_ASSERT_EQUAL_UINT8(0x78, testBlock[6]);
    TEST_ASSERT_EQUAL_UINT8(0x56, testBlock[7]);
    TEST_ASSERT_EQUAL_UINT8(0x34, testBlock[8]);
    TEST_ASSERT_EQUAL_UINT8(0x12, testBlock[9]);
    TEST_ASSERT_EQUAL_UINT8(0xCC, testBlock[10]);
    TEST_ASSERT_EQUAL_UINT8(0xDD, testBlock[11]);
    TEST_ASSERT_EQUAL_UINT8(0xEE, testBlock[12]);
    TEST_ASSERT_EQUAL_UINT8(0xFF, testBlock[13]);
    TEST_ASSERT_EQUAL_UINT8(0x00, testBlock[14]);
    TEST_ASSERT_EQUAL_UINT8(0x07, testBlock[15]);
}

void test_prepareBlockAiRx() {
    aesBlock testBlock;
    LoRaWAN::DevAddr.asUint32            = 0x12345678;
    LoRaWAN::downlinkFrameCount.asUint32 = 0xFFEEDDCC;
    uint32_t testBlockIndex{3};
    LoRaWAN::prepareBlockAi(testBlock, linkDirection::downlink, testBlockIndex);

    TEST_ASSERT_EQUAL_UINT8(0x01, testBlock[0]);
    TEST_ASSERT_EQUAL_UINT8(0x00, testBlock[1]);
    TEST_ASSERT_EQUAL_UINT8(0x00, testBlock[2]);
    TEST_ASSERT_EQUAL_UINT8(0x00, testBlock[3]);
    TEST_ASSERT_EQUAL_UINT8(0x00, testBlock[4]);
    TEST_ASSERT_EQUAL_UINT8(0x01, testBlock[5]);
    TEST_ASSERT_EQUAL_UINT8(0x78, testBlock[6]);
    TEST_ASSERT_EQUAL_UINT8(0x56, testBlock[7]);
    TEST_ASSERT_EQUAL_UINT8(0x34, testBlock[8]);
    TEST_ASSERT_EQUAL_UINT8(0x12, testBlock[9]);
    TEST_ASSERT_EQUAL_UINT8(0xCC, testBlock[10]);
    TEST_ASSERT_EQUAL_UINT8(0xDD, testBlock[11]);
    TEST_ASSERT_EQUAL_UINT8(0xEE, testBlock[12]);
    TEST_ASSERT_EQUAL_UINT8(0xFF, testBlock[13]);
    TEST_ASSERT_EQUAL_UINT8(0x00, testBlock[14]);
    TEST_ASSERT_EQUAL_UINT8(0x03, testBlock[15]);
}

void test_isValidDownlinkFrameCount() {
    LoRaWAN::downlinkFrameCount.asUint32 = 0;
    frameCount testFrameCount(10);
    TEST_ASSERT_TRUE(LoRaWAN::isValidDownlinkFrameCount(testFrameCount));
    LoRaWAN::downlinkFrameCount.asUint32 = 9;
    TEST_ASSERT_TRUE(LoRaWAN::isValidDownlinkFrameCount(testFrameCount));
    LoRaWAN::downlinkFrameCount.asUint32 = 10;
    TEST_ASSERT_FALSE(LoRaWAN::isValidDownlinkFrameCount(testFrameCount));
    LoRaWAN::downlinkFrameCount.asUint32 = 11;
    TEST_ASSERT_FALSE(LoRaWAN::isValidDownlinkFrameCount(testFrameCount));
}

void test_insertHeaders1() {
    LoRaWAN::DevAddr.asUint32          = 0x12345678;
    LoRaWAN::uplinkFrameCount.asUint32 = 0xFFEEDDCC;
    LoRaWAN::setOffsetsAndLengthsTx(0, 0);
    LoRaWAN::insertHeaders(nullptr, 0, 0, 0);
    TEST_ASSERT_EQUAL_UINT8(0b01000000, LoRaWAN::rawMessage[16]);
    TEST_ASSERT_EQUAL_UINT8(0x78, LoRaWAN::rawMessage[17]);
    TEST_ASSERT_EQUAL_UINT8(0x56, LoRaWAN::rawMessage[18]);
    TEST_ASSERT_EQUAL_UINT8(0x34, LoRaWAN::rawMessage[19]);
    TEST_ASSERT_EQUAL_UINT8(0x12, LoRaWAN::rawMessage[20]);
    TEST_ASSERT_EQUAL_UINT8(0x00, LoRaWAN::rawMessage[21]);
    TEST_ASSERT_EQUAL_UINT8(0xCC, LoRaWAN::rawMessage[22]);
    TEST_ASSERT_EQUAL_UINT8(0xDD, LoRaWAN::rawMessage[23]);
}

void test_insertHeaders2() {
    LoRaWAN::DevAddr.asUint32          = 0x12345678;
    LoRaWAN::uplinkFrameCount.asUint32 = 0xFFEEDDCC;
    static constexpr uint32_t testFrameOptionsLength{4};
    LoRaWAN::setOffsetsAndLengthsTx(0, testFrameOptionsLength);
    uint8_t testFrameOptions[testFrameOptionsLength] = {0x11, 0x22, 0x33, 0x44};
    LoRaWAN::insertHeaders(testFrameOptions, testFrameOptionsLength, 0, 0);
    TEST_ASSERT_EQUAL_UINT8(0b01000000, LoRaWAN::rawMessage[16]);
    TEST_ASSERT_EQUAL_UINT8(0x78, LoRaWAN::rawMessage[17]);
    TEST_ASSERT_EQUAL_UINT8(0x56, LoRaWAN::rawMessage[18]);
    TEST_ASSERT_EQUAL_UINT8(0x34, LoRaWAN::rawMessage[19]);
    TEST_ASSERT_EQUAL_UINT8(0x12, LoRaWAN::rawMessage[20]);
    TEST_ASSERT_EQUAL_UINT8(0x04, LoRaWAN::rawMessage[21]);
    TEST_ASSERT_EQUAL_UINT8(0xCC, LoRaWAN::rawMessage[22]);
    TEST_ASSERT_EQUAL_UINT8(0xDD, LoRaWAN::rawMessage[23]);
    TEST_ASSERT_EQUAL_UINT8(0x11, LoRaWAN::rawMessage[24]);
    TEST_ASSERT_EQUAL_UINT8(0x22, LoRaWAN::rawMessage[25]);
    TEST_ASSERT_EQUAL_UINT8(0x33, LoRaWAN::rawMessage[26]);
    TEST_ASSERT_EQUAL_UINT8(0x44, LoRaWAN::rawMessage[27]);
}

void test_insertHeaders3() {
    LoRaWAN::DevAddr.asUint32          = 0x12345678;
    LoRaWAN::uplinkFrameCount.asUint32 = 0xFFEEDDCC;
    static constexpr uint32_t testFrameOptionsLength{4};
    LoRaWAN::setOffsetsAndLengthsTx(0, testFrameOptionsLength);
    uint8_t testFrameOptions[testFrameOptionsLength] = {0x11, 0x22, 0x33, 0x44};
    LoRaWAN::insertHeaders(testFrameOptions, testFrameOptionsLength, 1, 8);
    TEST_ASSERT_EQUAL_UINT8(0b01000000, LoRaWAN::rawMessage[16]);
    TEST_ASSERT_EQUAL_UINT8(0x78, LoRaWAN::rawMessage[17]);
    TEST_ASSERT_EQUAL_UINT8(0x56, LoRaWAN::rawMessage[18]);
    TEST_ASSERT_EQUAL_UINT8(0x34, LoRaWAN::rawMessage[19]);
    TEST_ASSERT_EQUAL_UINT8(0x12, LoRaWAN::rawMessage[20]);
    TEST_ASSERT_EQUAL_UINT8(0x04, LoRaWAN::rawMessage[21]);
    TEST_ASSERT_EQUAL_UINT8(0xCC, LoRaWAN::rawMessage[22]);
    TEST_ASSERT_EQUAL_UINT8(0xDD, LoRaWAN::rawMessage[23]);
    TEST_ASSERT_EQUAL_UINT8(0x11, LoRaWAN::rawMessage[24]);
    TEST_ASSERT_EQUAL_UINT8(0x22, LoRaWAN::rawMessage[25]);
    TEST_ASSERT_EQUAL_UINT8(0x33, LoRaWAN::rawMessage[26]);
    TEST_ASSERT_EQUAL_UINT8(0x44, LoRaWAN::rawMessage[27]);
    TEST_ASSERT_EQUAL_UINT8(0x08, LoRaWAN::rawMessage[28]);
}

void test_insertPayload() {
    static constexpr uint32_t clearTextPayloadLength{16};
    LoRaWAN::setOffsetsAndLengthsTx(clearTextPayloadLength, 0);
    uint8_t clearTextPayload[clearTextPayloadLength]{0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96, 0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A};        // common NIST test vector plaintext 6bc1bee22e409f96e93d7e117393172a
    LoRaWAN::insertPayload(clearTextPayload, clearTextPayloadLength);
    TEST_ASSERT_EQUAL_UINT8(0x6B, LoRaWAN::rawMessage[25]);
    TEST_ASSERT_EQUAL_UINT8(0xC1, LoRaWAN::rawMessage[26]);
    TEST_ASSERT_EQUAL_UINT8(0xBE, LoRaWAN::rawMessage[27]);
    TEST_ASSERT_EQUAL_UINT8(0xE2, LoRaWAN::rawMessage[28]);
    TEST_ASSERT_EQUAL_UINT8(0x2E, LoRaWAN::rawMessage[29]);
    TEST_ASSERT_EQUAL_UINT8(0x40, LoRaWAN::rawMessage[30]);
    TEST_ASSERT_EQUAL_UINT8(0x9F, LoRaWAN::rawMessage[31]);
    TEST_ASSERT_EQUAL_UINT8(0x96, LoRaWAN::rawMessage[32]);
    TEST_ASSERT_EQUAL_UINT8(0xE9, LoRaWAN::rawMessage[33]);
    TEST_ASSERT_EQUAL_UINT8(0x3D, LoRaWAN::rawMessage[34]);
    TEST_ASSERT_EQUAL_UINT8(0x7E, LoRaWAN::rawMessage[35]);
    TEST_ASSERT_EQUAL_UINT8(0x11, LoRaWAN::rawMessage[36]);
    TEST_ASSERT_EQUAL_UINT8(0x73, LoRaWAN::rawMessage[37]);
    TEST_ASSERT_EQUAL_UINT8(0x93, LoRaWAN::rawMessage[38]);
    TEST_ASSERT_EQUAL_UINT8(0x17, LoRaWAN::rawMessage[39]);
    TEST_ASSERT_EQUAL_UINT8(0x2A, LoRaWAN::rawMessage[40]);
}

void test_encryptPayload() {
    aesKey aKey;
    uint8_t keyAsBytes[16]{0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};
    aKey.set(keyAsBytes);
    static constexpr uint32_t clearTextPayloadLength{16};
    LoRaWAN::DevAddr.asUint32          = 0x12345678;
    LoRaWAN::uplinkFrameCount.asUint32 = 0xFFEEDDCC;
    LoRaWAN::setOffsetsAndLengthsTx(clearTextPayloadLength, 0);
    uint8_t clearTextPayload[clearTextPayloadLength]{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    LoRaWAN::insertPayload(clearTextPayload, clearTextPayloadLength);
    LoRaWAN::encryptPayload(aKey);

    // Using https://www.cryptool.org/en/cto/aes-step-by-step
    //  Key :                      2b7e151628aed2a6abf7158809cf4f3c
    // Ai block i=0 :              01000000000078563412ccddeeff0001
    // Ai block after encryption : 4d9655c553debea0dfa6290bbad3ebc9
    // Then this needs to be XORed with the lorawan payload, here it is all zeroes so nothing changes for easy first test

    TEST_ASSERT_EQUAL_UINT8(0x4D, LoRaWAN::rawMessage[25]);
    TEST_ASSERT_EQUAL_UINT8(0x96, LoRaWAN::rawMessage[26]);
    TEST_ASSERT_EQUAL_UINT8(0x55, LoRaWAN::rawMessage[27]);
    TEST_ASSERT_EQUAL_UINT8(0xC5, LoRaWAN::rawMessage[28]);
    TEST_ASSERT_EQUAL_UINT8(0x53, LoRaWAN::rawMessage[29]);
    TEST_ASSERT_EQUAL_UINT8(0xDE, LoRaWAN::rawMessage[30]);
    TEST_ASSERT_EQUAL_UINT8(0xBE, LoRaWAN::rawMessage[31]);
    TEST_ASSERT_EQUAL_UINT8(0xA0, LoRaWAN::rawMessage[32]);
    TEST_ASSERT_EQUAL_UINT8(0xDF, LoRaWAN::rawMessage[33]);
    TEST_ASSERT_EQUAL_UINT8(0xA6, LoRaWAN::rawMessage[34]);
    TEST_ASSERT_EQUAL_UINT8(0x29, LoRaWAN::rawMessage[35]);
    TEST_ASSERT_EQUAL_UINT8(0x0B, LoRaWAN::rawMessage[36]);
    TEST_ASSERT_EQUAL_UINT8(0xBA, LoRaWAN::rawMessage[37]);
    TEST_ASSERT_EQUAL_UINT8(0xD3, LoRaWAN::rawMessage[38]);
    TEST_ASSERT_EQUAL_UINT8(0xEB, LoRaWAN::rawMessage[39]);
    TEST_ASSERT_EQUAL_UINT8(0xC9, LoRaWAN::rawMessage[40]);
}

void test_decryptPayload() {
    aesKey aKey;
    uint8_t keyAsBytes[16]{0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};
    aKey.set(keyAsBytes);
    static constexpr uint32_t clearTextPayloadLength{16};
    LoRaWAN::DevAddr.asUint32            = 0x12345678;
    LoRaWAN::downlinkFrameCount.asUint32 = 0xFFEEDDCC;
    LoRaWAN::setOffsetsAndLengthsTx(clearTextPayloadLength, 0);
    uint8_t clearTextPayload[clearTextPayloadLength]{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    LoRaWAN::insertPayload(clearTextPayload, clearTextPayloadLength);
    LoRaWAN::decryptPayload(aKey);

    // Using https://www.cryptool.org/en/cto/aes-step-by-step
    //  Key :                      2b7e151628aed2a6abf7158809cf4f3c
    // Ai block i=0 :              01000000000178563412ccddeeff0001
    // Ai block after encryption : d26ee43b33cd5463d48cb2c6cd710e21
    // Then this needs to be XORed with the lorawan payload, here it is all zeroes so nothing changes for easy first test

    TEST_ASSERT_EQUAL_UINT8(0xD2, LoRaWAN::rawMessage[25]);
    TEST_ASSERT_EQUAL_UINT8(0x6E, LoRaWAN::rawMessage[26]);
    TEST_ASSERT_EQUAL_UINT8(0xE4, LoRaWAN::rawMessage[27]);
    TEST_ASSERT_EQUAL_UINT8(0x3B, LoRaWAN::rawMessage[28]);
    TEST_ASSERT_EQUAL_UINT8(0x33, LoRaWAN::rawMessage[29]);
    TEST_ASSERT_EQUAL_UINT8(0xCD, LoRaWAN::rawMessage[30]);
    TEST_ASSERT_EQUAL_UINT8(0x54, LoRaWAN::rawMessage[31]);
    TEST_ASSERT_EQUAL_UINT8(0x63, LoRaWAN::rawMessage[32]);
    TEST_ASSERT_EQUAL_UINT8(0xD4, LoRaWAN::rawMessage[33]);
    TEST_ASSERT_EQUAL_UINT8(0x8C, LoRaWAN::rawMessage[34]);
    TEST_ASSERT_EQUAL_UINT8(0xB2, LoRaWAN::rawMessage[35]);
    TEST_ASSERT_EQUAL_UINT8(0xC6, LoRaWAN::rawMessage[36]);
    TEST_ASSERT_EQUAL_UINT8(0xCD, LoRaWAN::rawMessage[37]);
    TEST_ASSERT_EQUAL_UINT8(0x71, LoRaWAN::rawMessage[38]);
    TEST_ASSERT_EQUAL_UINT8(0x0E, LoRaWAN::rawMessage[39]);
    TEST_ASSERT_EQUAL_UINT8(0x21, LoRaWAN::rawMessage[40]);
}

void test_insertBlockB0() {
    deviceAddress testAddress(0x12345678);
    frameCount testFrameCount(0xFFEEDDCC);
    LoRaWAN::insertBlockB0(linkDirection::uplink, testFrameCount, 24);
    TEST_ASSERT_EQUAL_UINT8(0x49, LoRaWAN::rawMessage[0]);
    TEST_ASSERT_EQUAL_UINT8(0x00, LoRaWAN::rawMessage[1]);
    TEST_ASSERT_EQUAL_UINT8(0x00, LoRaWAN::rawMessage[2]);
    TEST_ASSERT_EQUAL_UINT8(0x00, LoRaWAN::rawMessage[3]);
    TEST_ASSERT_EQUAL_UINT8(0x00, LoRaWAN::rawMessage[4]);
    TEST_ASSERT_EQUAL_UINT8(0x00, LoRaWAN::rawMessage[5]);
    TEST_ASSERT_EQUAL_UINT8(0x78, LoRaWAN::rawMessage[6]);
    TEST_ASSERT_EQUAL_UINT8(0x56, LoRaWAN::rawMessage[7]);
    TEST_ASSERT_EQUAL_UINT8(0x34, LoRaWAN::rawMessage[8]);
    TEST_ASSERT_EQUAL_UINT8(0x12, LoRaWAN::rawMessage[9]);
    TEST_ASSERT_EQUAL_UINT8(0xCC, LoRaWAN::rawMessage[10]);
    TEST_ASSERT_EQUAL_UINT8(0xDD, LoRaWAN::rawMessage[11]);
    TEST_ASSERT_EQUAL_UINT8(0xEE, LoRaWAN::rawMessage[12]);
    TEST_ASSERT_EQUAL_UINT8(0xFF, LoRaWAN::rawMessage[13]);
    TEST_ASSERT_EQUAL_UINT8(0x00, LoRaWAN::rawMessage[14]);
    TEST_ASSERT_EQUAL_UINT8(24, LoRaWAN::rawMessage[15]);
}

void test_insertMic() {
    TEST_IGNORE_MESSAGE("Implement me!");
}

void test_isValidMic1() {
    static constexpr uint32_t testLoraPayloadLength{0x2B};
    uint8_t rxTestMessage[testLoraPayloadLength] = {0x60, 0x92, 0x3B, 0x0B, 0x26, 0x00, 0x3B, 0x00, 0x00, 0x11, 0x7C, 0xD9, 0xC4, 0xD5, 0x27, 0xA1, 0x78, 0xBD, 0x07, 0x88, 0x8B, 0x67, 0x52, 0xBC, 0xE4, 0x47, 0x26, 0xF9, 0x2B, 0x62, 0x89, 0xC3, 0x06, 0x2F, 0x69, 0x96, 0x4D, 0xD9, 0x18, 0x22, 0x58, 0x5F, 0x55};
    memcpy(LoRaWAN::rawMessage + LoRaWAN::b0BlockLength, rxTestMessage, testLoraPayloadLength);
    LoRaWAN::setOffsetsAndLengthsRx(testLoraPayloadLength);
    LoRaWAN::DevAddr.asUint32 = 0x260B3B92;
    LoRaWAN::downlinkFrameCount.set(0x0000'003B);
    TEST_ASSERT_TRUE(LoRaWAN::isValidMic());
}

void test_isValidMic2() {
    static constexpr uint32_t testLoraPayloadLength{15};
    uint8_t rxTestMessage[testLoraPayloadLength] = {0x60, 0x51, 0xAE, 0x0B, 0x26, 0x03, 0x01, 0x00, 0x02, 0x11, 0x02, 0x65, 0xF4, 0x53, 0xB5};
    memcpy(LoRaWAN::rawMessage + LoRaWAN::b0BlockLength, rxTestMessage, testLoraPayloadLength);
    LoRaWAN::setOffsetsAndLengthsRx(testLoraPayloadLength);
    LoRaWAN::downlinkFrameCount.set(0x0000'003B);
    TEST_ASSERT_TRUE(LoRaWAN::isValidMic());
}

void test_decodeMessage() {
    TEST_IGNORE_MESSAGE("Implement me!");
}

int main(int argc, char **argv) {
#ifndef generic
// Here we could setup the STM32 for target unit testing
#endif
    UNITY_BEGIN();

    RUN_TEST(test_constants);
    RUN_TEST(test_initialize);
    RUN_TEST(test_setOffsetsAndLengthsTx1);
    RUN_TEST(test_setOffsetsAndLengthsTx2);
    RUN_TEST(test_setOffsetsAndLengthsTx3);
    RUN_TEST(test_setOffsetsAndLengthsRx1);
    RUN_TEST(test_setOffsetsAndLengthsRx2);
    RUN_TEST(test_isValidDevAddr);
    RUN_TEST(test_prepareBlockAiTx);
    RUN_TEST(test_prepareBlockAiRx);
    RUN_TEST(test_isValidDownlinkFrameCount);
    RUN_TEST(test_insertHeaders1);
    RUN_TEST(test_insertHeaders2);
    RUN_TEST(test_insertHeaders3);
    RUN_TEST(test_insertBlockB0);
    RUN_TEST(test_insertPayload);
    RUN_TEST(test_encryptPayload);
    RUN_TEST(test_decryptPayload);
    // RUN_TEST(test_insertMic);
    // RUN_TEST(test_isValidMic1);
    // RUN_TEST(test_isValidMic2);
    // RUN_TEST(test_decodeMessage);

    UNITY_END();
}