#include <measurementgroup.hpp>
#include <float.hpp>
#include <realtimeclock.hpp>
#include <stdio.h>
#include <cstring>

void measurementGroup::snprint(char* tmpString, uint32_t maxLength) const {
    snprintf(tmpString, maxLength, "%u measurements from %s", nmbrOfMeasurements, ctime(&timestamp));
}

void measurementGroup::addMeasurement(uint32_t deviceIndex, uint32_t channelIndex, float value) {
    if (nmbrOfMeasurements < maxNmbrOfMeasurements) {
        measurements[nmbrOfMeasurements].deviceIndex  = static_cast<uint8_t>(deviceIndex);
        measurements[nmbrOfMeasurements].channelIndex = static_cast<uint8_t>(channelIndex);
        measurements[nmbrOfMeasurements].value        = value;
        nmbrOfMeasurements++;
    }
}

void measurementGroup::setTimestamp(time_t newTimestamp) {
    timestamp = newTimestamp;
}

void measurementGroup::toBytes(uint8_t* buffer, uint32_t bufferSize) const {
    if (bufferSize < lengthInBytes(nmbrOfMeasurements)) {
        return;
    }

    buffer[0]                       = nmbrOfMeasurements;
    const uint8_t* timestampAsBytes = realTimeClock::bytesFromTime_t(timestamp);
    memcpy(buffer + 1, timestampAsBytes, 4);
    for (uint32_t measurementIndex = 0; measurementIndex < nmbrOfMeasurements; measurementIndex++) {
        buffer[5 + (measurementIndex * 5)] = compressDeviceAndChannelIndex(measurements[measurementIndex].deviceIndex, measurements[measurementIndex].channelIndex);
        const uint8_t* valueAsBytes        = floatToBytes(measurements[measurementIndex].value);
        memcpy(buffer + 6 + (measurementIndex * 5), valueAsBytes, 4);
    }
    buffer[5 + (nmbrOfMeasurements * 5)] = calculateChecksum(buffer, lengthInBytes(nmbrOfMeasurements) - 1);
}

void measurementGroup::fromBytes(const uint8_t* source) {
    uint8_t tmpNmbrOfMeasurements = source[0];
    if (tmpNmbrOfMeasurements > maxNmbrOfMeasurements) {
        return;
    }

    uint32_t tmpLengthInBytes = lengthInBytes(tmpNmbrOfMeasurements);
    uint8_t tmpCheckSum{0};
    for (uint32_t index = 0; index < tmpLengthInBytes; index++) {
        tmpCheckSum = tmpCheckSum ^ source[index];
    }
    if (tmpCheckSum != 0x00) {
        return;
    }

    nmbrOfMeasurements              = tmpNmbrOfMeasurements;
    const uint8_t* timestampAsBytes = source + 1;
    timestamp                       = realTimeClock::time_tFromBytes(timestampAsBytes);
    for (uint32_t measurementIndex = 0; measurementIndex < nmbrOfMeasurements; measurementIndex++) {
        measurements[measurementIndex].deviceIndex  = extractDeviceIndex(source[5 + (measurementIndex * 5)]);
        measurements[measurementIndex].channelIndex = extractChannelIndex(source[5 + (measurementIndex * 5)]);
        measurements[measurementIndex].value        = bytesToFloat(source + 6 + (measurementIndex * 5));
    }
}

uint8_t measurementGroup::calculateChecksum(const uint8_t* buffer, uint32_t bufferSize) {
    uint8_t result{0};
    for (uint32_t index = 0; index < bufferSize; index++) {
        result = result ^ buffer[index];
    }
    return result;
}
