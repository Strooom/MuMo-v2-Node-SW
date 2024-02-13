#include <unity.h>
#include <lorawan.hpp>
#include <circularbuffer.hpp>
#include <lorawanevent.hpp>
#include <applicationevent.hpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;
circularBuffer<loRaWanEvent, 16U> loraWanEventBuffer;

uint8_t mockSX126xDataBuffer[256];
uint8_t mockSX126xRegisters[0x1000];
uint8_t mockSX126xCommandData[256][8];

uint8_t allZeroes[LoRaWAN::rawMessageLength];
uint8_t allOnes[LoRaWAN::rawMessageLength];

void setUp(void) {
    memset(allZeroes, 0, LoRaWAN::rawMessageLength);
    memset(allOnes, 0xFF, LoRaWAN::rawMessageLength);
}
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
    // TODO : check if these test cases are enough
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

void test_insertBlockB0() {
    LoRaWAN::DevAddr            = 0x12345678;
    LoRaWAN::uplinkFrameCount   = 0xFFEEDDCC;
    LoRaWAN::downlinkFrameCount = 0x00112233;
    uint32_t testPayloadLength{16};
    LoRaWAN::clearRawMessage();
    LoRaWAN::setOffsetsAndLengthsTx(testPayloadLength, 0);
    LoRaWAN::insertBlockB0(linkDirection::uplink, LoRaWAN::uplinkFrameCount);
    const uint8_t expectedTx[LoRaWAN::b0BlockLength]{0x49, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x56, 0x34, 0x12, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 41};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedTx, LoRaWAN::rawMessage, LoRaWAN::b0BlockLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage + LoRaWAN::b0BlockLength, (LoRaWAN::rawMessageLength - LoRaWAN::b0BlockLength));

    LoRaWAN::insertBlockB0(linkDirection::downlink, LoRaWAN::downlinkFrameCount);
    const uint8_t expectedRx[LoRaWAN::b0BlockLength]{0x49, 0x00, 0x00, 0x00, 0x00, 0x01, 0x78, 0x56, 0x34, 0x12, 0x33, 0x22, 0x11, 0x00, 0x00, 41};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedRx, LoRaWAN::rawMessage, LoRaWAN::b0BlockLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage + LoRaWAN::b0BlockLength, (LoRaWAN::rawMessageLength - LoRaWAN::b0BlockLength));
}

void test_insertHeaders() {
    LoRaWAN::DevAddr          = 0x12345678;
    LoRaWAN::uplinkFrameCount = 0xFFEEDDCC;
    uint32_t testPayloadLength{0};
    uint32_t testFrameOptionsLength{0};
    uint32_t testFramePort{8};
    uint8_t testFrameOptions[15]{0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E};

    // We should in all these tests consider at least 4 scenarios :
    // 1. some payload, no frameoptions

    testPayloadLength      = 24;
    testFrameOptionsLength = 0;
    LoRaWAN::clearRawMessage();
    LoRaWAN::setOffsetsAndLengthsTx(testPayloadLength, testFrameOptionsLength);
    LoRaWAN::insertHeaders(testFrameOptions, testFrameOptionsLength, testPayloadLength, testFramePort);
    const uint8_t expectedHeader1[23]{0b01000000, 0x78, 0x56, 0x34, 0x12, 0x00, 0xCC, 0xDD, 8};

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedHeader1, LoRaWAN::rawMessage + LoRaWAN::macHeaderOffset, LoRaWAN::macHeaderLength + LoRaWAN::frameHeaderLength + LoRaWAN::framePortLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage, LoRaWAN::b0BlockLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage + LoRaWAN::framePayloadOffset, (LoRaWAN::rawMessageLength - (LoRaWAN::b0BlockLength + LoRaWAN::macHeaderLength + LoRaWAN::frameHeaderLength + LoRaWAN::framePortLength)));

    // 2. some ayload + some frameoptions

    testPayloadLength      = 24;
    testFrameOptionsLength = 8;
    LoRaWAN::clearRawMessage();
    LoRaWAN::setOffsetsAndLengthsTx(testPayloadLength, testFrameOptionsLength);
    LoRaWAN::insertHeaders(testFrameOptions, testFrameOptionsLength, testPayloadLength, testFramePort);
    const uint8_t expectedHeader2[23]{0b01000000, 0x78, 0x56, 0x34, 0x12, 0x08, 0xCC, 0xDD, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 8};

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedHeader2, LoRaWAN::rawMessage + LoRaWAN::macHeaderOffset, LoRaWAN::macHeaderLength + LoRaWAN::frameHeaderLength + LoRaWAN::framePortLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage, LoRaWAN::b0BlockLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage + LoRaWAN::framePayloadOffset, (LoRaWAN::rawMessageLength - (LoRaWAN::b0BlockLength + LoRaWAN::macHeaderLength + LoRaWAN::frameHeaderLength + LoRaWAN::framePortLength)));

    // 3. no payload, some frameoptions

    testPayloadLength      = 0;
    testFrameOptionsLength = 8;
    LoRaWAN::clearRawMessage();
    LoRaWAN::setOffsetsAndLengthsTx(testPayloadLength, testFrameOptionsLength);
    LoRaWAN::insertHeaders(testFrameOptions, testFrameOptionsLength, testPayloadLength, testFramePort);
    const uint8_t expectedHeader3[23]{0b01000000, 0x78, 0x56, 0x34, 0x12, 0x08, 0xCC, 0xDD, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87};

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedHeader3, LoRaWAN::rawMessage + LoRaWAN::macHeaderOffset, LoRaWAN::macHeaderLength + LoRaWAN::frameHeaderLength + LoRaWAN::framePortLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage, LoRaWAN::b0BlockLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage + LoRaWAN::framePayloadOffset, (LoRaWAN::rawMessageLength - (LoRaWAN::b0BlockLength + LoRaWAN::macHeaderLength + LoRaWAN::frameHeaderLength + LoRaWAN::framePortLength)));

    // 4. no payload, no frameoptions
    testPayloadLength      = 0;
    testFrameOptionsLength = 0;
    LoRaWAN::clearRawMessage();
    LoRaWAN::setOffsetsAndLengthsTx(testPayloadLength, testFrameOptionsLength);
    LoRaWAN::insertHeaders(testFrameOptions, testFrameOptionsLength, testPayloadLength, testFramePort);
    const uint8_t expectedHeader4[23]{0b01000000, 0x78, 0x56, 0x34, 0x12, 0x00, 0xCC, 0xDD};

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedHeader4, LoRaWAN::rawMessage + LoRaWAN::macHeaderOffset, LoRaWAN::macHeaderLength + LoRaWAN::frameHeaderLength + LoRaWAN::framePortLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage, LoRaWAN::b0BlockLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage + LoRaWAN::framePayloadOffset, (LoRaWAN::rawMessageLength - (LoRaWAN::b0BlockLength + LoRaWAN::macHeaderLength + LoRaWAN::frameHeaderLength + LoRaWAN::framePortLength)));
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

