#include <measurementcollection.hpp>
#include <nvs.hpp>
#include <logging.hpp>
#include <sensordevicetype.hpp>
#include <sensordevicecollection.hpp>
#include <realtimeclock.hpp>
#include <float.hpp>

extern uint8_t mockEepromMemory[nonVolatileStorage::totalSize];

uint32_t measurementCollection::oldestMeasurementOffset{0};
uint32_t measurementCollection::newMeasurementsOffset{0};
uint32_t measurementCollection::uplinkHistoryIndex{0};
measurementCollection::uplinkMeasurement measurementCollection::uplinkHistory[uplinkHistoryLength]{};
linearBuffer<measurementCollection::newMeasurementsLength> measurementCollection::newMeasurements;
uint32_t measurementCollection::nmbrOfNewMeasurements{0};

void measurementCollection::initialize() {
    measurementCollection::oldestMeasurementOffset = 0;
    measurementCollection::newMeasurementsOffset   = 0;
    measurementCollection::uplinkHistoryIndex      = 0;
    for (uint32_t i = 0; i < uplinkHistoryLength; i++) {
        measurementCollection::uplinkHistory[i].frameCount  = 0;
        measurementCollection::uplinkHistory[i].startOffset = 0;
    }
    newMeasurements.initialize();
    nmbrOfNewMeasurements = 0;
}

void measurementCollection::erase() {
    static constexpr uint32_t blockSize{128};
    uint8_t defaultData[blockSize];
    memset(defaultData, 0xFF, blockSize);
    uint32_t nmbrOfBlocks = nonVolatileStorage::measurementsSize / blockSize;
    for (uint32_t blockIndex = 0; blockIndex < nmbrOfBlocks; blockIndex++) {
        nonVolatileStorage::write(addressFromOffset(blockIndex * blockSize), defaultData, blockSize);
    }
}

uint32_t measurementCollection::nmbrOfBytesToTransmit() {
    return (newMeasurementsOffset - uplinkHistory[uplinkHistoryIndex].startOffset);
}

void measurementCollection::saveNewMeasurementsToEeprom() {
    newMeasurements.prefix(realTimeClock::time_tToBytes(realTimeClock::get()), 4);
    newMeasurements.prefix(nmbrOfNewMeasurements);
    uint32_t level           = newMeasurements.getLevel();
    uint8_t trailingBytes[4] = {0xFF, 0xFF, 0xFF, 0xFF};        // TODO : check if we need to erase old measurements and add more 0xFF padding
    newMeasurements.append(trailingBytes, 4);
    nonVolatileStorage::write(addressFromOffset(newMeasurementsOffset), newMeasurements.asUint8Ptr(), newMeasurements.getLevel());
    newMeasurementsOffset = (newMeasurementsOffset + level) % nonVolatileStorage::measurementsSize;
    newMeasurements.initialize();
    nmbrOfNewMeasurements = 0;
}

void measurementCollection::setTransmitted(uint32_t frameCount, uint32_t length) {
    uplinkHistory[uplinkHistoryIndex].frameCount  = frameCount;
    uint32_t offset                               = uplinkHistory[uplinkHistoryIndex].startOffset;
    uplinkHistoryIndex                            = (uplinkHistoryIndex + 1) % uplinkHistoryLength;
    uplinkHistory[uplinkHistoryIndex].startOffset = (offset + length) % nonVolatileStorage::measurementsSize;
}

void measurementCollection::findStartEndOffsets() {
    // search the end of measurements by searching for 0xFF, 0xFF, 0xFF, 0xFF
    // If the bytes after that are not 0xFF, older measurements are overwritten and the start of measurements is after the 0xFF, 0xFF, 0xFF, 0xFF
    // If the bytes are 0xFF, 0xFF, 0xFF, 0xFF, the start of measurements is at 4096
    union {
        uint32_t asUint32;
        uint8_t bytes[4];
    } data;
    oldestMeasurementOffset = 0;
    newMeasurementsOffset   = 0;
    for (uint32_t offset = 0; offset < nonVolatileStorage::measurementsSize; offset++) {
        nonVolatileStorage::read(addressFromOffset(offset), data.bytes, 1);
        if (data.bytes[0] == 0xFF) {
            nonVolatileStorage::read(addressFromOffset(offset), data.bytes, 4);
            if (data.asUint32 == 0xFFFFFFFF) {
                newMeasurementsOffset = offset;
                nonVolatileStorage::read(addressFromOffset(offset + 4), data.bytes, 4);
                if (data.asUint32 == 0xFFFFFFFF) {
                    oldestMeasurementOffset = 0;
                } else {
                    oldestMeasurementOffset = (offset + 4) % nonVolatileStorage::measurementsSize;
                }
                break;
            }
        }
    }
}

