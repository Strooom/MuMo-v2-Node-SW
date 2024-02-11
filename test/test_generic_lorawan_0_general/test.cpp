#include <unity.h>
#include <lorawan.hpp>
#include <circularbuffer.hpp>
#include <lorawanevent.hpp>
#include <applicationevent.hpp>
#include <settingscollection.hpp>
#include <maccommand.hpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;
circularBuffer<loRaWanEvent, 16U> loraWanEventBuffer;

uint8_t mockSX126xDataBuffer[256];
uint8_t mockSX126xRegisters[0x1000];
uint8_t mockSX126xCommandData[256][8];

void setUp(void) {}
void tearDown(void) {}

void test_initialize() {
    // Before reading from NVS
    TEST_ASSERT_EQUAL(0, LoRaWAN::uplinkFrameCount.asUint32);
    TEST_ASSERT_EQUAL(0, LoRaWAN::downlinkFrameCount.asUint32);
    TEST_ASSERT_EQUAL(1, LoRaWAN::rx1DelayInSeconds);
    TEST_ASSERT_EQUAL(0, LoRaWAN::DevAddr.asUint32);
    // TODO : complete for all context

    // Populate non-volatile memory with some test data
    settingsCollection::save(settingsCollection::settingIndex::uplinkFrameCounter, 0x1234);
    settingsCollection::save(settingsCollection::settingIndex::downlinkFrameCounter, 0x5678);
    settingsCollection::save(settingsCollection::settingIndex::rx1Delay, 0x05);
    settingsCollection::save(settingsCollection::settingIndex::DevAddr, 0xABCD);

    LoRaWAN::initialize();

    // Check if all context is properly restored
    TEST_ASSERT_EQUAL(0x1234, LoRaWAN::uplinkFrameCount.asUint32);
    TEST_ASSERT_EQUAL(0x5678, LoRaWAN::downlinkFrameCount.asUint32);
    TEST_ASSERT_EQUAL(0x5, LoRaWAN::rx1DelayInSeconds);
    TEST_ASSERT_EQUAL(0xABCD, LoRaWAN::DevAddr.asUint32);
    // TODO : complete for all context

    uint8_t allZeroes[LoRaWAN::rawMessageLength];
    memset(allZeroes, 0, LoRaWAN::rawMessageLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage, LoRaWAN::rawMessageLength);
}

void test_isValidDownlinkFrameCount() {
    LoRaWAN::downlinkFrameCount = 0;
    frameCount testFrameCount(10);
    TEST_ASSERT_TRUE(LoRaWAN::isValidDownlinkFrameCount(testFrameCount));
    LoRaWAN::downlinkFrameCount = 9;
    TEST_ASSERT_TRUE(LoRaWAN::isValidDownlinkFrameCount(testFrameCount));
    LoRaWAN::downlinkFrameCount = 10;
    TEST_ASSERT_FALSE(LoRaWAN::isValidDownlinkFrameCount(testFrameCount));
    LoRaWAN::downlinkFrameCount = 11;
    TEST_ASSERT_FALSE(LoRaWAN::isValidDownlinkFrameCount(testFrameCount));
}

void test_dump() {
    LoRaWAN::dump();
    TEST_MESSAGE("For Coverage only");
}

int main(int argc, char **argv) {
#ifndef generic
// Here we could setup the STM32 for target unit testing
#endif
    UNITY_BEGIN();

    RUN_TEST(test_initialize);
    RUN_TEST(test_isValidDownlinkFrameCount);
    RUN_TEST(test_dump);

    UNITY_END();
}