#include <settingscollection.hpp>
#include <nvs.hpp>
#include <cstring>

const setting settingsCollection::settings[static_cast<uint32_t>(settingIndex::numberOfSettings)] = {
    // Important note : make sure that none of the settings are mapped into two pages of 128 Bytes, as the page-write of the EEPROM is limited to 128 Byte pages and the address will wrap around to the beginning of the page if addressing more than 128 Bytes. A unit test will check this

    {0, 1},        // mapVersion : 1 byte
    {1, 1},        // displayType : 1 byte
    {2, 1},        // batteryType : 1 byte
    {3, 1},        // eepromType : 1 byte
    {4, 1},        // radioType : 1 byte

    {5, 59},         // unusedGeneral : extra hardware settings can be inserted hereafter
    {64, 4},         // activelogging::sources : 4 bytes
    {68, 8},         // nodeName
    {76, 3},         // displaySettings : 1 byte per line for deviceIndex and channelIndex, 3 lines
    {79, 49},        // unusedGeneral : extra settings can be inserted hereafter

    {128, 4},          // oldestMeasurementOffset : 4 bytes
    {132, 4},          // newMeasurementsOffset : 4 bytes
    {136, 120},        // unusedMeasurements : 120 bytes

    {256, 8},         // DevEUI : 64 bits
    {264, 4},         // DevAddr : 32 bits
    {268, 4},         // uplinkFrameCounter : 32 bits
    {272, 4},         // downlinkFrameCounter : 32 bits
    {276, 16},        // applicationSessionKey : 128 bits
    {292, 16},        // networkSessionKey : 128 bits
    {308, 1},         // rx1Delay : 1 byte
    {309, 1},         // dataRate : 1 byte
    {310, 1},         // rx1DataRateOffset : 1 byte
    {311, 1},         // rx2DataRateIndex : 1 byte
    {312, 72},        // unusedLoRaWAN : extra settings can be inserted here

    {384, 16 * 6},        // 16 Tx channels : 6 bytes / channel [frequency, minDR, maxDR]
    {480, 6},             // Rx channel : [frequency, rx1DataRateOffset, rx2DataRateIndex]
    {486, 26},            // unusedLoRaWAN : extra settings can be inserted hereafter

    {512, 256},        // sensorSettings : 256 combinations of sensorDeviceIndex and sensorChannelIndex, 1 byte  (6 bits) per channel : 2 bits for oversamplingIndex, 4 bits for prescalingIndex
};

bool settingsCollection::isValid() {
    uint8_t settingsCollectionVersion = read<uint8_t>(settingIndex::mapVersion);
    return ((settingsCollectionVersion <= maxMapVersion) && (settingsCollectionVersion > 0));
}

uint32_t settingsCollection::getMapVersion() {
    return read<uint8_t>(settingIndex::mapVersion);
}

void settingsCollection::saveByteArray(const uint8_t* dataIn, settingIndex theIndex) {
    if (settingsCollection::isValidIndex(theIndex)) {
        uint32_t startAddress = settings[static_cast<uint32_t>(theIndex)].startAddress;
        uint32_t length       = settings[static_cast<uint32_t>(theIndex)].length;
        nonVolatileStorage::write(startAddress, dataIn, length);
    }
}
void settingsCollection::readByteArray(uint8_t* dataOut, settingIndex theIndex) {
    if (settingsCollection::isValidIndex(theIndex)) {
        uint32_t startAddress = settings[static_cast<uint32_t>(theIndex)].startAddress;
        uint32_t length       = settings[static_cast<uint32_t>(theIndex)].length;
        nonVolatileStorage::read(startAddress, dataOut, length);
    }
}

void settingsCollection::save(const uint8_t data, settingIndex theIndex, uint32_t offset) {
    uint32_t address = settings[static_cast<uint32_t>(theIndex)].startAddress + offset;
    nonVolatileStorage::write(address, data);
}
uint8_t settingsCollection::read(settingIndex theIndex, uint32_t offset) {
    uint32_t address = settings[static_cast<uint32_t>(theIndex)].startAddress + offset;
    return nonVolatileStorage::read(address);
}
