// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <time.h>
#include <nvs.hpp>
#include <linearbuffer.hpp>

class measurementCollection {
  public:
    static void initialize();
    static void findMeasurementsInEeprom();
    static void eraseAll();
    static void eraseOne(uint32_t measurementOffset);

    static void addMeasurement(uint32_t deviceIndex, uint32_t channelIndex, float value);
    static void saveNewMeasurementsToEeprom();
    static uint32_t nmbrOfBytesToTransmit();
    static const uint8_t* getTransmitBuffer();
    static void setTransmitted(uint32_t frameCount, uint32_t length);

    static uint32_t nmbrOfMeasurementBytes();
    static uint32_t nmbrOfMeasurementsInGroup(uint32_t measurementGroupOffset);
    static time_t timestampOfMeasurementsGroup(uint32_t measurementGroupOffset);
    static uint8_t measurementHeader(uint32_t measurementOffset);
    static uint32_t measurementDeviceIndex(uint32_t measurementOffset);
    static uint32_t measurementChannelIndex(uint32_t measurementOffset);
    static float measurementValue(uint32_t measurementOffset);

    static uint32_t getOldestMeasurementOffset() { return oldestMeasurementOffset; };
    static uint32_t getNewMeasurementsOffset() { return newMeasurementsOffset; };

    static void dumpMeasurement(uint32_t addressOffset);
    static uint32_t dumpMeasurementGroup(uint32_t addressOffset);
    static void dumpAll();
    static void dumpRaw(uint32_t offset, uint32_t nmbrOfBytes);

#ifndef unitTesting

  private:
#endif

    struct uplinkMeasurement {
        uint32_t frameCount;
        uint32_t offset;
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
    static linearBuffer<newMeasurementsLength> newMeasurements;
    static uint32_t nmbrOfNewMeasurements;

    static uint32_t oldestMeasurementOffset;
    static uint32_t newMeasurementsOffset;
    static constexpr uint32_t measurementsGap{4};
    static uint32_t lastMeasurementWithTimestampOffset;
};
