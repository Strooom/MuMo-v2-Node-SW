#include <measurementcollection.hpp>
#include <nvs.hpp>
#include <logging.hpp>
#include <sensordevicetype.hpp>
#include <sensordevicecollection.hpp>
#include <realtimeclock.hpp>
#include <float.hpp>

uint32_t measurementCollection::oldestMeasurementOffset{0};
uint32_t measurementCollection::newMeasurementsOffset{0};
uint32_t measurementCollection::uplinkHistoryIndex{0};
measurementCollection::uplinkMeasurement measurementCollection::uplinkHistory[uplinkHistoryLength]{};
linearBuffer<measurementCollection::newMeasurementsLength> measurementCollection::newMeasurements;
uint32_t measurementCollection::nmbrOfNewMeasurements{0};

void measurementCollection::initialize() {
    oldestMeasurementOffset = 0;
    newMeasurementsOffset   = 0;
    uplinkHistoryIndex      = 0;
    for (uint32_t i = 0; i < uplinkHistoryLength; i++) {
        uplinkHistory[i].frameCount = 0;
        uplinkHistory[i].offset     = 0;
    }
    nmbrOfNewMeasurements = 0;
    newMeasurements.initialize();
}

void measurementCollection::eraseAll() {
    static constexpr uint32_t blockSize{128};
    uint8_t defaultData[blockSize];
    memset(defaultData, 0xFF, blockSize);
    uint32_t nmbrOfBlocks = nonVolatileStorage::measurementsSize / blockSize;
    for (uint32_t blockIndex = 0; blockIndex < nmbrOfBlocks; blockIndex++) {
        nonVolatileStorage::write(offsetToAddress(blockIndex * blockSize), defaultData, blockSize);
    }
}

// void measurementCollection::eraseOne(uint32_t offset) {}

void measurementCollection::findMeasurementsInEeprom() {
    int32_t offsetDataBackward = findDataByteBackward();
    if (offsetDataBackward == -1) {
        oldestMeasurementOffset = 0;
        newMeasurementsOffset   = 0;
        return;
    }
    int32_t offsetGapBackward = findGapBackward(offsetDataBackward);
    if (offsetGapBackward == -1) {
        oldestMeasurementOffset = 0;
        newMeasurementsOffset   = 0;
        return;
    }
    int32_t offsetDataForward = findDataByteForward();
    if (offsetDataForward == -1) {
        oldestMeasurementOffset = 0;
        newMeasurementsOffset   = 0;
        return;
    }
    int32_t offsetGapForward = findGapForward(offsetDataBackward);
    if (offsetGapForward == -1) {
        oldestMeasurementOffset = 0;
        newMeasurementsOffset   = 0;
        return;
    }
    oldestMeasurementOffset                  = (offsetGapBackward + 1) % nonVolatileStorage::measurementsSize;
    newMeasurementsOffset                    = offsetGapForward;
    uplinkHistory[uplinkHistoryIndex].offset = offsetGapForward;
}

void measurementCollection::addMeasurement(uint32_t deviceIndex, uint32_t channelIndex, float aValue) {
    uint8_t measurementHeader{0};
    measurementHeader = measurementHeader | ((static_cast<uint8_t>(deviceIndex) << 3) & 0b11111000);
    measurementHeader = measurementHeader | (static_cast<uint8_t>(channelIndex) & 0b00000111);
    newMeasurements.append(&measurementHeader, 1);
    newMeasurements.append(floatToBytes(aValue), 4);
    nmbrOfNewMeasurements++;
}

void measurementCollection::saveNewMeasurementsToEeprom() {
    newMeasurements.prefix(realTimeClock::bytesFromTime_t(realTimeClock::get()), 4);
    newMeasurements.prefix(nmbrOfNewMeasurements);
    uint32_t level           = newMeasurements.getLevel();
    uint8_t trailingBytes[4] = {0xFF, 0xFF, 0xFF, 0xFF};        // TODO : check if we need to erase old measurements and add more 0xFF padding
    newMeasurements.append(trailingBytes, 4);
    nonVolatileStorage::write(offsetToAddress(newMeasurementsOffset), newMeasurements.asUint8Ptr(), newMeasurements.getLevel());        // TODO : this also needs to wrap around
    newMeasurementsOffset = (newMeasurementsOffset + level) % nonVolatileStorage::measurementsSize;
    // newMeasurements.initialize();
    nmbrOfNewMeasurements = 0;
}

uint32_t measurementCollection::nmbrOfBytesToTransmit() {
    return (nonVolatileStorage::measurementsSize + newMeasurementsOffset - uplinkHistory[uplinkHistoryIndex].offset) % nonVolatileStorage::measurementsSize;
}

