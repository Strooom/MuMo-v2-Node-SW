// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>

class nonVolatileStorage {
  public:
    nonVolatileStorage() = delete;
    static bool isPresent();        // testing if the EEPROM is found correctly on the I2C bus

    static constexpr uint32_t size{64 * 1024};
    static constexpr uint8_t i2cAddress{0x50};
    static constexpr uint8_t halTrialsIsPresent{0x03};
    static constexpr uint8_t halTimeoutIsPresent{10};
    static constexpr uint8_t halTrialsIsBusy{0x05};
    static constexpr uint8_t halTimeoutIsBusy{1};
    static constexpr uint8_t writeCycleTime{4};        // [ms] : the BR24G512xxx EEPROM requires a wait time of max 3.5 ms after writing to internally store the data
    static constexpr uint8_t blankEepromValue{0xFF};

#ifndef unitTesting

  private:
#endif
    static void read(const uint32_t startAddress, uint8_t* data, const uint32_t dataLength);
    static void write(const uint32_t startAddress, const uint8_t* data, const uint32_t dataLength);
    static void erase();
    static void fill(uint8_t value);

    friend class settingsCollection;
    friend class measurementCollection;
};
