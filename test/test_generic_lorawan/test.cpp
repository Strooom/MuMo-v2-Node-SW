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

void test_setOffsetsAndLengthsTx() {
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

    testFramePayloadLength = 32;
    testFrameOptionsLength = 0;
    LoRaWAN::setOffsetsAndLengthsTx(testFramePayloadLength, testFrameOptionsLength);
    TEST_ASSERT_EQUAL(40, LoRaWAN::macPayloadLength);
    TEST_ASSERT_EQUAL(1, LoRaWAN::framePortLength);
    TEST_ASSERT_EQUAL(45, LoRaWAN::loRaPayloadLength);
    TEST_ASSERT_EQUAL(57, LoRaWAN::micOffset);

    testFramePayloadLength = 28;
    testFrameOptionsLength = 4;
    LoRaWAN::setOffsetsAndLengthsTx(testFramePayloadLength, testFrameOptionsLength);
    TEST_ASSERT_EQUAL(40, LoRaWAN::macPayloadLength);
    TEST_ASSERT_EQUAL(1, LoRaWAN::framePortLength);
    TEST_ASSERT_EQUAL(45, LoRaWAN::loRaPayloadLength);
    TEST_ASSERT_EQUAL(57, LoRaWAN::micOffset);
    
}

void test_isValidDevAddr() {
    deviceAddress testAddress1{0xABCD};
    deviceAddress testAddress2{0x5678};
    TEST_ASSERT_TRUE(LoRaWAN::isValidDevAddr(testAddress1));
    TEST_ASSERT_FALSE(LoRaWAN::isValidDevAddr(testAddress2));
}

void test_setOffsetsAndLengthsRx() {
    static constexpr uint32_t loRaPayloadLength{32};
    uint8_t receivedBytes[loRaPayloadLength] = {};
    memcpy(LoRaWAN::rawMessage, receivedBytes, loRaPayloadLength);
    LoRaWAN::setOffsetsAndLengthsRx(loRaPayloadLength);
    TEST_ASSERT_EQUAL(0, LoRaWAN::macPayloadLength);
    TEST_ASSERT_EQUAL(1, LoRaWAN::framePortLength);
    TEST_ASSERT_EQUAL(0, LoRaWAN::frameOptionsLength);
    TEST_ASSERT_EQUAL(0, LoRaWAN::frameHeaderLength);
    TEST_ASSERT_EQUAL(0, LoRaWAN::framePayloadLength);
    TEST_ASSERT_EQUAL(0, LoRaWAN::loRaPayloadLength);
    TEST_ASSERT_EQUAL(0, LoRaWAN::framePortOffset);
    TEST_ASSERT_EQUAL(0, LoRaWAN::framePayloadOffset);
    TEST_ASSERT_EQUAL(0, LoRaWAN::micOffset);
}

