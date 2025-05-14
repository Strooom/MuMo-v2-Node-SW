#include <measurementgroup.hpp>
#include <float.hpp>
#include <realtimeclock.hpp>
#include <stdio.h>

bool measurementGroup::isValid() const {
    return ((calculateChecksum() == checkSum) && (timestamp >= oldestPossibleTimestamp));
}

uint8_t measurementGroup::calculateChecksum() const {
    uint8_t result                  = nmbrOfMeasurements;
    const uint8_t* timestampAsBytes = realTimeClock::bytesFromTime_t(timestamp);
    for (uint32_t byteIndex = 0; byteIndex < 4; byteIndex++) {
        result = result ^ timestampAsBytes[byteIndex];
    }
    for (uint32_t measurementIndex = 0; measurementIndex < nmbrOfMeasurements; measurementIndex++) {
        result                      = result ^ compressDeviceAndChannelIndex(measurements[measurementIndex].deviceIndex, measurements[measurementIndex].channelIndex);
        const uint8_t* valueAsBytes = floatToBytes(measurements[measurementIndex].value);
        for (uint32_t byteIndex = 0; byteIndex < 4; byteIndex++) {
            result = result ^ valueAsBytes[byteIndex];
        }
    }
    return result;
}

bool measurementGroup::isValidChecksum() const {
    return (calculateChecksum() == checkSum);
}

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
    checkSum = calculateChecksum();
}

void measurementGroup::setTimestamp(time_t newTimestamp) {
    timestamp = newTimestamp;
    checkSum  = calculateChecksum();
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

    nmbrOfMeasurements = tmpNmbrOfMeasurements;
    const uint8_t* timestampAsBytes = source + 1;
    timestamp                       = realTimeClock::time_tFromBytes(timestampAsBytes);
    checkSum                        = source[5];
    for (uint32_t measurementIndex = 0; measurementIndex < nmbrOfMeasurements; measurementIndex++) {
        measurements[measurementIndex].deviceIndex  = source[6 + (measurementIndex * 5)];
        measurements[measurementIndex].channelIndex = source[7 + (measurementIndex * 5)];
        measurements[measurementIndex].value        = bytesToFloat(source + 8 + (measurementIndex * 5));
    }
}

void measurementGroup::toBytes(uint8_t* buffer, uint32_t bufferSize) const {
}
