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
#include <sx126x.hpp>
#include <mcutype.hpp>
#include <maincontroller.hpp>
#include <uniqueid.hpp>
#include <i2c.hpp>

// #######################################################
// ###  Which settings to be written to EEPROM         ###
// #######################################################

const bool resetBatteryType{false};
const bool resetMcuType{false};
const bool setName{true};
const bool fixDevAddr{false};
const bool overwriteExistingLoRaWANConfig{false};
const bool resetLoRaWANState{false};
const bool resetLoRaWANChannels{false};
const bool eraseMeasurementsInEeprom{false};

// #######################################################
// ###  Non-Volatile settings to be written to EEPROM  ###
// #######################################################

eepromType selectedEepromType{eepromType::M24M01E};
uint8_t selectedDisplayType{0};
batteryType selectedBatteryType{batteryType::alkaline_1200mAh};
mcuType selectedPowerVersion{mcuType::highPower};

const char toBeName[9] = "TTC";

uint32_t toBeDevAddr            = 0x260BA90C;
const char toBeNetworkKey[]     = "00DC8F01A6661926B84EDE29C273330E";
const char toBeApplicationKey[] = "DBA40D12067C5DBB684F883312F02A31";

// #######################################################

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void showUid() {
    uint64_t uid = uniqueId::get();
    char output[17];
    hexAscii::uint64ToHexString(output, uid);
    TEST_MESSAGE(output);
}

void initializeNvsVersion() {
    settingsCollection::save(static_cast<uint8_t>(1U), settingsCollection::settingIndex::nvsMapVersion);
    TEST_ASSERT_EQUAL_UINT8(1, settingsCollection::read<uint8_t>(settingsCollection::settingIndex::nvsMapVersion));
}

void initializeDisplayType() {
    settingsCollection::save(selectedDisplayType, settingsCollection::settingIndex::displayType);
    TEST_ASSERT_EQUAL_UINT8(selectedDisplayType, settingsCollection::read<uint8_t>(settingsCollection::settingIndex::displayType));
}

void initializeBatteryType() {
    if (resetBatteryType) {
        settingsCollection::save(static_cast<uint8_t>(selectedBatteryType), settingsCollection::settingIndex::batteryType);
        TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(selectedBatteryType), settingsCollection::read<uint8_t>(settingsCollection::settingIndex::batteryType));
    }
}

void initializeEepromType() {
    settingsCollection::save(static_cast<uint8_t>(selectedEepromType), settingsCollection::settingIndex::eepromType);
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(selectedEepromType), settingsCollection::read<uint8_t>(settingsCollection::settingIndex::eepromType));
}

void initializeMcuType() {
    if (resetMcuType) {
        settingsCollection::save(static_cast<uint8_t>(selectedPowerVersion), settingsCollection::settingIndex::mcuType);
        TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(selectedPowerVersion), settingsCollection::read<uint8_t>(settingsCollection::settingIndex::mcuType));
    }
}

void initializeName() {
    if (setName) {
        uint8_t newName[9]{0};
        for (uint32_t index = 0; index < strlen(toBeName); index++) {
            newName[index] = toBeName[index];
        }
        settingsCollection::saveByteArray(newName, settingsCollection::settingIndex::name);
        uint8_t isName[9]{0};
        settingsCollection::readByteArray(isName, settingsCollection::settingIndex::name);
        TEST_ASSERT_EQUAL_STRING(newName, isName);
    }
}

void fixDevAddrEndianess() {
    if (fixDevAddr) {
        LoRaWAN::restoreConfig();
        LoRaWAN::correctDevAddrEndianness();
    }
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
        hexAscii::hexStringToByteArray(test, toBeApplicationKey, 32);
        TEST_ASSERT_EQUAL_UINT8_ARRAY(test, LoRaWAN::applicationKey.asBytes(), 16);
        hexAscii::hexStringToByteArray(test, toBeNetworkKey, 32);
        TEST_ASSERT_EQUAL_UINT8_ARRAY(test, LoRaWAN::networkKey.asBytes(), 16);
    } else {
        TEST_ASSERT_NOT_EQUAL_UINT32(0xFFFFFFFF, LoRaWAN::DevAddr.asUint32);
        uint8_t test[16]{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        TEST_ASSERT_NOT_EQUAL(0, memcmp(test, LoRaWAN::applicationKey.asBytes(), 16));
        TEST_ASSERT_NOT_EQUAL(0, memcmp(test, LoRaWAN::networkKey.asBytes(), 16));
    }
}

void initializeLorawanState() {
    if (resetLoRaWANState) {
        LoRaWAN::currentDataRateIndex = 5;
        LoRaWAN::saveState();
        LoRaWAN::restoreState();

        TEST_ASSERT_EQUAL_UINT32(5, LoRaWAN::currentDataRateIndex);
        TEST_ASSERT_EQUAL_UINT32(0, LoRaWAN::rx1DataRateOffset);
        TEST_ASSERT_EQUAL_UINT32(3, LoRaWAN::rx2DataRateIndex);
        TEST_ASSERT_EQUAL_UINT32(1, LoRaWAN::rx1DelayInSeconds);
        TEST_ASSERT_EQUAL_UINT32(1, LoRaWAN::uplinkFrameCount.asUint32);
        TEST_ASSERT_EQUAL_UINT32(0, LoRaWAN::downlinkFrameCount.asUint32);
    }
}

void initializeLorawanChannels() {
    if (resetLoRaWANChannels) {
        LoRaWAN::saveChannels();
        LoRaWAN::restoreChannels();

        // TODO : make this config dependent on the setting of the resetLoRaWANStateAndChannels flag

        TEST_ASSERT_EQUAL_UINT32(868'100'000U, loRaTxChannelCollection::channel[0].frequencyInHz);
        TEST_ASSERT_EQUAL_UINT32(868'300'000U, loRaTxChannelCollection::channel[1].frequencyInHz);
        TEST_ASSERT_EQUAL_UINT32(868'500'000U, loRaTxChannelCollection::channel[2].frequencyInHz);

        for (auto index = 3U; index < loRaTxChannelCollection::maxNmbrChannels; index++) {
            TEST_ASSERT_EQUAL_UINT32(0U, loRaTxChannelCollection::channel[index].frequencyInHz);
        }
        TEST_ASSERT_EQUAL(869525000U, LoRaWAN::rx2FrequencyInHz);
    }
}

void test_eraseMeasurementsInEeprom() {
    if (eraseMeasurementsInEeprom) {
        measurementCollection::eraseAll();
    }
}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(3000);
    SystemClock_Config();
    i2c::wakeUp();

    UNITY_BEGIN();
    RUN_TEST(showUid);    
    RUN_TEST(initializeNvsVersion);
    RUN_TEST(initializeDisplayType);
    RUN_TEST(initializeBatteryType);
    RUN_TEST(initializeEepromType);
    RUN_TEST(initializeActiveLoggingSources);
    RUN_TEST(initializeMcuType);
    RUN_TEST(initializeName);
    RUN_TEST(initializeLorawanConfig);
    RUN_TEST(initializeLorawanState);
    RUN_TEST(initializeLorawanChannels);
    RUN_TEST(test_eraseMeasurementsInEeprom);
    UNITY_END();
}
