#include "settingscollection.h"
#include "nvs.h"

const setting settingsCollection::settings[static_cast<uint32_t>(settingIndex::numberOfSettings)] = {
    // Important note : make sure that none of the settings are mapped into two pages of 128 Bytes, as the page-write of the EEPROM is limited to 128 Byte pages and the address will wrap around to the beginning of the page if addressing more than 128 Bytes
    {0, 1},          // nvsMapVersion : 1 byte
    {1, 1},          // displayVersion : 1 byte
    {2, 1},          // batteryVersion : 1 byte
    {3, 4},          // activelogging::sources : 4 bytes
    {7, 121},        // unusedGeneral : extra settings can be inserted hereafter 

    {128, 4},        // measurementWriteIndex : 32 bits
    {132, 4},        // oldestUnsentMeasurementIndex : 32 bits
    {136, 4},        // oldestUnconfirmedMeasurementindex : 32 bits
    // TODO add a setting to enable resending unconfirmed measurements
    {140, 116},        // unusedMeasurementDataManagement : extra settings can be inserted hereafter for a maximum of 116 bytes

    {256, 8},          // DevEUI : 64 bits
    {264, 4},          // DevAddr : 32 bits
    {268, 4},          // uplinkFrameCounter : 32 bits
    {272, 4},          // downlinkFrameCounter : 32 bits
    {276, 16},         // applicationSessionKey : 128 bits
    {292, 16},         // networkSessionKey : 128 bits
    {308, 1},          // rx1Delay : 1 byte
    {309, 203},        // unusedLoRaWAN : extra settings can be inserted hereafter
};

bool settingsCollection::isInitialized() {
    uint8_t currentSettingscollectionVersion{0};
    currentSettingscollectionVersion = read<uint8_t>(settingsCollection::settingIndex::nvsMapVersion);
    return (0xFF != currentSettingscollectionVersion);
}

void settingsCollection::initializeOnce() {
    save<uint8_t>(settingsCollection::settingIndex::nvsMapVersion, 0x01);

    save<uint32_t>(settingsCollection::settingIndex::measurementWriteIndex, 0);
    save<uint32_t>(settingsCollection::settingIndex::oldestUnsentMeasurementIndex, 0);
    save<uint32_t>(settingsCollection::settingIndex::oldestUnconfirmedMeasurementindex, 0);
    save<uint32_t>(settingsCollection::settingIndex::unusedMeasurementDataManagement, 0);

    save<uint32_t>(settingsCollection::settingIndex::DevAddr, 0);
    save<uint32_t>(settingsCollection::settingIndex::uplinkFrameCounter, 0);
    save<uint32_t>(settingsCollection::settingIndex::downlinkFrameCounter, 0);
    save<uint8_t>(settingsCollection::settingIndex::rx1Delay, 0);

    uint8_t data[16]{0};
    settingsCollection::save(settingsCollection::settingIndex::applicationSessionKey, data);
    settingsCollection::save(settingsCollection::settingIndex::networkSessionKey, data);
}

void settingsCollection::save(settingIndex theIndex, const uint8_t* dataIn) {
    if (settingsCollection::isValidIndex(theIndex)) {
        uint32_t startAddress = settingsCollection::settings[static_cast<uint32_t>(theIndex)].startAddress;
        uint32_t length       = settingsCollection::settings[static_cast<uint32_t>(theIndex)].length;
        nonVolatileStorage::write(startAddress, dataIn, length);
    }
}
void settingsCollection::read(settingIndex theIndex, uint8_t* dataOut) {
    if (settingsCollection::isValidIndex(theIndex)) {
        uint32_t startAddress = settingsCollection::settings[static_cast<uint32_t>(theIndex)].startAddress;
        uint32_t length       = settingsCollection::settings[static_cast<uint32_t>(theIndex)].length;
        nonVolatileStorage::read(startAddress, dataOut, length);
    }
}
