#include <unity.h>
#include <lorawan.hpp>
#include <circularbuffer.hpp>
#include <lorawanevent.hpp>
#include <applicationevent.hpp>
#include <settingscollection.hpp>
#include <maccommand.hpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;
circularBuffer<loRaWanEvent, 16U> loraWanEventBuffer;

uint8_t mockSX126xDataBuffer[256];            // unitTesting mock for the LoRa Rx/Tx buffer, inside the SX126x
uint8_t mockSX126xRegisters[0x1000];          // unitTesting mock for the config registers, inside the SX126x
uint8_t mockSX126xCommandData[256][8];        // unitTesting mock for capturing the commands and their parameters to configure the SX126x

void setUp(void) {}
void tearDown(void) {}

void test_constants() {
    TEST_ASSERT_EQUAL(16, LoRaWAN::b0BlockLength);
    TEST_ASSERT_EQUAL(1, LoRaWAN::macHeaderLength);
    TEST_ASSERT_EQUAL(16, LoRaWAN::macHeaderOffset);
    TEST_ASSERT_EQUAL(16, LoRaWAN::loRaPayloadOffset);
    TEST_ASSERT_EQUAL(4, LoRaWAN::micLength);

    TEST_ASSERT_EQUAL(4, LoRaWAN::deviceAddressLength);
    TEST_ASSERT_EQUAL(1, LoRaWAN::frameControlLength);
    TEST_ASSERT_EQUAL(2, LoRaWAN::frameCountLSHLength);

    TEST_ASSERT_EQUAL(17, LoRaWAN::deviceAddressOffset);
    TEST_ASSERT_EQUAL(21, LoRaWAN::frameControlOffset);
    TEST_ASSERT_EQUAL(22, LoRaWAN::frameCountOffset);
    TEST_ASSERT_EQUAL(24, LoRaWAN::frameOptionsOffset);
}

void test_setOffsetsAndLengthsTx() {
    // Case 1 : minimal LoRaWAN message

    uint32_t testFramePayloadLength{0};
    uint32_t testFrameOptionsLength{0};
    LoRaWAN::setOffsetsAndLengthsTx(testFramePayloadLength, testFrameOptionsLength);

    TEST_ASSERT_EQUAL(0, LoRaWAN::frameOptionsLength);
    TEST_ASSERT_EQUAL(7, LoRaWAN::frameHeaderLength);
    TEST_ASSERT_EQUAL(0, LoRaWAN::framePortLength);
    TEST_ASSERT_EQUAL(0, LoRaWAN::framePayloadLength);

    TEST_ASSERT_EQUAL(24, LoRaWAN::framePortOffset);
    TEST_ASSERT_EQUAL(24, LoRaWAN::framePayloadOffset);

    TEST_ASSERT_EQUAL(7, LoRaWAN::macPayloadLength);
    TEST_ASSERT_EQUAL(24, LoRaWAN::micOffset);

    TEST_ASSERT_EQUAL(12, LoRaWAN::loRaPayloadLength);

    // Case 2 : 32 byte application payload, no frameOptions

    testFramePayloadLength = 32;
    testFrameOptionsLength = 0;
    LoRaWAN::setOffsetsAndLengthsTx(testFramePayloadLength, testFrameOptionsLength);

    TEST_ASSERT_EQUAL(0, LoRaWAN::frameOptionsLength);
    TEST_ASSERT_EQUAL(7, LoRaWAN::frameHeaderLength);
    TEST_ASSERT_EQUAL(1, LoRaWAN::framePortLength);
    TEST_ASSERT_EQUAL(32, LoRaWAN::framePayloadLength);

    TEST_ASSERT_EQUAL(24, LoRaWAN::framePortOffset);
    TEST_ASSERT_EQUAL(25, LoRaWAN::framePayloadOffset);

    TEST_ASSERT_EQUAL(40, LoRaWAN::macPayloadLength);
    TEST_ASSERT_EQUAL(57, LoRaWAN::micOffset);

    TEST_ASSERT_EQUAL(45, LoRaWAN::loRaPayloadLength);

    // Case 3 : 28 bytes application payload, 4 bytes frameOptions

    testFramePayloadLength = 28;
    testFrameOptionsLength = 4;
    LoRaWAN::setOffsetsAndLengthsTx(testFramePayloadLength, testFrameOptionsLength);

    TEST_ASSERT_EQUAL(4, LoRaWAN::frameOptionsLength);
    TEST_ASSERT_EQUAL(11, LoRaWAN::frameHeaderLength);
    TEST_ASSERT_EQUAL(1, LoRaWAN::framePortLength);
    TEST_ASSERT_EQUAL(28, LoRaWAN::framePayloadLength);

    TEST_ASSERT_EQUAL(28, LoRaWAN::framePortOffset);
    TEST_ASSERT_EQUAL(29, LoRaWAN::framePayloadOffset);

    TEST_ASSERT_EQUAL(40, LoRaWAN::macPayloadLength);
    TEST_ASSERT_EQUAL(57, LoRaWAN::micOffset);

    TEST_ASSERT_EQUAL(45, LoRaWAN::loRaPayloadLength);

    // Case 4 : no application payload, 15 bytes frameOptions

    testFramePayloadLength = 0;
    testFrameOptionsLength = 15;
    LoRaWAN::setOffsetsAndLengthsTx(testFramePayloadLength, testFrameOptionsLength);

    TEST_ASSERT_EQUAL(15, LoRaWAN::frameOptionsLength);
    TEST_ASSERT_EQUAL(22, LoRaWAN::frameHeaderLength);
    TEST_ASSERT_EQUAL(0, LoRaWAN::framePortLength);
    TEST_ASSERT_EQUAL(0, LoRaWAN::framePayloadLength);

    TEST_ASSERT_EQUAL(39, LoRaWAN::framePortOffset);
    TEST_ASSERT_EQUAL(39, LoRaWAN::framePayloadOffset);

    TEST_ASSERT_EQUAL(22, LoRaWAN::macPayloadLength);
    TEST_ASSERT_EQUAL(39, LoRaWAN::micOffset);

    TEST_ASSERT_EQUAL(27, LoRaWAN::loRaPayloadLength);
}

