#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>
#include <gpio.hpp>
#include <lorawan.hpp>
#include <hexascii.hpp>
#include <settingscollection.hpp>
#include <logging.hpp>
#include <batterytype.hpp>
#include <eepromtype.hpp>
#include <measurementcollection.hpp>

// #######################################################
// ###  Non-Volatile settings to be written to EEPROM  ###
// #######################################################

eepromType selectedEepromType{eepromType::BR24G512};
uint8_t selectedDisplayType{0};
batteryType selectedBatteryType{batteryType::alkaline_1200mAh};

bool overwriteExistingLoRaWANConfig{false};
uint32_t toBeDevAddr            = 0x260BC71B;
const char toBeApplicationKey[] = "ECF61A5B18BFBF81EF4FA7DBA764CE8B";
const char toBeNetworkKey[]     = "34CE07A8DDE81F4C29A0AED7B4F1D7BB";
bool resetLoRaWANStateAndChannels{true};
bool eraseMeasurements{true};

// #######################################################

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {}
void tearDown(void) {}

void initializeNvsVersion() {
    settingsCollection::save(static_cast<uint8_t>(1U), settingsCollection::settingIndex::nvsMapVersion);
    TEST_ASSERT_EQUAL_UINT8(1, settingsCollection::read<uint8_t>(settingsCollection::settingIndex::nvsMapVersion));
}

void initializeDisplayType() {
    settingsCollection::save(selectedDisplayType, settingsCollection::settingIndex::displayType);
    TEST_ASSERT_EQUAL_UINT8(selectedDisplayType, settingsCollection::read<uint8_t>(settingsCollection::settingIndex::displayType));
}

void initializeBatteryType() {
    settingsCollection::save(static_cast<uint8_t>(selectedBatteryType), settingsCollection::settingIndex::batteryType);
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(selectedBatteryType), settingsCollection::read<uint8_t>(settingsCollection::settingIndex::batteryType));
}

void initializeEepromType() {
    settingsCollection::save(static_cast<uint8_t>(selectedEepromType), settingsCollection::settingIndex::eepromType);
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(selectedEepromType), settingsCollection::read<uint8_t>(settingsCollection::settingIndex::eepromType));
}

void initializeActiveLoggingSources() {
    logging::reset();
    logging::enable(logging::source::lorawanMac);
    logging::enable(logging::source::lorawanData);
    logging::enable(logging::source::error);
    logging::enable(logging::source::criticalError);
    uint32_t expectedActiveSources = logging::getActiveSources();
    settingsCollection::save(expectedActiveSources, settingsCollection::settingIndex::activeLoggingSources);
    TEST_ASSERT_EQUAL_UINT32(expectedActiveSources, settingsCollection::read<uint32_t>(settingsCollection::settingIndex::activeLoggingSources));
}

void initializeLorawanConfig() {
    if (overwriteExistingLoRaWANConfig) {
        LoRaWAN::DevAddr.asUint32 = toBeDevAddr;
        LoRaWAN::networkKey.setFromHexString(toBeNetworkKey);
        LoRaWAN::applicationKey.setFromHexString(toBeApplicationKey);
        LoRaWAN::saveConfig();
        LoRaWAN::restoreConfig();

        TEST_ASSERT_EQUAL_UINT32(toBeDevAddr, LoRaWAN::DevAddr.asUint32);
        uint8_t test[16];
        hexAscii::hexStringToByteArray(test, toBeApplicationKey);
        TEST_ASSERT_EQUAL_UINT8_ARRAY(test, LoRaWAN::applicationKey.asBytes(), 16);
        hexAscii::hexStringToByteArray(test, toBeNetworkKey);
        TEST_ASSERT_EQUAL_UINT8_ARRAY(test, LoRaWAN::networkKey.asBytes(), 16);
    } else {
        TEST_ASSERT_NOT_EQUAL_UINT32(0xFFFFFFFF, LoRaWAN::DevAddr.asUint32);
        uint8_t test[16]{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        TEST_ASSERT_NOT_EQUAL(0, memcmp(test, LoRaWAN::applicationKey.asBytes(), 16));
        TEST_ASSERT_NOT_EQUAL(0, memcmp(test, LoRaWAN::networkKey.asBytes(), 16));
    }
}

void initializeLorawanState() {
    LoRaWAN::currentDataRateIndex = 5;
    LoRaWAN::saveState();
    LoRaWAN::restoreState();

    // TODO : make this config dependent on the setting of the resetLoRaWANStateAndChannels flag

    TEST_ASSERT_EQUAL_UINT32(5, LoRaWAN::currentDataRateIndex);
    TEST_ASSERT_EQUAL_UINT32(0, LoRaWAN::rx1DataRateOffset);
    TEST_ASSERT_EQUAL_UINT32(3, LoRaWAN::rx2DataRateIndex);
    TEST_ASSERT_EQUAL_UINT32(1, LoRaWAN::rx1DelayInSeconds);
    TEST_ASSERT_EQUAL_UINT32(1, LoRaWAN::uplinkFrameCount.asUint32);
    TEST_ASSERT_EQUAL_UINT32(0, LoRaWAN::downlinkFrameCount.asUint32);
}

void initializeLorawanChannels() {
    LoRaWAN::saveChannels();
    LoRaWAN::restoreChannels();

    // TODO : make this config dependent on the setting of the resetLoRaWANStateAndChannels flag

    TEST_ASSERT_EQUAL_UINT32(868'100'000U, loRaTxChannelCollection::channel[0].frequencyInHz);
    TEST_ASSERT_EQUAL_UINT32(868'300'000U, loRaTxChannelCollection::channel[1].frequencyInHz);
    TEST_ASSERT_EQUAL_UINT32(868'500'000U, loRaTxChannelCollection::channel[2].frequencyInHz);

    for (auto index = 3U; index < static_cast<uint32_t>(loRaTxChannelCollection::maxNmbrChannels); index++) {
        TEST_ASSERT_EQUAL_UINT32(0U, loRaTxChannelCollection::channel[index].frequencyInHz);
    }
    TEST_ASSERT_EQUAL(869525000U, LoRaWAN::rx2FrequencyInHz);
}

void eraseMeasurementsInEeprom() {
    if (eraseMeasurements) {
        measurementCollection::eraseAll();
    }
}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(2000);
    SystemClock_Config();
    MX_I2C2_Init();
    gpio::enableGpio(gpio::group::i2cEeprom);

    UNITY_BEGIN();
    RUN_TEST(initializeNvsVersion);
    RUN_TEST(initializeDisplayType);
    RUN_TEST(initializeBatteryType);
    RUN_TEST(initializeEepromType);
    RUN_TEST(initializeActiveLoggingSources);
    RUN_TEST(initializeLorawanConfig);
    RUN_TEST(initializeLorawanState);
    RUN_TEST(initializeLorawanChannels);
    RUN_TEST(eraseMeasurementsInEeprom);
    UNITY_END();
}
