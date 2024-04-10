#include <measurementcollection.hpp>
#include <nvs.hpp>
#include <logging.hpp>
#include <sensordevicetype.hpp>

uint32_t measurementCollection::writeAddress{0};
uint32_t measurementCollection::startAddress{0};        // TODO : what should be good initial value?
uint32_t measurementCollection::endAddress{0};

void measurementCollection::initialize() {
    findStartEndAddress();
}

// uint32_t measurementCollection::head{0};
// uint32_t measurementCollection::level{0};
// uint32_t measurementCollection::measurementWriteIndex{0};                    // measurement setting index where we will write the next measurement
// uint32_t measurementCollection::oldestUnsentMeasurementIndex{0};             // measurement setting index of the oldest unconfirmed measurement
// uint32_t measurementCollection::oldestUnconfirmedMeasurementindex{0};        // measurement setting index of the oldest unsent measurement

// void measurementCollection::add(measurement newSample) {
// }

// void measurementCollection::get() {
// }

// uint32_t measurementCollection::getNmbrToBeTransmitted() {
//     return 50;        // TODO - implement
// }

// void measurementCollection::read(uint32_t measurementIndex, measurement& destination) {
//     measurementIndex      = measurementIndex % nmbrMeasurementBlocks;
//     uint32_t startAddress = (measurementIndex * measurementBlockLength) + measurementsAddressOffset;
//     uint8_t rawData[measurementBlockLength]{};
//     nonVolatileStorage::read(startAddress, rawData, measurementBlockLength);
//     destination.timestamp.asBytes[0] = rawData[0];
//     destination.timestamp.asBytes[1] = rawData[1];
//     destination.timestamp.asBytes[2] = rawData[2];
//     destination.timestamp.asBytes[3] = rawData[3];
//     destination.value.asBytes[0]     = rawData[4];
//     destination.value.asBytes[1]     = rawData[5];
//     destination.value.asBytes[2]     = rawData[6];
//     destination.value.asBytes[3]     = rawData[7];
//     // destination.sensorDeviceId                 = static_cast<sensorChannelType>(rawData[8]);
//     // destination.channelAndFlags                = rawData[9];
// }

// void measurementCollection::write(uint32_t measurementIndex, measurement& source) {
//     measurementIndex      = measurementIndex % nmbrMeasurementBlocks;
//     uint32_t startAddress = (measurementIndex * measurementBlockLength) + measurementsAddressOffset;
//     uint8_t rawData[measurementBlockLength];

//     rawData[0] = source.timestamp.asBytes[0];
//     rawData[1] = source.timestamp.asBytes[1];
//     rawData[2] = source.timestamp.asBytes[2];
//     rawData[3] = source.timestamp.asBytes[3];

//     rawData[4] = source.value.asBytes[0];
//     rawData[5] = source.value.asBytes[1];
//     rawData[6] = source.value.asBytes[2];
//     rawData[7] = source.value.asBytes[3];

//     // rawData[8] = static_cast<uint8_t>(source.type);
//     // rawData[9] = source.flags;
//     nonVolatileStorage::write(startAddress, rawData, measurementBlockLength);
// }

// void measurementCollection::run() {
//     // TODO - implement
// }

// bool measurementCollection::isReady() {
//     // TODO - implement
//     return true;
// }

void measurementCollection::findStartEndAddress() {
    // search the end of measurements by searching for 0xFF, 0xFF, 0xFF, 0xFF
    // If the bytes after that are not 0xFF, older measurements are overwritten and the start of measurements is after the 0xFF, 0xFF, 0xFF, 0xFF
    // If the bytes are 0xFF, 0xFF, 0xFF, 0xFF, the start of measurements is at 4096
    union {
        uint32_t dword;
        uint8_t bytes[4];
    } data;
    startAddress = nonVolatileStorage::measurementsStartAddress;
    endAddress   = nonVolatileStorage::measurementsStartAddress + nonVolatileStorage::measurementsSize;
    for (uint32_t address = nonVolatileStorage::measurementsStartAddress; address < nonVolatileStorage::measurementsStartAddress + nonVolatileStorage::measurementsSize; address++) {
        nonVolatileStorage::read(address, data.bytes, 1);
        if (data.bytes[0] == 0xFF) {
            nonVolatileStorage::read(address, data.bytes, 4);
            if (data.dword == 0xFFFFFFFF) {
                endAddress = address;
                nonVolatileStorage::read(address + 4, data.bytes, 4);
                if (data.dword == 0xFFFFFFFF) {
                    startAddress = nonVolatileStorage::measurementsStartAddress;
                } else {
                    startAddress = address + 4;
                }
                break;
            }
        }
    }
}

void measurementCollection::dump() {
    if (endAddress <= startAddress) {
        logging::snprintf("No measurements found");
        return;
    }
    uint32_t address = startAddress;
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

    while (address < endAddress) {
        nonVolatileStorage::read(address, &byteZero, 1);
        bool hasTimestamp = byteZero & 0x80;
        sensorDeviceType theSensorDeviceType = static_cast<sensorDeviceType>((byteZero & 0x7C) >> 2);
        uint8_t channelId = (byteZero & 0x03);
        if (hasTimestamp) {
            nonVolatileStorage::read(address + 1, timestamp.asBytes, 4);
            nonVolatileStorage::read(address + 5, value.asBytes, 4);
            logging::snprintf("date:time %u:%u value", theSensorDeviceType, channelId);
            address += 9;
        } else {
            nonVolatileStorage::read(address + 1, value.asBytes, 4);
            logging::snprintf("????:???? %u:%u value", theSensorDeviceType, channelId);
            address += 5;
        }
    }
}
