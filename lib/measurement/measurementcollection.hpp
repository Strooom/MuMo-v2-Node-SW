// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <nvs.hpp>
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

// we keep a writeOffset in the measurements section of the EEPROM where the next data should be written.
// After writing a batch of data, we also write 4 bytes of 0xFF, this can be used to retrieve the writeOffset after a reset.

// When writing data to eeprom, we need to take care of crossing page boundaries, 128 or 256 bytes depending on the type of EEPROM -> maybe we need a setting for the page size.
// if there is a page boundary between start and end-address, we need to split it to start - boundary and boundary - end.

// search the end of measurements by searching for 0xFF, 0xFF, 0xFF, 0xFF
// If the bytes after that are not 0xFF, older measurements are overwritten and the start of measurements is after the 0xFF, 0xFF, 0xFF, 0xFF
// If the bytes are 0xFF, 0xFF, 0xFF, 0xFF, the start of measurements is at offset 0

class measurementCollection {
  public:
    void dump();
    static void initialize();
    static uint32_t oldestMeasurementOffset;
    static uint32_t newMeasurementsOffset;
    static uint32_t lastMeasurementWithTimestampOffset;
    static uint32_t nmbrOfBytesToTransmit();

    static void saveNewMeasurementsToEeprom();
    static void setTransmitted(uint32_t frameCount, uint32_t length);

    static void addMeasurement(measurement &newMeasurement);

#ifndef unitTesting

  private:
#endif
    static void findStartEndOffsets();

    struct measurementsInUplink {
        uint32_t uplinkFrameCount;        // frameCount used to transmit the message
        uint32_t startOffset;             // start address of the measurements in EEPROM for that uplink
    };
    static constexpr uint32_t maxNumberOfUplinksMemory{256};        // length of the uplinkMemory table
    static uint32_t uplinkMemoryIndex;                              // index into the next write position in uplinkMemory table
    static measurementsInUplink uplinkMemory[maxNumberOfUplinksMemory];

    static uint32_t addressFromOffset(uint32_t offset) { return (nonVolatileStorage::measurementsStartAddress + offset) % nonVolatileStorage::measurementsSize; };

    static constexpr uint32_t newMeasurementsLength{260};
    static linearBuffer<newMeasurementsLength> newMeasurements;        // this collects measurements before writing them to EEPROM
};
