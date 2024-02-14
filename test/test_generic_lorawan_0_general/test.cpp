#include <unity.h>
#include <lorawan.hpp>
#include <circularbuffer.hpp>
#include <lorawanevent.hpp>
#include <applicationevent.hpp>
#include <settingscollection.hpp>
#include <maccommand.hpp>
#include <hexascii.hpp>
#include <txchannelcollection.hpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;
circularBuffer<loRaWanEvent, 16U> loraWanEventBuffer;

uint8_t mockSX126xDataBuffer[256];
uint8_t mockSX126xRegisters[0x1000];
uint8_t mockSX126xCommandData[256][8];

extern uint8_t mockEepromMemory[nonVolatileStorage::size];

void setUp(void) {}
void tearDown(void) {}

void test_initialize() {
    TEST_ASSERT_EQUAL(0, LoRaWAN::uplinkFrameCount.asUint32);
    TEST_ASSERT_EQUAL(0, LoRaWAN::downlinkFrameCount.asUint32);
    TEST_ASSERT_EQUAL(1, LoRaWAN::rx1DelayInSeconds);
    TEST_ASSERT_EQUAL(0, LoRaWAN::DevAddr.asUint32);

    LoRaWAN::txChannels.channel[0].frequencyInHz = 868'100'000U;

    TEST_ASSERT_EQUAL(868'100'000U, LoRaWAN::txChannels.channel[0].frequencyInHz);
    TEST_ASSERT_EQUAL(868'300'000U, LoRaWAN::txChannels.channel[1].frequencyInHz);
    TEST_ASSERT_EQUAL(868'500'000U, LoRaWAN::txChannels.channel[2].frequencyInHz);
    TEST_ASSERT_EQUAL(0U, LoRaWAN::txChannels.channel[3].frequencyInHz);
    TEST_ASSERT_EQUAL(0U, LoRaWAN::txChannels.channel[4].frequencyInHz);
    //TEST_ASSERT_EQUAL(869'525'000U, LoRaWAN::theChannels.rx2Channel.frequencyInHz);

    // TODO : complete for all context

    settingsCollection::save(0x1234, settingsCollection::settingIndex::uplinkFrameCounter);
    settingsCollection::save(0x5678, settingsCollection::settingIndex::downlinkFrameCounter);
    settingsCollection::save(0x05, settingsCollection::settingIndex::rx1Delay);
    settingsCollection::save(0xABCD, settingsCollection::settingIndex::DevAddr);

    LoRaWAN::initialize();

    TEST_ASSERT_EQUAL(0x1234, LoRaWAN::uplinkFrameCount.asUint32);
    TEST_ASSERT_EQUAL(0x5678, LoRaWAN::downlinkFrameCount.asUint32);
    TEST_ASSERT_EQUAL(0x5, LoRaWAN::rx1DelayInSeconds);
    TEST_ASSERT_EQUAL(0xABCD, LoRaWAN::DevAddr.asUint32);

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
    LoRaWAN::dumpConfig();
    LoRaWAN::dumpState();
    LoRaWAN::dumpChannels();

    TEST_MESSAGE("For Coverage only");
}

void test_saveContext() {
    memset(mockEepromMemory, 0x00, 512);

    LoRaWAN::DevAddr              = 0x44657641;        // reads as DevA in memory
    LoRaWAN::uplinkFrameCount     = 0x554C4643;        // reads as ULFC in memory
    LoRaWAN::downlinkFrameCount   = 0x444C4643;        // reads as DLFC in memory
    LoRaWAN::rx1DelayInSeconds    = 7;
    LoRaWAN::currentDataRateIndex = 3;
    LoRaWAN::applicationKey.setFromHexString("000102030405060708090A0B0C0D0E0F");
    LoRaWAN::networkKey.setFromHexString("101112131415161718191A1B1C1D1E1F");

    // TODO : add channel data

    LoRaWAN::saveConfig();
    LoRaWAN::saveState();
    LoRaWAN::saveChannels();

    LoRaWAN::DevAddr              = 0;
    LoRaWAN::uplinkFrameCount     = 0;
    LoRaWAN::downlinkFrameCount   = 0;
    LoRaWAN::rx1DelayInSeconds    = 0;
    LoRaWAN::currentDataRateIndex = 0;
    LoRaWAN::applicationKey.setFromHexString("00000000000000000000000000000000");
    LoRaWAN::networkKey.setFromHexString("00000000000000000000000000000000");

    LoRaWAN::restoreConfig();
    LoRaWAN::restoreState();
    LoRaWAN::restoreChannels();

    TEST_ASSERT_EQUAL_UINT32(0x44657641, LoRaWAN::DevAddr.asUint32);
    TEST_ASSERT_EQUAL_UINT32(0x554C4643, LoRaWAN::uplinkFrameCount.toUint32());
    TEST_ASSERT_EQUAL_UINT32(0x444C4643, LoRaWAN::downlinkFrameCount.toUint32());
    TEST_ASSERT_EQUAL_UINT32(7, LoRaWAN::rx1DelayInSeconds);
    TEST_ASSERT_EQUAL_UINT32(3, LoRaWAN::currentDataRateIndex);
    char tmpKeyAsHexString[33];
    hexAscii::byteArrayToHexString(tmpKeyAsHexString, LoRaWAN::applicationKey.asBytes(), 16);
    TEST_ASSERT_EQUAL_STRING("000102030405060708090A0B0C0D0E0F", tmpKeyAsHexString);
    hexAscii::byteArrayToHexString(tmpKeyAsHexString, LoRaWAN::networkKey.asBytes(), 16);
    TEST_ASSERT_EQUAL_STRING("101112131415161718191A1B1C1D1E1F", tmpKeyAsHexString);
}

int main(int argc, char **argv) {
#ifndef generic
// Here we could setup the STM32 for target unit testing
#endif
    UNITY_BEGIN();

    RUN_TEST(test_initialize);
    RUN_TEST(test_isValidDownlinkFrameCount);
    RUN_TEST(test_dump);
    RUN_TEST(test_saveContext);

    UNITY_END();
}