void test_prepareBlockAi() {
    aesBlock testBlock;
    uint8_t testBlockDataUp[aesBlock::lengthAsBytes] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
    testBlock.set(testBlockDataUp);
    deviceAddress testAddress(0x1234);
    frameCount testFrameCount(0x5678);
    uint32_t testBlockIndex{7};
    LoRaWAN::prepareBlockAi(testBlock, linkDirection::uplink, testAddress, testFrameCount, testBlockIndex);
    TEST_ASSERT_EQUAL_UINT8(0x01, testBlock[0]);
    TEST_ASSERT_EQUAL_UINT8(0x02, testBlock[1]);
    TEST_ASSERT_EQUAL_UINT8(0x03, testBlock[2]);
    TEST_ASSERT_EQUAL_UINT8(0x04, testBlock[3]);
    TEST_ASSERT_EQUAL_UINT8(0x56, testBlock[4]);
    TEST_ASSERT_EQUAL_UINT8(0x78, testBlock[5]);
    TEST_ASSERT_EQUAL_UINT8(0x00, testBlock[6]);
    TEST_ASSERT_EQUAL_UINT8(0x07, testBlock[7]);
    TEST_ASSERT_EQUAL_UINT8(0x12, testBlock[8]);
    TEST_ASSERT_EQUAL_UINT8(0x34, testBlock[9]);
    TEST_ASSERT_EQUAL_UINT8(0x00, testBlock[10]);
    TEST_ASSERT_EQUAL_UINT8(0x00, testBlock[11]);
    TEST_ASSERT_EQUAL_UINT8(0x00, testBlock[12]);
    TEST_ASSERT_EQUAL_UINT8(0x00, testBlock[13]);
    TEST_ASSERT_EQUAL_UINT8(0x00, testBlock[14]);
    TEST_ASSERT_EQUAL_UINT8(0x07, testBlock[15]);

    uint8_t testBlockDataDown[aesBlock::lengthAsBytes] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
    testBlock.set(testBlockDataDown);
    LoRaWAN::prepareBlockAi(testBlock, linkDirection::downlink, testAddress, testFrameCount, testBlockIndex);
    TEST_ASSERT_EQUAL_UINT8(0x01, testBlock[0]);
    TEST_ASSERT_EQUAL_UINT8(0x02, testBlock[1]);
    TEST_ASSERT_EQUAL_UINT8(0x03, testBlock[2]);
    TEST_ASSERT_EQUAL_UINT8(0x04, testBlock[3]);
    TEST_ASSERT_EQUAL_UINT8(0x56, testBlock[4]);
    TEST_ASSERT_EQUAL_UINT8(0x78, testBlock[5]);
    TEST_ASSERT_EQUAL_UINT8(0x00, testBlock[6]);
    TEST_ASSERT_EQUAL_UINT8(0x07, testBlock[7]);
    TEST_ASSERT_EQUAL_UINT8(0x12, testBlock[8]);
    TEST_ASSERT_EQUAL_UINT8(0x34, testBlock[9]);
    TEST_ASSERT_EQUAL_UINT8(0x00, testBlock[10]);
    TEST_ASSERT_EQUAL_UINT8(0x00, testBlock[11]);
    TEST_ASSERT_EQUAL_UINT8(0x00, testBlock[12]);
    TEST_ASSERT_EQUAL_UINT8(0x00, testBlock[13]);
    TEST_ASSERT_EQUAL_UINT8(0x00, testBlock[14]);
    TEST_ASSERT_EQUAL_UINT8(0x07, testBlock[15]);
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

void test_encryptPayload() {
    TEST_IGNORE_MESSAGE("Implement me!");
}

void test_decryptPayload() {
    TEST_IGNORE_MESSAGE("Implement me!");
}

void test_insertHeaders() {
    TEST_IGNORE_MESSAGE("Implement me!");
}

void test_insertBlockB0() {
    deviceAddress testAddress(0x1234);
    frameCount testFrameCount(0x5678);
    LoRaWAN::insertBlockB0(linkDirection::uplink, testAddress, testFrameCount, 24);
    TEST_ASSERT_EQUAL_UINT8(0x49, LoRaWAN::rawMessage[0]);
    TEST_ASSERT_EQUAL_UINT8(0x12, LoRaWAN::rawMessage[1]);
    TEST_ASSERT_EQUAL_UINT8(0x34, LoRaWAN::rawMessage[2]);
    TEST_ASSERT_EQUAL_UINT8(0x56, LoRaWAN::rawMessage[3]);
    TEST_ASSERT_EQUAL_UINT8(0x78, LoRaWAN::rawMessage[4]);
    TEST_ASSERT_EQUAL_UINT8(0x00, LoRaWAN::rawMessage[5]);
    TEST_ASSERT_EQUAL_UINT8(0x00, LoRaWAN::rawMessage[6]);
    TEST_ASSERT_EQUAL_UINT8(0x00, LoRaWAN::rawMessage[7]);
    TEST_ASSERT_EQUAL_UINT8(0x00, LoRaWAN::rawMessage[8]);
    TEST_ASSERT_EQUAL_UINT8(0x00, LoRaWAN::rawMessage[9]);
    TEST_ASSERT_EQUAL_UINT8(0x00, LoRaWAN::rawMessage[10]);
    TEST_ASSERT_EQUAL_UINT8(0x00, LoRaWAN::rawMessage[11]);
    TEST_ASSERT_EQUAL_UINT8(0x00, LoRaWAN::rawMessage[12]);
    TEST_ASSERT_EQUAL_UINT8(0x00, LoRaWAN::rawMessage[13]);
    TEST_ASSERT_EQUAL_UINT8(0x00, LoRaWAN::rawMessage[14]);
    TEST_ASSERT_EQUAL_UINT8(0x00, LoRaWAN::rawMessage[15]);
}

void test_insertMic() {
    TEST_IGNORE_MESSAGE("Implement me!");
}

void test_isValidMic() {
    TEST_IGNORE_MESSAGE("Implement me!");
}

void test_decodeMessage() {
    TEST_IGNORE_MESSAGE("Implement me!");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_constants);
    RUN_TEST(test_initialize);
    RUN_TEST(test_setOffsetsAndLengthsTx);
    // RUN_TEST(test_isValidDevAddr);
    // RUN_TEST(test_setOffsetsAndLengthsRx);
    // RUN_TEST(test_prepareBlockAi);
    // RUN_TEST(test_isValidDownlinkFrameCount);
    // RUN_TEST(test_insertHeaders);
    // RUN_TEST(test_encryptPayload);
    // RUN_TEST(test_decryptPayload);
    // RUN_TEST(test_insertBlockB0);
    // RUN_TEST(test_insertMic);
    // RUN_TEST(test_isValidMic);
    // RUN_TEST(test_decodeMessage);

    UNITY_END();
}