void test_padForMicCalculation() {
    LoRaWAN::setOffsetsAndLengthsTx(24, 0);
    memcpy(LoRaWAN::rawMessage + LoRaWAN::micOffset, allOnes, 16);        // writing 0xFF so we can be sure it's overwritten with zeroes for the correct amount and not just zeroes from initialization
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

void test_insertMic() {
    uint32_t testFramePayloadLength{0};
    uint32_t testFrameOptionsLength{0};
    LoRaWAN::setOffsetsAndLengthsTx(testFramePayloadLength, testFrameOptionsLength);
    LoRaWAN::clearRawMessage();
    LoRaWAN::insertMic(0x12345678);
    TEST_ASSERT_EQUAL_UINT8(0x78, LoRaWAN::rawMessage[24]);
    TEST_ASSERT_EQUAL_UINT8(0x56, LoRaWAN::rawMessage[25]);
    TEST_ASSERT_EQUAL_UINT8(0x34, LoRaWAN::rawMessage[26]);
    TEST_ASSERT_EQUAL_UINT8(0x12, LoRaWAN::rawMessage[27]);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage, 24);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage + 28, LoRaWAN::rawMessageLength - 28);
}

void test_getReceivedFramecount() {
    static constexpr uint32_t testLoRaPayloadLength{16};
    uint8_t testLoRaPayload[testLoRaPayloadLength]{0x60, 0x78, 0x56, 0x34, 0x12, 0x00, 0xCC, 0xDD, 0x01, 0x00, 0x01, 0x02, 0x13, 0xF1, 0x4E, 0x5E};
    LoRaWAN::clearRawMessage();
    memcpy(LoRaWAN::rawMessage + LoRaWAN::loRaPayloadOffset, testLoRaPayload, testLoRaPayloadLength);
    LoRaWAN::setOffsetsAndLengthsRx(testLoRaPayloadLength);
    TEST_ASSERT_EQUAL_UINT16(0xDDCC, LoRaWAN::getReceivedFramecount());

}

void test_receivedMic() {
    static constexpr uint32_t testLoRaPayloadLength{16};
    uint8_t testLoRaPayload[testLoRaPayloadLength]{0x60, 0x78, 0x56, 0x34, 0x12, 0x00, 0xCC, 0xDD, 0x01, 0x00, 0x01, 0x02, 0x13, 0xF1, 0x4E, 0x5E};
    LoRaWAN::clearRawMessage();
    memcpy(LoRaWAN::rawMessage + LoRaWAN::loRaPayloadOffset, testLoRaPayload, testLoRaPayloadLength);
    LoRaWAN::setOffsetsAndLengthsRx(testLoRaPayloadLength);
    TEST_ASSERT_EQUAL_UINT32(0x5E4EF113, LoRaWAN::getReceivedMic());
}

void test_receivedDeviceAddress() {
    static constexpr uint32_t testLoRaPayloadLength{16};
    uint8_t testLoRaPayload[testLoRaPayloadLength]{0x60, 0x78, 0x56, 0x34, 0x12, 0x00, 0xCC, 0xDD, 0x01, 0x00, 0x01, 0x02, 0x13, 0xF1, 0x4E, 0x5E};
    LoRaWAN::clearRawMessage();
    memcpy(LoRaWAN::rawMessage + LoRaWAN::loRaPayloadOffset, testLoRaPayload, testLoRaPayloadLength);
    LoRaWAN::setOffsetsAndLengthsRx(testLoRaPayloadLength);
    TEST_ASSERT_EQUAL_UINT32(0x12345678, LoRaWAN::getReceivedDeviceAddress());
}


int main(int argc, char **argv) {
#ifndef generic
// Here we could setup the STM32 for target unit testing
#endif
    UNITY_BEGIN();

    RUN_TEST(test_constants);
    RUN_TEST(test_setOffsetsAndLengthsTx);
    RUN_TEST(test_setOffsetsAndLengthsRx);

    RUN_TEST(test_insertBlockB0);
    RUN_TEST(test_insertHeaders);
    RUN_TEST(test_insertPayload);
    RUN_TEST(test_padForMicCalculation);
    RUN_TEST(test_insertMic);
    RUN_TEST(test_receivedMic);
    RUN_TEST(test_receivedDeviceAddress);
    RUN_TEST(test_getReceivedFramecount);


    UNITY_END();
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