void test_setOffsetsAndLengthsRx() {
    static constexpr uint32_t loRaPayloadLength1{12};
    uint8_t receivedBytes1[loRaPayloadLength1] = {0b01100000, 0x78, 0x56, 0x34, 0x12, 0b00000000, 0x00, 0x00, 0xaa, 0xbb, 0xcc, 0xdd};        // minimal msg with no payload and no options
    memcpy(LoRaWAN::rawMessage + LoRaWAN::b0BlockLength, receivedBytes1, loRaPayloadLength1);
    LoRaWAN::setOffsetsAndLengthsRx(loRaPayloadLength1);
    TEST_ASSERT_EQUAL(7, LoRaWAN::macPayloadLength);
    TEST_ASSERT_EQUAL(0, LoRaWAN::framePortLength);
    TEST_ASSERT_EQUAL(0, LoRaWAN::frameOptionsLength);

    TEST_ASSERT_EQUAL(7, LoRaWAN::frameHeaderLength);
    TEST_ASSERT_EQUAL(0, LoRaWAN::framePayloadLength);
    TEST_ASSERT_EQUAL(12, LoRaWAN::loRaPayloadLength);
    TEST_ASSERT_EQUAL(24, LoRaWAN::micOffset);

    static constexpr uint32_t loRaPayloadLength2{21};
    uint8_t receivedBytes2[loRaPayloadLength2] = {0b01100000, 0x78, 0x56, 0x34, 0x12, 0b00000100, 0x00, 0x00, 0x88, 0x77, 0x66, 0x55, 0x01, 0x10, 0x20, 0x30, 0x40, 0xaa, 0xbb, 0xcc, 0xdd};        // msg with payload and no frameoptions
    memcpy(LoRaWAN::rawMessage + LoRaWAN::b0BlockLength, receivedBytes2, loRaPayloadLength2);
    LoRaWAN::setOffsetsAndLengthsRx(loRaPayloadLength2);
    TEST_ASSERT_EQUAL(16, LoRaWAN::macPayloadLength);
    TEST_ASSERT_EQUAL(1, LoRaWAN::framePortLength);
    TEST_ASSERT_EQUAL(4, LoRaWAN::frameOptionsLength);

    TEST_ASSERT_EQUAL(11, LoRaWAN::frameHeaderLength);
    TEST_ASSERT_EQUAL(4, LoRaWAN::framePayloadLength);
    TEST_ASSERT_EQUAL(33, LoRaWAN::micOffset);
}