const uint8_t* measurementCollection::getTransmitBuffer() {
    return newMeasurements.asUint8Ptr();
}

void measurementCollection::setTransmitted(uint32_t frameCount, uint32_t length) {
    uplinkHistory[uplinkHistoryIndex].frameCount = frameCount;
    uint32_t offset                              = uplinkHistory[uplinkHistoryIndex].offset;
    uplinkHistoryIndex                           = (uplinkHistoryIndex + 1) % uplinkHistoryLength;
    uplinkHistory[uplinkHistoryIndex].offset     = (offset + length) % nonVolatileStorage::measurementsSize;
    newMeasurements.initialize();
}

#pragma region eepromSearchFunctions

bool measurementCollection::is0xFF(uint32_t offset) {
    uint8_t rawByte;
    nonVolatileStorage::read(offsetToAddress(offset), &rawByte, 1);
    return (rawByte == 0xFF);
}

int32_t measurementCollection::findDataByteForward() {
    for (uint32_t offset = 0; offset < nonVolatileStorage::measurementsSize; offset++) {
        if (!is0xFF(offset)) {
            return offset;
        }
    }
    return -1;
}

int32_t measurementCollection::findDataByteBackward() {
    for (int32_t offset = (nonVolatileStorage::measurementsSize - 1); offset >= 0; offset--) {
        if (!is0xFF(offset)) {
            return static_cast<uint32_t>(offset);
        }
    }
    return -1;
}

int32_t measurementCollection::findGapForward(uint32_t startOffset) {
    for (uint32_t offset = 0; offset < nonVolatileStorage::measurementsSize; offset++) {
        uint32_t wrappingOffset = (startOffset + offset) % nonVolatileStorage::measurementsSize;
        if (isGapForward(wrappingOffset)) {
            return wrappingOffset;
        }
    }
    return -1;
}

int32_t measurementCollection::findGapBackward(uint32_t startOffset) {
    for (uint32_t offset = 0; offset < nonVolatileStorage::measurementsSize; offset++) {
        uint32_t wrappingOffset = (startOffset + nonVolatileStorage::measurementsSize - offset) % nonVolatileStorage::measurementsSize;
        if (isGapBackward(wrappingOffset)) {
            return wrappingOffset;
        }
    }
    return -1;
}

bool measurementCollection::isGapForward(uint32_t offset) {
    uint8_t rawByte;
    for (uint32_t index = 0; index < measurementsGap; index++) {
        uint32_t wrappingoffset = (offset + index) % nonVolatileStorage::measurementsSize;
        nonVolatileStorage::read(offsetToAddress(wrappingoffset), &rawByte, 1);
        if (rawByte != 0xFF) {
            return false;
        }
    }
    return true;
}

bool measurementCollection::isGapBackward(uint32_t offset) {
    uint8_t rawByte;
    for (uint32_t index = 0; index < measurementsGap; index++) {
        uint32_t wrappingOffset = (offset + nonVolatileStorage::measurementsSize - index) % nonVolatileStorage::measurementsSize;
        nonVolatileStorage::read(offsetToAddress(wrappingOffset), &rawByte, 1);
        if (rawByte != 0xFF) {
            return false;
        }
    }
    return true;
}

#pragma endregion
#pragma region measurementMembers

uint32_t measurementCollection::nmbrOfMeasurementsInGroup(uint32_t measurementGroupOffset) {
    uint8_t nmbrOfMeasurements;
    nonVolatileStorage::read(offsetToAddress(measurementGroupOffset), &nmbrOfMeasurements, 1);
    return nmbrOfMeasurements;
}

uint32_t measurementCollection::nmbrOfMeasurementBytes() {
    return (nonVolatileStorage::measurementsSize + newMeasurementsOffset - oldestMeasurementOffset) % nonVolatileStorage::measurementsSize;
}

time_t measurementCollection::timestampOfMeasurementsGroup(uint32_t measurementGroupOffset) {
    uint8_t timeStampAsBytes[4];
    nonVolatileStorage::read(offsetToAddress(measurementGroupOffset + 1), timeStampAsBytes, 1);        // cannot read 4 bytes at once, because of address wrapping
    nonVolatileStorage::read(offsetToAddress(measurementGroupOffset + 2), timeStampAsBytes + 1, 1);
    nonVolatileStorage::read(offsetToAddress(measurementGroupOffset + 3), timeStampAsBytes + 2, 1);
    nonVolatileStorage::read(offsetToAddress(measurementGroupOffset + 4), timeStampAsBytes + 3, 1);
    time_t timestamp = realTimeClock::time_tFromBytes(timeStampAsBytes);
    return timestamp;
}