void measurementCollection::addMeasurement(uint32_t deviceIndex, uint32_t channelIndex, float aValue) {
    uint8_t measurementHeader{0};
    measurementHeader = measurementHeader | ((static_cast<uint8_t>(deviceIndex) << 3) & 0b11111000);
    measurementHeader = measurementHeader | (static_cast<uint8_t>(channelIndex) & 0b00000111);
    newMeasurements.append(&measurementHeader, 1);
    newMeasurements.append(floatToBytes(aValue), 4);
    nmbrOfNewMeasurements++;
}

uint32_t measurementCollection::nmbrOfMeasurementsInGroup(uint32_t measurementGroupOffset) {
    uint8_t nmbrOfMeasurements;
    nonVolatileStorage::read(addressFromOffset(measurementGroupOffset), &nmbrOfMeasurements, 1);
    return nmbrOfMeasurements;
}

uint32_t measurementCollection::nmbrOfMeasurementBytes() {
    int32_t nmbrOfMeasurementBytes = static_cast<int32_t>(newMeasurementsOffset) - static_cast<int32_t>(oldestMeasurementOffset);
    if (nmbrOfMeasurementBytes < 0) {
        nmbrOfMeasurementBytes += nonVolatileStorage::measurementsSize;
    }
    return static_cast<uint32_t>(nmbrOfMeasurementBytes);
}

time_t measurementCollection::timestampOfMeasurementsGroup(uint32_t measurementGroupOffset) {
    uint8_t timeStampAsBytes[4];
    nonVolatileStorage::read(addressFromOffset(measurementGroupOffset + 1), timeStampAsBytes, 4);
    time_t timestamp = realTimeClock::bytesToTime_t(timeStampAsBytes);
    return timestamp;
}

uint8_t measurementCollection::measurementHeader(uint32_t measurementOffset) {
    uint8_t measurementHeader;
    nonVolatileStorage::read(addressFromOffset(measurementOffset), &measurementHeader, 1);
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
    nonVolatileStorage::read(addressFromOffset(measurementOffset + 1), measurementValueAsBytes, 4);
    float measurementValue = bytesToFloat(measurementValueAsBytes);
    return measurementValue;
}

void measurementCollection::dumpMeasurement(uint32_t measurementOffset) {
    uint32_t deviceIndex  = measurementDeviceIndex(measurementOffset);
    uint32_t channelIndex = measurementChannelIndex(measurementOffset);
    float value           = measurementValue(measurementOffset);

    uint32_t decimals = sensorDeviceCollection::channelDecimals(deviceIndex, channelIndex);
    uint32_t intPart  = integerPart(value, decimals);
    if (decimals > 0) {
        uint32_t fracPart = fractionalPart(value, decimals);
        logging::snprintf(logging::source::sensorData, "  %s - %s : %d.%d %s\n", sensorDeviceCollection::name(deviceIndex), sensorDeviceCollection::channelName(deviceIndex, channelIndex), intPart, fracPart, sensorDeviceCollection::channelUnits(deviceIndex, channelIndex));
    } else {
        logging::snprintf(logging::source::sensorData, "  %s - %s : %d %s\n", sensorDeviceCollection::name(deviceIndex), sensorDeviceCollection::channelName(deviceIndex, channelIndex), intPart, sensorDeviceCollection::channelUnits(deviceIndex, channelIndex));
    }
}

uint32_t measurementCollection::dumpMeasurementGroup(uint32_t measurementGroupOffset) {
    uint8_t nmbrOfMeasurements = nmbrOfMeasurementsInGroup(measurementGroupOffset);
    time_t timestamp           = timestampOfMeasurementsGroup(measurementGroupOffset);
    logging::snprintf("%s", ctime(&timestamp));        // ctime appends a newline by itself..

    uint32_t measurementOffset = measurementGroupOffset + 5;
    uint32_t nmbrOfBytesConsumed = + 5;
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
    uint8_t rawBytes[16];
    uint32_t nmbrOfRows = (nmbrOfBytes / 16);

    for (uint32_t row = 0; row < nmbrOfRows; row++) {
        nonVolatileStorage::read(addressFromOffset(offset + (16 * row)), rawBytes, 16);
        logging::snprintf("[%d] : %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n", (offset + row), rawBytes[0], rawBytes[1], rawBytes[2], rawBytes[3], rawBytes[4], rawBytes[5], rawBytes[6], rawBytes[7], rawBytes[8], rawBytes[9], rawBytes[10], rawBytes[11], rawBytes[12], rawBytes[13], rawBytes[14], rawBytes[15]);
    }
}