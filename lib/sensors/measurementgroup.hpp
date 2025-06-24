// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <ctime>

class measurementGroup {
  public:
    void reset() { nmbrOfMeasurements = 0; };
    uint32_t getNumberOfMeasurements() const { return nmbrOfMeasurements; };
    time_t getTimeStamp() const { return timestamp; };
    uint32_t getDeviceIndex(uint32_t measurementIndex) const { return measurements[measurementIndex].deviceIndex; };
    uint32_t getChannelIndex(uint32_t measurementIndex) const { return measurements[measurementIndex].channelIndex; };
    float getValue(uint32_t measurementIndex) const { return measurements[measurementIndex].value; };

    void snprint(char* tmpString, uint32_t maxLength) const;
    void setTimestamp(time_t newTimestamp);
    void addMeasurement(uint32_t deviceIndex, uint32_t channelIndex, float value);
    void fromBytes(const uint8_t* source);
    void toBytes(uint8_t* buffer, uint32_t bufferSize) const;
    static uint32_t lengthInBytes(uint32_t someNmbrOfMeasurements) {
        return ((someNmbrOfMeasurements * 5U) + 6U);        // 1 byte for nmbrOfMeasurements, 4 bytes for timestamp, 1 byte for checksum + 5 bytes per measurement : 1 byte DeviceChannelIndex + 4 byte float value
    };
    static uint8_t calculateChecksum(const uint8_t* buffer, uint32_t bufferSize);

    static constexpr uint32_t maxNmbrOfMeasurements{16};
    static constexpr uint32_t oldestPossibleTimestamp{1577836800};        // Jan 01 2020 00:00:00 GMT+0000 // 0x5E0BE100
    static constexpr uint8_t defaultChecksum{0xB4};                       // checksum for 0x5E0BE100 oldestPossibleTimestamp

#ifndef unitTesting

  private:
#endif

    uint8_t nmbrOfMeasurements{0};
    time_t timestamp{oldestPossibleTimestamp};
    struct {
        uint8_t deviceIndex;
        uint8_t channelIndex;
        float value;
    } measurements[maxNmbrOfMeasurements];

    static uint8_t compressDeviceAndChannelIndex(uint8_t deviceIndex, uint8_t channelIndex) {
        return (((deviceIndex << 3) & 0b11111000) | (channelIndex & 0b00000111));
    };
    static uint8_t extractDeviceIndex(uint8_t deviceAndChannelIndex) {
        return (deviceAndChannelIndex >> 3);
    };
    static uint8_t extractChannelIndex(uint8_t deviceAndChannelIndex) {
        return (deviceAndChannelIndex & 0b00000111);
    };
};