uint8_t measurementCollection::measurementHeader(uint32_t measurementOffset) {
    uint8_t measurementHeader;
    nonVolatileStorage::read(offsetToAddress(measurementOffset), &measurementHeader, 1);
    return measurementHeader;
}

uint32_t measurementCollection::measurementDeviceIndex(uint32_t measurementOffset) {
    uint8_t header       = measurementHeader(measurementOffset);
    uint32_t deviceIndex = (header & 0b11111000) >> 3;
    return deviceIndex;
}

uint32_t measurementCollection::measurementChannelIndex(uint32_t measurementOffset) {
    uint8_t header        = measurementHeader(measurementOffset);
    uint32_t channelIndex = header & 0b00000111;
    return channelIndex;
}

float measurementCollection::measurementValue(uint32_t measurementOffset) {
    uint8_t measurementValueAsBytes[4];
    nonVolatileStorage::read(offsetToAddress(measurementOffset + 1), measurementValueAsBytes, 1);        // cannot read 4 bytes at once, because of address wrapping
    nonVolatileStorage::read(offsetToAddress(measurementOffset + 2), measurementValueAsBytes + 1, 1);
    nonVolatileStorage::read(offsetToAddress(measurementOffset + 3), measurementValueAsBytes + 2, 1);
    nonVolatileStorage::read(offsetToAddress(measurementOffset + 4), measurementValueAsBytes + 3, 1);
    float measurementValue = bytesToFloat(measurementValueAsBytes);
    return measurementValue;
}

#pragma endregion

void measurementCollection::dumpMeasurement(uint32_t measurementOffset) {
    uint32_t deviceIndex  = measurementDeviceIndex(measurementOffset);
    uint32_t channelIndex = measurementChannelIndex(measurementOffset);
    float value           = measurementValue(measurementOffset);

    uint32_t decimals = sensorDeviceCollection::decimals(deviceIndex, channelIndex);
    uint32_t intPart  = integerPart(value, decimals);
    if (decimals > 0) {
        uint32_t fracPart = fractionalPart(value, decimals);
        logging::snprintf(logging::source::sensorData, "  %s - %s : %d.%d %s\n", sensorDeviceCollection::name(deviceIndex), sensorDeviceCollection::name(deviceIndex, channelIndex), intPart, fracPart, sensorDeviceCollection::units(deviceIndex, channelIndex));
    } else {
        logging::snprintf(logging::source::sensorData, "  %s - %s : %d %s\n", sensorDeviceCollection::name(deviceIndex), sensorDeviceCollection::name(deviceIndex, channelIndex), intPart, sensorDeviceCollection::units(deviceIndex, channelIndex));
    }
}

uint32_t measurementCollection::dumpMeasurementGroup(uint32_t measurementGroupOffset) {
    uint8_t nmbrOfMeasurements = nmbrOfMeasurementsInGroup(measurementGroupOffset);
    time_t timestamp           = timestampOfMeasurementsGroup(measurementGroupOffset);
    logging::snprintf("%s", ctime(&timestamp));        // ctime appends a newline by itself..

    uint32_t measurementOffset   = measurementGroupOffset + 5;
    uint32_t nmbrOfBytesConsumed = +5;
    for (uint32_t index = 0; index < nmbrOfMeasurements; index++) {
        dumpMeasurement(measurementOffset);
        measurementOffset += 5;
        nmbrOfBytesConsumed += 5;
    }
    return nmbrOfBytesConsumed;
}

void measurementCollection::dumpAll() {
    uint32_t nmbrOfBytes = nmbrOfMeasurementBytes();
    logging::snprintf("%d bytes of measurements found\n", nmbrOfBytes);

    for (uint32_t index = 0; index < nmbrOfBytes;) {
        uint32_t bytesConsumed = dumpMeasurementGroup(index);
        index += bytesConsumed;
    }
}

void measurementCollection::dumpRaw(uint32_t offset, uint32_t nmbrOfBytes) {
    uint8_t rawByte;
    uint32_t nmbrOfRows = (nmbrOfBytes / 16);

    for (uint32_t row = 0; row < nmbrOfRows; row++) {
        logging::snprintf("[%d] : ", (offset + row));
        for (uint32_t byteIndex = 0; byteIndex < 16; byteIndex++) {
            nonVolatileStorage::read(offsetToAddress(offset + (16 * row) + byteIndex), &rawByte, 1);
            logging::snprintf("%02x ", rawByte);
        }
        logging::snprintf("\n");
    }
}
