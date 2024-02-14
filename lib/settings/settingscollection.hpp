#pragma once
#include <stdint.h>
#include "setting.hpp"
#include "nvs.hpp"

class settingsCollection {
  public:
    settingsCollection() = delete;
    enum class settingIndex : uint32_t {
        nvsMapVersion = 0,
        displayVersion,
        batteryVersion,
        activeLoggingSources,
        unusedGeneral,

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
        dataRate,
        unusedLoRaWAN1,
        txChannels,
        rxChannel,
        unusedLoRaWAN2,

        numberOfSettings
    };
    static bool isInitialized();
    static void initializeOnce();
    template <typename dataType>
    static void save(const dataType& data, settingIndex theIndex);
    template <typename dataType>
    static dataType read(settingIndex theIndex);
    static void saveByteArray(const uint8_t* dataIn, settingIndex theIndex);
    static void readByteArray(uint8_t* dataOut, settingIndex theIndex);

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
