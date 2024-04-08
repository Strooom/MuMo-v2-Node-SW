#include <settingscollection.hpp>
#include <nvs.hpp>

const setting settingsCollection::settings[static_cast<uint32_t>(settingIndex::numberOfSettings)] = {
    // Important note : make sure that none of the settings are mapped into two pages of 128 Bytes, as the page-write of the EEPROM is limited to 128 Byte pages and the address will wrap around to the beginning of the page if addressing more than 128 Bytes. A unit test will check this

    {0, 1},               // nvsMapVersion : 1 byte
    {1, 1},               // displayType : 1 byte
    {2, 1},               // batteryType : 1 byte
    {3, 1},               // eepromType : 1 byte
    {4, 60},              // unusedGeneral : extra hardware settings can be inserted hereafter
    {64, 4},              // activelogging::sources : 4 bytes
    {68, 60},             // unusedGeneral : extra settings can be inserted hereafter

    {128, 128},           // unusedMeasurements : 128 bytes

    {256, 8},             // DevEUI : 64 bits
    {264, 4},             // DevAddr : 32 bits
    {268, 4},             // uplinkFrameCounter : 32 bits
    {272, 4},             // downlinkFrameCounter : 32 bits
    {276, 16},            // applicationSessionKey : 128 bits
    {292, 16},            // networkSessionKey : 128 bits
    {308, 1},             // rx1Delay : 1 byte
    {309, 1},             // dataRate : 1 byte
    {310, 1},             // rx1DataRateOffset : 1 byte
    {311, 1},             // rx2DataRateIndex : 1 byte
    {312, 72},            // unusedLoRaWAN : extra settings can be inserted herea

    {384, 16 * 6},        // 16 Tx channels : 6 bytes / channel [frequency, minDR, maxDR]
    {480, 6},             // Rx channel : [frequency, rx1DataRateOffset, rx2DataRateIndex]
    {486, 26},            // unusedLoRaWAN : extra settings can be inserted hereafter

    // TODO : add the settings for the known sensors here
};

bool settingsCollection::isInitialized() {
    uint8_t settingsCollectionVersion = read<uint8_t>(settingIndex::nvsMapVersion);
    return (settingsCollectionVersion != nonVolatileStorage::blankEepromValue);
}

void settingsCollection::initializeOnce() {
    save<uint8_t>(0x01, settingsCollection::settingIndex::nvsMapVersion);

    save<uint32_t>(0, settingsCollection::settingIndex::DevAddr);
    save<uint32_t>(0, settingsCollection::settingIndex::uplinkFrameCounter);
    save<uint32_t>(0, settingsCollection::settingIndex::downlinkFrameCounter);
    save<uint8_t>(0, settingsCollection::settingIndex::rx1Delay);

    uint8_t data[16]{0};
    settingsCollection::save(data, settingIndex::applicationSessionKey);
    settingsCollection::save(data, settingIndex::networkSessionKey);
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

// void settingsCollection::dump() {
//     if ((!logging::isActive()) || (!logging::isActive(logging::source::eepromData))) {
//         return;
//     }
//     logging::snprintf("EEPROM dump");
//     uint8_t settingsCollectionVersion = read<uint8_t>(settingIndex::nvsMapVersion);
//     switch (settingsCollectionVersion) {
//         case nonVolatileStorage::blankEepromValue:
//             logging::snprintf("EEPROM is blank");
//             return;
//             break;
//         default:
//             logging::snprintf("%d : nvsMapVersion = %d", settings[static_cast<uint32_t>(settingIndex::nvsMapVersion)].startAddress, settingsCollectionVersion);
//             break;
//     }
//     logging::snprintf("%d : displayType = %d", settings[static_cast<uint32_t>(settingIndex::displayType)].startAddress, read<uint8_t>(settingIndex::displayType));
//     logging::snprintf("%d : batteryType = %d", settings[static_cast<uint32_t>(settingIndex::batteryType)].startAddress, read<uint8_t>(settingIndex::batteryType));
//     logging::snprintf("%d : activeLoggingSources = %d", settings[static_cast<uint32_t>(settingIndex::activeLoggingSources)].startAddress, read<uint32_t>(settingIndex::activeLoggingSources));
// }