#include <unity.h>
#include <lorawan.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <settingscollection.hpp>
#include <maccommand.hpp>
#include <hexascii.hpp>
#include <txchannelcollection.hpp>
#include <logging.hpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

extern uint8_t mockSX126xDataBuffer[256];
extern uint8_t mockSX126xRegisters[0x1000];
extern uint8_t mockSX126xCommandData[256][8];

extern uint8_t mockEepromMemory[nonVolatileStorage::totalSize];

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_initialize() {
    uint8_t allZeroes[LoRaWAN::rawMessageLength];
    memset(allZeroes, 0, LoRaWAN::rawMessageLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage, LoRaWAN::rawMessageLength);
}

void test_initialize_config() {
    TEST_ASSERT_EQUAL(0, LoRaWAN::DevAddr.asUint32);
    TEST_ASSERT_EQUAL(1, LoRaWAN::rx1DelayInSeconds);
    char tmpKeyAsHexString[33];
    hexAscii::byteArrayToHexString(tmpKeyAsHexString, LoRaWAN::applicationKey.asBytes(), 16);
    TEST_ASSERT_EQUAL_STRING("00000000000000000000000000000000", tmpKeyAsHexString);
    hexAscii::byteArrayToHexString(tmpKeyAsHexString, LoRaWAN::networkKey.asBytes(), 16);
    TEST_ASSERT_EQUAL_STRING("00000000000000000000000000000000", tmpKeyAsHexString);
}

void test_initialize_state() {
    TEST_ASSERT_EQUAL(1, LoRaWAN::uplinkFrameCount.asUint32);
    TEST_ASSERT_EQUAL(0, LoRaWAN::downlinkFrameCount.asUint32);
    TEST_ASSERT_EQUAL(5, LoRaWAN::currentDataRateIndex);
    TEST_ASSERT_EQUAL(3, LoRaWAN::rx2DataRateIndex);
    TEST_ASSERT_EQUAL(0, LoRaWAN::rx1DataRateOffset);
}

void test_initialize_channels() {
    TEST_ASSERT_EQUAL(868'100'000U, LoRaWAN::txChannels.channel[0].frequencyInHz);
    TEST_ASSERT_EQUAL(868'300'000U, LoRaWAN::txChannels.channel[1].frequencyInHz);
    TEST_ASSERT_EQUAL(868'500'000U, LoRaWAN::txChannels.channel[2].frequencyInHz);
    TEST_ASSERT_EQUAL(0U, LoRaWAN::txChannels.channel[3].frequencyInHz);
    TEST_ASSERT_EQUAL(0U, LoRaWAN::txChannels.channel[4].frequencyInHz);
    TEST_ASSERT_EQUAL(0U, LoRaWAN::txChannels.channel[5].frequencyInHz);
    TEST_ASSERT_EQUAL(0U, LoRaWAN::txChannels.channel[6].frequencyInHz);
    TEST_ASSERT_EQUAL(0U, LoRaWAN::txChannels.channel[7].frequencyInHz);
    TEST_ASSERT_EQUAL(0U, LoRaWAN::txChannels.channel[8].frequencyInHz);
    TEST_ASSERT_EQUAL(869'525'000U, LoRaWAN::rx2FrequencyInHz);
}

void test_save_restore_config() {
    LoRaWAN::DevAddr = 0x1234;
    LoRaWAN::applicationKey.setFromHexString("000102030405060708090A0B0C0D0E0F");
    LoRaWAN::networkKey.setFromHexString("101112131415161718191A1B1C1D1E1F");
    LoRaWAN::saveConfig();
    LoRaWAN::DevAddr = 0x0;
    LoRaWAN::applicationKey.setFromHexString("00000000000000000000000000000000");
    LoRaWAN::networkKey.setFromHexString("00000000000000000000000000000000");
    LoRaWAN::restoreConfig();
    TEST_ASSERT_EQUAL(0x1234, LoRaWAN::DevAddr.asUint32);
    char tmpKeyAsHexString[33];
    hexAscii::byteArrayToHexString(tmpKeyAsHexString, LoRaWAN::applicationKey.asBytes(), 16);
    TEST_ASSERT_EQUAL_STRING("000102030405060708090A0B0C0D0E0F", tmpKeyAsHexString);
    hexAscii::byteArrayToHexString(tmpKeyAsHexString, LoRaWAN::networkKey.asBytes(), 16);
    TEST_ASSERT_EQUAL_STRING("101112131415161718191A1B1C1D1E1F", tmpKeyAsHexString);
}

void test_save_restore_state() {
    LoRaWAN::rx1DelayInSeconds    = 0x05;
    LoRaWAN::uplinkFrameCount     = 0x1234;
    LoRaWAN::downlinkFrameCount   = 0x5678;
    LoRaWAN::currentDataRateIndex = 0x02;
    LoRaWAN::rx2DataRateIndex     = 0x03;
    LoRaWAN::rx1DataRateOffset    = 0x04;
    LoRaWAN::saveState();
    LoRaWAN::rx1DelayInSeconds    = 0x0;
    LoRaWAN::uplinkFrameCount     = 0;
    LoRaWAN::downlinkFrameCount   = 0;
    LoRaWAN::currentDataRateIndex = 0;
    LoRaWAN::rx2DataRateIndex     = 0;
    LoRaWAN::rx1DataRateOffset    = 0;
    LoRaWAN::restoreState();
    TEST_ASSERT_EQUAL(0x05, LoRaWAN::rx1DelayInSeconds);
    TEST_ASSERT_EQUAL(0x1234, LoRaWAN::uplinkFrameCount.asUint32);
    TEST_ASSERT_EQUAL(0x5678, LoRaWAN::downlinkFrameCount.asUint32);
    TEST_ASSERT_EQUAL(0x02, LoRaWAN::currentDataRateIndex);
    TEST_ASSERT_EQUAL(0x03, LoRaWAN::rx2DataRateIndex);
    TEST_ASSERT_EQUAL(0x04, LoRaWAN::rx1DataRateOffset);
}

