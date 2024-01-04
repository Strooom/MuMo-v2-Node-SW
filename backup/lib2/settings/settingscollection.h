#pragma once
#include <stdint.h>
#include "setting.h"
#include "nvs.h"

class settingsCollection {
  public:
    settingsCollection() = delete;
    enum class settingIndex : uint32_t {
        nvsMapVersion = 0,
        displayVersion,
        batteryVersion,
        unusedGeneral,
        activeloggingSources,

        measurementWriteIndex,
        oldestUnsentMeasurementIndex,
        oldestUnconfirmedMeasurementindex,
        unusedMeasurementDataManagement,

        DevEUI,
        DevAddr,
        uplinkFrameCounter,
        downlinkFrameCounter,
        applicationSessionKey,
        networkSessionKey,
        rx1Delay,
        unusedLoRaWAN,

        numberOfSettings
    };
    static bool isInitialized();
    static void initializeOnce();
    template <typename dataType>
    static void save(settingIndex theIndex, const dataType& data);
    template <typename dataType>
    static dataType read(settingIndex theIndex);
    static void save(settingIndex theIndex, const uint8_t* dataIn);
    static void read(settingIndex theIndex, uint8_t* dataOut);

#ifndef unitTesting

  private:
#endif
    static bool isValidIndex(settingIndex aSettingIndex) { return (static_cast<uint32_t>(aSettingIndex) < static_cast<uint32_t>(settingsCollection::settingIndex::numberOfSettings)); }
    static const setting settings[static_cast<uint32_t>(settingIndex::numberOfSettings)];        // static member is initialized in settingscollection.cpp
};

template <typename dataType>
void settingsCollection::save(settingIndex theIndex, const dataType& sourceData) {
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
