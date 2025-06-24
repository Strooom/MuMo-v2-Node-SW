// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>

class nonVolatileStorage {
  public:
    nonVolatileStorage() = delete;
    static uint32_t detectNmbr64KBanks();
    static uint32_t totalSize() { return nmbr64KBanks * 64U * 1024U; };

    static constexpr uint32_t maxNmbr64KBanks{8};
    static constexpr uint32_t pageSize{128};
    static constexpr uint16_t baseI2cAddress{0x50};        // I2C address for the first 64K bank

    static constexpr uint8_t halNmbrOfTrials{0x03};        // ST-HAL parameter
    static constexpr uint8_t halTimeoutInMs{10};           // ST-HAL parameter

    static constexpr uint8_t halByteWriteTimeoutInMs{2};         // ST-HAL parameter needed
    static constexpr uint8_t halPageWriteTimeoutInMs{10};        // ST-HAL parameter needed
    static constexpr uint8_t writeCycleBusyTimeInMs{4};          // delay needed after writing the EEPROM to complete its internal write cycle and be ready for new access
    static constexpr uint8_t blankEepromValue{0xFF};

    static constexpr uint32_t settingStartAddress{0};
    static constexpr uint32_t settingsSize{4 * 1024};
    static constexpr uint32_t measurementsStartAddress{settingStartAddress + settingsSize};
    static constexpr uint32_t measurementsSize{60 * 1024};

#ifndef unitTesting

  private:
#endif
    static uint32_t nmbr64KBanks;
    // single byte read/write
    static uint8_t read(const uint32_t address);
    static void write(const uint32_t address, const uint8_t data);
    // range of bytes read/write but all inside a single page
    static void readInPage(const uint32_t startAddress, uint8_t* destination, const uint32_t dataLength);
    static void writeInPage(const uint32_t startAddress, const uint8_t* source, const uint32_t dataLength);
    // range of bytes read/write, may span multiple pages
    static void read(const uint32_t startAddress, uint8_t* destination, const uint32_t dataLength);
    static void write(const uint32_t startAddress, const uint8_t* source, const uint32_t dataLength);

    static uint32_t pageNumber(uint32_t address) { return address / pageSize; };
    static uint32_t bytesInCurrentPage(uint32_t address, uint32_t dataLength);
    static uint16_t bankNumber(uint32_t address) { return address / (64U * 1024U); };

    static void erase();
    static void fill(uint8_t value);

    static void disableWriteProtect();
    static void enableWriteProtect();
    static uint16_t bankI2cAddress(uint32_t address) { return baseI2cAddress + bankNumber(address); };
    static uint16_t bankOffset(uint32_t address) { return static_cast<uint16_t>(address % (64U * 1024U)); };

#ifdef generic
    static uint32_t mockEepromNmbr64KPages;
    static uint8_t mockEepromMemory[maxNmbr64KBanks * 64 * 1024];
#endif

    friend class settingsCollection;
    friend class measurementGroupCollection;
};