// void test_padForEncryptDecrypt() {
//     static constexpr uint32_t clearTextPayloadLength{24};
//     LoRaWAN::setOffsetsAndLengthsTx(clearTextPayloadLength, 0);
//     LoRaWAN::padForEncryptDecrypt();
//     TEST_ASSERT_EQUAL(8, LoRaWAN::nmbrOfBytesToPad);
//     TEST_ASSERT_EQUAL_UINT8_ARRAY(LoRaWAN::padBuffer, LoRaWAN::rawMessage + LoRaWAN::framePayloadOffset, 8);
//     LoRaWAN::setOffsetsAndLengthsTx(0, 5);        // 5 bytes option, no payload
//     LoRaWAN::padForEncryptDecrypt();
//     TEST_ASSERT_EQUAL(3, LoRaWAN::nmbrOfBytesToPad);
//     LoRaWAN::setOffsetsAndLengthsTx(0, 0);        // minimal LoRaWAN message EDGE case, as there is no payload, so no encryption, and thus no padding
//     LoRaWAN::padForEncryptDecrypt();
//     TEST_ASSERT_EQUAL(0, LoRaWAN::nmbrOfBytesToPad);
// }

void test_padForMicCalculation() {
    const uint8_t allZeroes[16]{};
    const uint8_t allOnes[16]{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    LoRaWAN::setOffsetsAndLengthsTx(24, 0);
    memcpy(LoRaWAN::rawMessage + LoRaWAN::micOffset, allOnes, 16); // writing 0xFF so we can be sure it's overwritten with zeroes for the correct amount and not just zeroes from initialization
    LoRaWAN::padForMicCalculation();
    TEST_ASSERT_EQUAL(15, LoRaWAN::nmbrOfBytesToPad);
    TEST_ASSERT_EQUAL_UINT8(0x80, LoRaWAN::rawMessage[LoRaWAN::micOffset]);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage + LoRaWAN::micOffset + 1, LoRaWAN::nmbrOfBytesToPad - 1);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allOnes, LoRaWAN::rawMessage + LoRaWAN::micOffset + LoRaWAN::nmbrOfBytesToPad, 16 - LoRaWAN::nmbrOfBytesToPad);

    LoRaWAN::setOffsetsAndLengthsTx(23, 0);
    memcpy(LoRaWAN::rawMessage + LoRaWAN::micOffset, allOnes, 16);
    LoRaWAN::padForMicCalculation();
    TEST_ASSERT_EQUAL(0, LoRaWAN::nmbrOfBytesToPad);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allOnes, LoRaWAN::rawMessage + LoRaWAN::micOffset + LoRaWAN::nmbrOfBytesToPad, 16 - LoRaWAN::nmbrOfBytesToPad);

    LoRaWAN::setOffsetsAndLengthsTx(22, 0);
    memcpy(LoRaWAN::rawMessage + LoRaWAN::micOffset, allOnes, 16);
    LoRaWAN::padForMicCalculation();
    TEST_ASSERT_EQUAL(1, LoRaWAN::nmbrOfBytesToPad);
    TEST_ASSERT_EQUAL_UINT8(0x80, LoRaWAN::rawMessage[LoRaWAN::micOffset]);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allOnes, LoRaWAN::rawMessage + LoRaWAN::micOffset + LoRaWAN::nmbrOfBytesToPad, 16 - LoRaWAN::nmbrOfBytesToPad);

    LoRaWAN::setOffsetsAndLengthsTx(21, 0);
    memcpy(LoRaWAN::rawMessage + LoRaWAN::micOffset, allOnes, 16);
    LoRaWAN::padForMicCalculation();
    TEST_ASSERT_EQUAL(2, LoRaWAN::nmbrOfBytesToPad);
    TEST_ASSERT_EQUAL_UINT8(0x80, LoRaWAN::rawMessage[LoRaWAN::micOffset]);
    TEST_ASSERT_EQUAL_UINT8(0x00, LoRaWAN::rawMessage[LoRaWAN::micOffset + 1]);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allOnes, LoRaWAN::rawMessage + LoRaWAN::micOffset + LoRaWAN::nmbrOfBytesToPad, 16 - LoRaWAN::nmbrOfBytesToPad);
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


