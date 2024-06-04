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
    static void initialize();
    static void findMeasurementsInEeprom();
    static void erase();

    static void addMeasurement(uint32_t deviceIndex, uint32_t channelIndex, float value);
    static void saveNewMeasurementsToEeprom();
    static uint32_t nmbrOfBytesToTransmit();
    static void setTransmitted(uint32_t frameCount, uint32_t length);

    static uint32_t nmbrOfMeasurementBytes();
    static uint32_t nmbrOfMeasurementsInGroup(uint32_t measurementGroupOffset);
    static time_t timestampOfMeasurementsGroup(uint32_t measurementGroupOffset);
    static uint8_t measurementHeader(uint32_t measurementOffset);
    static uint32_t measurementDeviceIndex(uint32_t measurementOffset);
    static uint32_t measurementChannelIndex(uint32_t measurementOffset);
    static float measurementValue(uint32_t measurementOffset);

    static void dumpMeasurement(uint32_t addressOffset);
    static uint32_t dumpMeasurementGroup(uint32_t addressOffset);
    static void dumpAll();
    static void dumpRaw(uint32_t offset, uint32_t nmbrOfBytes);

#ifndef unitTesting

  private:
#endif

    struct uplinkMeasurement {
        uint32_t frameCount;
        uint32_t startOffset;
    };
    static constexpr uint32_t uplinkHistoryLength{256};
    static uint32_t uplinkHistoryIndex;
    static uplinkMeasurement uplinkHistory[uplinkHistoryLength];

    static bool is0xFF(uint32_t offset);
    static int32_t findDataByteForward();
    static int32_t findDataByteBackward();
    static int32_t findGapForward(uint32_t startOffset);
    static int32_t findGapBackward(uint32_t startOffset);

    static bool isGapForward(uint32_t offset);
    static bool isGapBackward(uint32_t offset);

    static uint32_t offsetToAddress(uint32_t offset) { return (nonVolatileStorage::measurementsStartAddress + (offset % nonVolatileStorage::measurementsSize)); };

    static constexpr uint32_t newMeasurementsLength{260};
    static linearBuffer<newMeasurementsLength> newMeasurements;        // this collects measurements before writing them to EEPROM
    static uint32_t nmbrOfNewMeasurements;

    static uint32_t oldestMeasurementOffset;
    static uint32_t newMeasurementsOffset;
    static constexpr uint32_t measurementsGap{4};        // number of 0xFF bytes between the last measurement and the first new measurement
    static uint32_t lastMeasurementWithTimestampOffset;
};
