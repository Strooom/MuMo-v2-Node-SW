// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <measurement.hpp>
#include <linearbuffer.hpp>

// This implements a large circular buffer to store measurements in EEPROM
// Measurements with the same timestamp as the previous measurement do not store this timestamp again. This implies the length of a measurement can vary:
// * With timestamp : 9 bytes
//   1 byte for timestampFlag=1 [7] | sensorDeviceId [6..2] | channelId [1..0]
//   4 bytes for the timestamp
//   4 bytes for the value
// * Without timestamp : 5 bytes
//   1 byte for timestampFlag=0 [7] | sensorDeviceId [6..2] | channelId [1..0]
//   4 bytes for the value

// we keep a writepointer to the EEPROM address where the next measurement should be written.
// After writing a batch of measurements, we also write 4 bytes of 0xFF, this can be used to retrieve the writepointer after a reset.

// When writing a batch of measurements to eeprom, we need to take care of crossing page boundaries, 128 or 256 bytes depending on the type of EEPROM -> maybe we need a setting for the page size.
// if there is a page boundary between start and end-address, we need to split it to start - boundary and boundary - end.

// search the end of measurements by searching for 0xFF, 0xFF, 0xFF, 0xFF
// If the bytes after that are not 0xFF, older measurements are overwritten and the start of measurements is after the 0xFF, 0xFF, 0xFF, 0xFF
// If the bytes are 0xFF, 0xFF, 0xFF, 0xFF, the start of measurements is at 4096


struct transmit {
    uint32_t uplinkFrameCount;
    uint32_t startAddress;
};

static constexpr uint32_t maxNmbrOfRetransmitItems{8};
transmit reTransmitData[maxNmbrOfRetransmitItems];
uint32_t reTransmitWriteIndex{0};

    class measurementCollection2 {
  public:
    void dump();

#ifndef unitTesting

  private:
#endif
    static uint32_t writeAddress;
    static constexpr uint32_t maxNmbrOfUnsavedMeasurements{16};
    static linearBuffer<maxNmbrOfUnsavedMeasurements> unsavedMeasurements;

    static constexpr uint32_t measurementsAddressStart{4096};                     // First 4K is for settings, Last 124K for measurements
    static constexpr uint32_t measurementsAddressEnd{(128U * 1024U) - 1U};        // 128K EEPROM

    static void read(uint32_t measurementIndex, measurement& destination);        // reads the measurement from EEPROM and stores it in the destination measurement object
    static void write(uint32_t measurementIndex, measurement& source);            // write data from source measurement object to the EEPROM at specific location
};
