#include <measurementcollection.hpp>
#include <nvs.hpp>
#include <logging.hpp>
#include <sensordevicetype.hpp>

extern uint8_t mockEepromMemory[nonVolatileStorage::totalSize];

uint32_t measurementCollection::oldestMeasurementOffset{0};
uint32_t measurementCollection::newMeasurementsOffset{0};
uint32_t measurementCollection::uplinkMemoryIndex{0};
measurementCollection::measurementsInUplink measurementCollection::uplinkMemory[maxNumberOfUplinksMemory]{};
linearBuffer<measurementCollection::newMeasurementsLength> measurementCollection::newMeasurements;

void measurementCollection::initialize() {
    measurementCollection::oldestMeasurementOffset = 0;
    measurementCollection::newMeasurementsOffset   = 0;
    measurementCollection::uplinkMemoryIndex       = 0;
    for (uint32_t i = 0; i < maxNumberOfUplinksMemory; i++) {
        measurementCollection::uplinkMemory[i].uplinkFrameCount = 0;
        measurementCollection::uplinkMemory[i].startOffset      = 0;
    }
    newMeasurements.initialize();
    findStartEndOffsets();
}

uint32_t measurementCollection::nmbrOfBytesToTransmit() {
    return (newMeasurementsOffset - uplinkMemory[uplinkMemoryIndex].startOffset);
}

void measurementCollection::saveNewMeasurementsToEeprom() {
    uint32_t level           = newMeasurements.getLevel();
    uint8_t trailingBytes[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    newMeasurements.append(trailingBytes, 4);
    nonVolatileStorage::write(addressFromOffset(newMeasurementsOffset), newMeasurements.asUint8Ptr(), newMeasurements.getLevel());
    newMeasurementsOffset = (newMeasurementsOffset + level) % nonVolatileStorage::measurementsSize;
    newMeasurements.initialize();
}

void measurementCollection::setTransmitted(uint32_t frameCount, uint32_t length) {
    uplinkMemory[uplinkMemoryIndex].uplinkFrameCount = frameCount;
    uint32_t offset                                  = uplinkMemory[uplinkMemoryIndex].startOffset;
    uplinkMemoryIndex                                = (uplinkMemoryIndex + 1) % maxNumberOfUplinksMemory;
    uplinkMemory[uplinkMemoryIndex].startOffset      = (offset + length) % nonVolatileStorage::measurementsSize;
}

void measurementCollection::findStartEndOffsets() {
    // search the end of measurements by searching for 0xFF, 0xFF, 0xFF, 0xFF
    // If the bytes after that are not 0xFF, older measurements are overwritten and the start of measurements is after the 0xFF, 0xFF, 0xFF, 0xFF
    // If the bytes are 0xFF, 0xFF, 0xFF, 0xFF, the start of measurements is at 4096
    union {
        uint32_t dword;
        uint8_t bytes[4];
    } data;
    oldestMeasurementOffset = 0;
    newMeasurementsOffset   = 0;
    for (uint32_t offset = 0; offset < nonVolatileStorage::measurementsSize; offset++) {
        nonVolatileStorage::read(addressFromOffset(offset), data.bytes, 1);
        if (data.bytes[0] == 0xFF) {
            nonVolatileStorage::read(addressFromOffset(offset), data.bytes, 4);
            if (data.dword == 0xFFFFFFFF) {
                newMeasurementsOffset = offset;
                nonVolatileStorage::read(addressFromOffset(offset + 4), data.bytes, 4);
                if (data.dword == 0xFFFFFFFF) {
                    oldestMeasurementOffset = 0;
                } else {
                    oldestMeasurementOffset = (offset + 4) % nonVolatileStorage::measurementsSize;
                }
                break;
            }
        }
    }
}

void measurementCollection::dump() {
    if (newMeasurementsOffset = oldestMeasurementOffset) {
        logging::snprintf("No measurements found");
        return;
    }
    uint32_t offset = oldestMeasurementOffset;
    uint8_t byteZero;
    union {
        uint32_t asUint32;
        uint8_t asBytes[4];
    } timestamp;
    union {
        uint32_t asFloat;
        uint32_t asUint32;
        uint8_t asBytes[4];
    } value;

    while (offset < newMeasurementsOffset) {
        nonVolatileStorage::read(addressFromOffset(offset), &byteZero, 1);
        bool hasTimestamp                    = byteZero & 0x80;
        sensorDeviceType theSensorDeviceType = static_cast<sensorDeviceType>((byteZero & 0x7C) >> 2);
        uint8_t channelId                    = (byteZero & 0x03);
        if (hasTimestamp) {
            nonVolatileStorage::read(addressFromOffset(offset + 1), timestamp.asBytes, 4);
            nonVolatileStorage::read(addressFromOffset(offset + 5), value.asBytes, 4);
            logging::snprintf("date:time %u:%u value", theSensorDeviceType, channelId);
            offset = (offset + 9) % nonVolatileStorage::measurementsSize;
        } else {
            nonVolatileStorage::read(addressFromOffset(offset + 1), value.asBytes, 4);
            logging::snprintf("????:???? %u:%u value", theSensorDeviceType, channelId);
            offset = (offset + 5) % nonVolatileStorage::measurementsSize;
        }
    }
}

void measurementCollection::addMeasurement(measurement &newMeasurement) {
    newMeasurements.append(&newMeasurement.id, 1);
    newMeasurementsOffset = (newMeasurementsOffset + 1) % nonVolatileStorage::measurementsSize;
    if (newMeasurement.id & measurement::hasTimestampMask) {
        newMeasurements.append(newMeasurement.timestamp.asBytes, 4);
        newMeasurementsOffset = (newMeasurementsOffset + 4) % nonVolatileStorage::measurementsSize;
    }
    newMeasurements.append(newMeasurement.value.asBytes, 4);
    newMeasurementsOffset = (newMeasurementsOffset + 4) % nonVolatileStorage::measurementsSize;
}
