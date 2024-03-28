// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
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
    static constexpr uint8_t i2cAddress{0x50};         // default I2C address of the first EEPROM, the second one has an address i2cAddress+1, set by its A0 pin being ties to VDD
    static constexpr uint8_t halTrials{0x03};          // ST HAL requires a 'retry' parameters
    static constexpr uint8_t halTimeout{100};          // ST HAL requires a 'timeout' in ms
    static constexpr uint8_t writeCycleTime{4};        // [ms] : the BR24G512xxx EEPROM requires a wait time of 3.5 ms after writing to internally store the data
    static constexpr uint8_t blankEepromValue{0xFF};

#ifndef unitTesting

  private:
#endif
    static void read(const uint32_t startAddress, uint8_t* data, const uint32_t dataLength);               // low-level I2C reading from EEPROM
    static void write(const uint32_t startAddress, const uint8_t* data, const uint32_t dataLength);        // low-level I2C writing to EEPROM
    static void erase();                                                                                   // erase the EEPROM by writing 0xFF to all locations
    static void fill(uint8_t value);                                                                       // fill the EEPROM with a specific value - only used for unitTesting

    friend class settingsCollection;           // settings can access the EEPROM
    friend class measurementCollection;        // settings can access the EEPROM
};