// TODO : there is no test for inserting a header with payload and no frameoptions
// We should in all these tests consider at least 4 scenarios :
// 1. payload, no frameoptions
// 2. payload, frameoptions
// 3. no payload, frameoptions
// 4. no payload, no frameoptions

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
    // Case 1 : no frameOptions
    static constexpr uint32_t clearTextPayloadLength{16};
    LoRaWAN::setOffsetsAndLengthsTx(clearTextPayloadLength, 0);
    uint8_t clearTextPayload[clearTextPayloadLength]{0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96, 0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A};        // common NIST test vector plaintext 6bc1bee22e409f96e93d7e117393172a
    LoRaWAN::insertPayload(clearTextPayload, clearTextPayloadLength);
    TEST_ASSERT_EQUAL_INT8_ARRAY(clearTextPayload, LoRaWAN::rawMessage + LoRaWAN::framePayloadOffset, clearTextPayloadLength);

    // Case 2 : with some frameOptions
    static constexpr uint32_t testFrameOptionsLength{4};
    LoRaWAN::setOffsetsAndLengthsTx(clearTextPayloadLength, testFrameOptionsLength);
    uint8_t testFrameOptions[testFrameOptionsLength] = {0x11, 0x22, 0x33, 0x44};
    LoRaWAN::insertHeaders(testFrameOptions, testFrameOptionsLength, 0, 0);
    LoRaWAN::insertPayload(clearTextPayload, clearTextPayloadLength);
    TEST_ASSERT_EQUAL_INT8_ARRAY(testFrameOptions, LoRaWAN::rawMessage + LoRaWAN::frameOptionsOffset, testFrameOptionsLength);
    TEST_ASSERT_EQUAL_INT8_ARRAY(clearTextPayload, LoRaWAN::rawMessage + LoRaWAN::framePayloadOffset, clearTextPayloadLength);
}

void test_insertBlockB0() {
    deviceAddress testAddress(0x12345678);
    frameCount testFrameCount(0xFFEEDDCC);
    LoRaWAN::insertBlockB0(linkDirection::uplink, testFrameCount, 24);
    const uint8_t expected[16]{0x49, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x56, 0x34, 0x12, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x18};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, LoRaWAN::rawMessage, 16);
}

void test_isValidDevAddr() {
    LoRaWAN::DevAddr.asUint32 = 0x1234;
    deviceAddress testAddress1{0x1234};
    deviceAddress testAddress2{0x5678};
    TEST_ASSERT_TRUE(LoRaWAN::isValidDevAddr(testAddress1));
    TEST_ASSERT_FALSE(LoRaWAN::isValidDevAddr(testAddress2));
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

int main(int argc, char **argv) {
#ifndef generic
// Here we could setup the STM32 for target unit testing
#endif
    UNITY_BEGIN();

    RUN_TEST(test_constants);
    RUN_TEST(test_setOffsetsAndLengthsTx);
    RUN_TEST(test_setOffsetsAndLengthsRx);
    // RUN_TEST(test_padForEncryptDecrypt);
    RUN_TEST(test_padForMicCalculation);
    RUN_TEST(test_isValidDevAddr);
    RUN_TEST(test_prepareBlockAiTx);
    RUN_TEST(test_prepareBlockAiRx);
    RUN_TEST(test_isValidDownlinkFrameCount);
    RUN_TEST(test_insertHeaders1);
    RUN_TEST(test_insertHeaders2);
    RUN_TEST(test_insertHeaders3);
    RUN_TEST(test_insertBlockB0);
    RUN_TEST(test_insertPayload);

    UNITY_END();
}