void test_save_restore_channels() {
    for (uint32_t channelIndex = 0; channelIndex < loRaTxChannelCollection::maxNmbrChannels; channelIndex++) {
        LoRaWAN::txChannels.channel[channelIndex].frequencyInHz = channelIndex * 100U;
    }
    LoRaWAN::rx2FrequencyInHz = 0x12345678;
    LoRaWAN::saveChannels();
    for (uint32_t channelIndex = 0; channelIndex < loRaTxChannelCollection::maxNmbrChannels; channelIndex++) {
        LoRaWAN::txChannels.channel[channelIndex].frequencyInHz = 0;
    }
    LoRaWAN::rx2FrequencyInHz = 0;
    LoRaWAN::restoreChannels();

    for (uint32_t channelIndex = 0; channelIndex < loRaTxChannelCollection::maxNmbrChannels; channelIndex++) {
        TEST_ASSERT_EQUAL(channelIndex * 100U, LoRaWAN::txChannels.channel[channelIndex].frequencyInHz);
    }
    TEST_ASSERT_EQUAL(0x12345678, LoRaWAN::rx2FrequencyInHz);
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
    logging::enable(logging::destination::uart2usb);
    logging::enable(logging::source::lorawanData);
    logging::enable(logging::source::lorawanEvents);
    logging::enable(logging::source::lorawanMac);
    LoRaWAN::dumpConfig();
    LoRaWAN::dumpState();
    LoRaWAN::dumpChannels();

    TEST_MESSAGE("For Coverage only");
}

// void test_saveContext() {
//     memset(mockEepromMemory, 0x00, 512);

//     LoRaWAN::DevAddr              = 0x44657641;        // reads as DevA in memory
//     LoRaWAN::uplinkFrameCount     = 0x554C4643;        // reads as ULFC in memory
//     LoRaWAN::downlinkFrameCount   = 0x444C4643;        // reads as DLFC in memory
//     LoRaWAN::rx1DelayInSeconds    = 7;
//     LoRaWAN::currentDataRateIndex = 3;
//     LoRaWAN::applicationKey.setFromHexString("000102030405060708090A0B0C0D0E0F");
//     LoRaWAN::networkKey.setFromHexString("101112131415161718191A1B1C1D1E1F");

//     LoRaWAN::saveConfig();
//     LoRaWAN::saveState();
//     LoRaWAN::saveChannels();

//     LoRaWAN::DevAddr              = 0;
//     LoRaWAN::uplinkFrameCount     = 0;
//     LoRaWAN::downlinkFrameCount   = 0;
//     LoRaWAN::rx1DelayInSeconds    = 0;
//     LoRaWAN::currentDataRateIndex = 0;
//     LoRaWAN::applicationKey.setFromHexString("00000000000000000000000000000000");
//     LoRaWAN::networkKey.setFromHexString("00000000000000000000000000000000");

//     LoRaWAN::restoreConfig();
//     LoRaWAN::restoreState();
//     LoRaWAN::restoreChannels();

//     TEST_ASSERT_EQUAL_UINT32(0x44657641, LoRaWAN::DevAddr.asUint32);
//     TEST_ASSERT_EQUAL_UINT32(0x554C4643, LoRaWAN::uplinkFrameCount.toUint32());
//     TEST_ASSERT_EQUAL_UINT32(0x444C4643, LoRaWAN::downlinkFrameCount.toUint32());
//     TEST_ASSERT_EQUAL_UINT32(7, LoRaWAN::rx1DelayInSeconds);
//     TEST_ASSERT_EQUAL_UINT32(3, LoRaWAN::currentDataRateIndex);
//     char tmpKeyAsHexString[33];
//     hexAscii::byteArrayToHexString(tmpKeyAsHexString, LoRaWAN::applicationKey.asBytes(), 16);
//     TEST_ASSERT_EQUAL_STRING("000102030405060708090A0B0C0D0E0F", tmpKeyAsHexString);
//     hexAscii::byteArrayToHexString(tmpKeyAsHexString, LoRaWAN::networkKey.asBytes(), 16);
//     TEST_ASSERT_EQUAL_STRING("101112131415161718191A1B1C1D1E1F", tmpKeyAsHexString);
// }

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_initialize);
    RUN_TEST(test_initialize_config);
    RUN_TEST(test_initialize_state);
    RUN_TEST(test_initialize_channels);
    RUN_TEST(test_save_restore_config);
    RUN_TEST(test_save_restore_state);
    RUN_TEST(test_save_restore_channels);
    RUN_TEST(test_isValidDownlinkFrameCount);
    RUN_TEST(test_dump);
    // RUN_TEST(test_saveContext);

    UNITY_END();
}