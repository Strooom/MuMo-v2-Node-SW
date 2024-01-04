#include "measurementcollection.h"
#include "nvs.h"

// initialization of static members

uint32_t measurementCollection::head{0};
uint32_t measurementCollection::level{0};
uint32_t measurementCollection::measurementWriteIndex{0};                    // measurement setting index where we will write the next measurement
uint32_t measurementCollection::oldestUnsentMeasurementIndex{0};             // measurement setting index of the oldest unconfirmed measurement
uint32_t measurementCollection::oldestUnconfirmedMeasurementindex{0};        // measurement setting index of the oldest unsent measurement

void measurementCollection::add(measurement newSample) {
}

void measurementCollection::get() {
}

uint32_t measurementCollection::getNmbrToBeTransmitted() {
    return 50;        // TODO - implement
}

void measurementCollection::read(uint32_t measurementIndex, measurement& destination) {
    measurementIndex      = measurementIndex % nmbrMeasurementBlocks;
    uint32_t startAddress = (measurementIndex * measurementBlockLength) + measurementsAddressOffset;
    uint8_t rawData[measurementBlockLength]{};
    nonVolatileStorage::read(startAddress, rawData, measurementBlockLength);
    destination.timestamp.asBytes[0] = rawData[0];
    destination.timestamp.asBytes[1] = rawData[1];
    destination.timestamp.asBytes[2] = rawData[2];
    destination.timestamp.asBytes[3] = rawData[3];
    destination.value.asBytes[0]     = rawData[4];
    destination.value.asBytes[1]     = rawData[5];
    destination.value.asBytes[2]     = rawData[6];
    destination.value.asBytes[3]     = rawData[7];
    destination.type                 = static_cast<sensorChannelType>(rawData[8]);
    destination.flags                = rawData[9];
}

void measurementCollection::write(uint32_t measurementIndex, measurement& source) {
    measurementIndex      = measurementIndex % nmbrMeasurementBlocks;
    uint32_t startAddress = (measurementIndex * measurementBlockLength) + measurementsAddressOffset;
    uint8_t rawData[measurementBlockLength];

    rawData[0] = source.timestamp.asBytes[0];
    rawData[1] = source.timestamp.asBytes[1];
    rawData[2] = source.timestamp.asBytes[2];
    rawData[3] = source.timestamp.asBytes[3];

    rawData[4] = source.value.asBytes[0];
    rawData[5] = source.value.asBytes[1];
    rawData[6] = source.value.asBytes[2];
    rawData[7] = source.value.asBytes[3];

    rawData[8] = static_cast<uint8_t>(source.type);
    rawData[9] = source.flags;
    nonVolatileStorage::write(startAddress, rawData, measurementBlockLength);
}
