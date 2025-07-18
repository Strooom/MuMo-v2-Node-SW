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
    TEST_ASSERT_EQUAL(0, LoRaWAN::DevAddr.getAsWord());
    TEST_ASSERT_EQUAL(1, LoRaWAN::rx1DelayInSeconds);
    TEST_ASSERT_EQUAL_STRING("", LoRaWAN::applicationKey.getAsHexString());
    TEST_ASSERT_EQUAL_STRING("", LoRaWAN::networkKey.getAsHexString());
}

void test_initialize_state() {
    TEST_ASSERT_EQUAL(0, LoRaWAN::uplinkFrameCount.getAsWord());
    TEST_ASSERT_EQUAL(0, LoRaWAN::downlinkFrameCount.getAsWord());
    TEST_ASSERT_EQUAL(5, LoRaWAN::currentDataRateIndex);
    TEST_ASSERT_EQUAL(3, LoRaWAN::rx2DataRateIndex);
    TEST_ASSERT_EQUAL(0, LoRaWAN::rx1DataRateOffset);
}

void test_initialize_channels() {
    TEST_ASSERT_EQUAL(868'100'000U, loRaTxChannelCollection::channel[0].frequencyInHz);
    TEST_ASSERT_EQUAL(868'300'000U, loRaTxChannelCollection::channel[1].frequencyInHz);
    TEST_ASSERT_EQUAL(868'500'000U, loRaTxChannelCollection::channel[2].frequencyInHz);
    TEST_ASSERT_EQUAL(0U, loRaTxChannelCollection::channel[3].frequencyInHz);
    TEST_ASSERT_EQUAL(0U, loRaTxChannelCollection::channel[4].frequencyInHz);
    TEST_ASSERT_EQUAL(0U, loRaTxChannelCollection::channel[5].frequencyInHz);
    TEST_ASSERT_EQUAL(0U, loRaTxChannelCollection::channel[6].frequencyInHz);
    TEST_ASSERT_EQUAL(0U, loRaTxChannelCollection::channel[7].frequencyInHz);
    TEST_ASSERT_EQUAL(0U, loRaTxChannelCollection::channel[8].frequencyInHz);
    TEST_ASSERT_EQUAL(869'525'000U, LoRaWAN::rx2FrequencyInHz);
}

void test_save_restore_config() {
    LoRaWAN::DevAddr.setFromWord(0x1234);
    LoRaWAN::applicationKey.setFromHexString("000102030405060708090A0B0C0D0E0F");
    LoRaWAN::networkKey.setFromHexString("101112131415161718191A1B1C1D1E1F");
    LoRaWAN::saveConfig();
    LoRaWAN::DevAddr.setFromWord(0);
    LoRaWAN::applicationKey.setFromHexString("00000000000000000000000000000000");
    LoRaWAN::networkKey.setFromHexString("00000000000000000000000000000000");
    LoRaWAN::restoreConfig();
    TEST_ASSERT_EQUAL(0x1234, LoRaWAN::DevAddr.getAsWord());
    TEST_ASSERT_EQUAL_STRING("000102030405060708090A0B0C0D0E0F", LoRaWAN::applicationKey.getAsHexString());
    TEST_ASSERT_EQUAL_STRING("101112131415161718191A1B1C1D1E1F", LoRaWAN::networkKey.getAsHexString());
}

void test_save_restore_state() {
    LoRaWAN::rx1DelayInSeconds    = 0x05;
    LoRaWAN::uplinkFrameCount.setFromWord(0x1234);
    LoRaWAN::downlinkFrameCount.setFromWord(0x5678);
    LoRaWAN::currentDataRateIndex = 0x02;
    LoRaWAN::rx2DataRateIndex     = 0x03;
    LoRaWAN::rx1DataRateOffset    = 0x04;
    LoRaWAN::saveState();
    LoRaWAN::rx1DelayInSeconds    = 0x0;
    LoRaWAN::uplinkFrameCount.setFromWord(0);
    LoRaWAN::downlinkFrameCount.setFromWord(0);
    LoRaWAN::currentDataRateIndex = 0;
    LoRaWAN::rx2DataRateIndex     = 0;
    LoRaWAN::rx1DataRateOffset    = 0;
    LoRaWAN::restoreState();
    TEST_ASSERT_EQUAL(0x05, LoRaWAN::rx1DelayInSeconds);
    TEST_ASSERT_EQUAL(0x1234, LoRaWAN::uplinkFrameCount.getAsWord());
    TEST_ASSERT_EQUAL(0x5678, LoRaWAN::downlinkFrameCount.getAsWord());
    TEST_ASSERT_EQUAL(0x02, LoRaWAN::currentDataRateIndex);
    TEST_ASSERT_EQUAL(0x03, LoRaWAN::rx2DataRateIndex);
    TEST_ASSERT_EQUAL(0x04, LoRaWAN::rx1DataRateOffset);
}

void test_save_restore_channels() {
    for (uint32_t channelIndex = 0; channelIndex < loRaTxChannelCollection::maxNmbrChannels; channelIndex++) {
        loRaTxChannelCollection::channel[channelIndex].frequencyInHz = channelIndex * 100U;
    }
    LoRaWAN::rx2FrequencyInHz = 0x12345678;
    LoRaWAN::saveChannels();
    for (uint32_t channelIndex = 0; channelIndex < loRaTxChannelCollection::maxNmbrChannels; channelIndex++) {
        loRaTxChannelCollection::channel[channelIndex].frequencyInHz = 0;
    }
    LoRaWAN::rx2FrequencyInHz = 0;
    LoRaWAN::restoreChannels();

    for (uint32_t channelIndex = 0; channelIndex < loRaTxChannelCollection::maxNmbrChannels; channelIndex++) {
        TEST_ASSERT_EQUAL(channelIndex * 100U, loRaTxChannelCollection::channel[channelIndex].frequencyInHz);
    }
    TEST_ASSERT_EQUAL(0x12345678, LoRaWAN::rx2FrequencyInHz);
}

void test_isValidDownlinkFrameCount() {
    LoRaWAN::downlinkFrameCount.setFromWord(0);
    frameCount testFrameCount;
    testFrameCount.setFromWord(10);
    TEST_ASSERT_TRUE(LoRaWAN::isValidDownlinkFrameCount(testFrameCount.getAsWord()));
    LoRaWAN::downlinkFrameCount.setFromWord(9);
    TEST_ASSERT_TRUE(LoRaWAN::isValidDownlinkFrameCount(testFrameCount.getAsWord()));
    LoRaWAN::downlinkFrameCount.setFromWord(10);
    TEST_ASSERT_FALSE(LoRaWAN::isValidDownlinkFrameCount(testFrameCount.getAsWord()));
    LoRaWAN::downlinkFrameCount.setFromWord(11);
    TEST_ASSERT_FALSE(LoRaWAN::isValidDownlinkFrameCount(testFrameCount.getAsWord()));
}

void test_dump() {
    LoRaWAN::dumpConfig();
    LoRaWAN::dumpState();
    LoRaWAN::dumpChannels();
    logging::enable(logging::destination::uart2);
    logging::enable(logging::source::lorawanData);
    logging::enable(logging::source::lorawanEvents);
    logging::enable(logging::source::lorawanMac);
    LoRaWAN::dumpConfig();
    LoRaWAN::dumpState();
    LoRaWAN::dumpChannels();

    TEST_MESSAGE("For Coverage only");
}


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

    UNITY_END();
}