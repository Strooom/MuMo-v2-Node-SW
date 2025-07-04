#pragma once
#include <stdint.h>
#include "setting.hpp"
#include "nvs.hpp"

class settingsCollection {
  public:
    settingsCollection() = delete;
    enum class settingIndex : uint32_t {
        mapVersion = 0,
        displayType,        // unused for now, as there is only one display type
        batteryType,        // different types of batteries need different voltage to state of charge conversion
        eepromType,         // unused for now, as the different EEPROM types in use are software compatible, except for pagesize, which we always use as 128 bytes
        radioType,          // high power or low power radio, as there are  two types of Seeed Studio modules
        unusedHardware,

        activeLoggingSources,
        name,
        unusedGeneral,

        oldestMeasurementOffset,
        newMeasurementsOffset,
        unusedMeasurements,

        DevEUI,
        DevAddr,
        uplinkFrameCounter,
        downlinkFrameCounter,
        applicationSessionKey,
        networkSessionKey,
        rx1Delay,
        dataRate,
        rx1DataRateOffset,
        rx2DataRateIndex,
        unusedLoRaWAN1,
        txChannels,
        rxChannel,
        unusedLoRaWAN2,

        sensorSettings,

        numberOfSettings
    };
    static bool isValid();
    static uint32_t getMapVersion();

    template <typename dataType>
    static void save(const dataType& data, settingIndex theIndex);

    template <typename dataType>
    static dataType read(settingIndex theIndex);

    static void save(const uint8_t data, settingIndex theIndex, uint32_t offset);
    static uint8_t read(settingIndex theIndex, uint32_t offset);

    static void saveByteArray(const uint8_t* dataIn, settingIndex theIndex);
    static void readByteArray(uint8_t* dataOut, settingIndex theIndex);

    static void saveSensorSettings(const uint8_t settings, uint8_t deviceAndChannelIndex);
    static uint8_t readSensorSettings(uint8_t deviceAndChannelIndex);

    static constexpr uint8_t maxMapVersion{1};

#ifndef unitTesting

  private:
#endif
    static bool isValidIndex(settingIndex aSettingIndex) { return (static_cast<uint32_t>(aSettingIndex) < static_cast<uint32_t>(settingsCollection::settingIndex::numberOfSettings)); }
    static const setting settings[static_cast<uint32_t>(settingIndex::numberOfSettings)];
};

template <typename dataType>
void settingsCollection::save(const dataType& sourceData, settingIndex theIndex) {
    if (settingsCollection::isValidIndex(theIndex)) {
        const uint8_t* bytePtr = reinterpret_cast<const uint8_t*>(&sourceData);
        uint32_t startAddress  = settingsCollection::settings[static_cast<uint32_t>(theIndex)].startAddress;
        uint32_t length        = settingsCollection::settings[static_cast<uint32_t>(theIndex)].length;
        nonVolatileStorage::write(startAddress, bytePtr, length);
    }
}

template <typename dataType>
dataType settingsCollection::read(settingIndex theIndex) {
    dataType result{};
    if (settingsCollection::isValidIndex(theIndex)) {
        uint8_t* bytePtr      = reinterpret_cast<uint8_t*>(&result);
        uint32_t startAddress = settingsCollection::settings[static_cast<uint32_t>(theIndex)].startAddress;
        uint32_t length       = settingsCollection::settings[static_cast<uint32_t>(theIndex)].length;
        nonVolatileStorage::read(startAddress, bytePtr, length);
    }
    return result;
}
