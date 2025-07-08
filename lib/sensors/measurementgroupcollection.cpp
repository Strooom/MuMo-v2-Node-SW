#include <measurementgroupcollection.hpp>
#include <nvs.hpp>
#include <settingscollection.hpp>
#include <logging.hpp>
#include <sensordevicetype.hpp>
#include <sensordevicecollection.hpp>
#include <realtimeclock.hpp>
#include <float.hpp>
#include <stdio.h>

uint32_t measurementGroupCollection::oldestMeasurementOffset{0};
uint32_t measurementGroupCollection::newMeasurementsOffset{0};

void measurementGroupCollection::initialize() {
    // read the offsets from non-volatile storage
    oldestMeasurementOffset = settingsCollection::read<uint32_t>(settingsCollection::settingIndex::oldestMeasurementOffset);
    newMeasurementsOffset   = settingsCollection::read<uint32_t>(settingsCollection::settingIndex::newMeasurementsOffset);
    if (!isValid()) {
        reset();
    }
}

void measurementGroupCollection::reset() {
    oldestMeasurementOffset = 0;
    newMeasurementsOffset   = 0;
    settingsCollection::save(oldestMeasurementOffset, settingsCollection::settingIndex::oldestMeasurementOffset);
    settingsCollection::save(newMeasurementsOffset, settingsCollection::settingIndex::newMeasurementsOffset);
}

void measurementGroupCollection::addNew(const measurementGroup& aMeasurementGroup) {
    uint32_t lengthInBytes   = measurementGroup::lengthInBytes(aMeasurementGroup.getNumberOfMeasurements());
    uint32_t nmbrOfFreeBytes = getFreeSpace();
    while (nmbrOfFreeBytes < lengthInBytes) {
        eraseOldest();
        nmbrOfFreeBytes = getFreeSpace();
    }
    uint8_t buffer[lengthInBytes];
    aMeasurementGroup.toBytes(buffer, lengthInBytes);

    const uint8_t* remainingData{buffer};
    uint32_t remainingLength{lengthInBytes};
    uint32_t currentOffset{newMeasurementsOffset};

    while (remainingLength > 0) {
        uint32_t bytesInThisPage = nonVolatileStorage::bytesInCurrentPage(getAddressFromOffset(newMeasurementsOffset), remainingLength);
        nonVolatileStorage::writeInPage(getAddressFromOffset(currentOffset), remainingData, bytesInThisPage);
        currentOffset += bytesInThisPage;
        remainingData += bytesInThisPage;
        remainingLength -= bytesInThisPage;
    }

    newMeasurementsOffset = (newMeasurementsOffset + lengthInBytes) % nonVolatileStorage::getMeasurementsAreaSize();
    settingsCollection::save(newMeasurementsOffset, settingsCollection::settingIndex::newMeasurementsOffset);
}

void measurementGroupCollection::eraseOldest() {
    measurementGroup oldestMeasurementGroup;
    uint32_t nmbrOfMeasurements = nonVolatileStorage::read(getAddressFromOffset(oldestMeasurementOffset));
    uint32_t lengthInBytes      = measurementGroup::lengthInBytes(nmbrOfMeasurements);
    oldestMeasurementOffset     = (oldestMeasurementOffset + lengthInBytes) % nonVolatileStorage::getMeasurementsAreaSize();
    settingsCollection::save(oldestMeasurementOffset, settingsCollection::settingIndex::oldestMeasurementOffset);
}

void measurementGroupCollection::get(measurementGroup& aMeasurementGroup, uint32_t offset) {
    uint32_t lengthInBytes = measurementGroup::lengthInBytes(nonVolatileStorage::read(getAddressFromOffset(offset)));
    uint8_t buffer[lengthInBytes];

    uint8_t* remainingData{buffer};
    uint32_t remainingLength{lengthInBytes};
    uint32_t currentOffset{offset};

    // TODO : wakeUp NVS if needed

    while (remainingLength > 0) {
        uint32_t bytesInThisPage = nonVolatileStorage::bytesInCurrentPage(getAddressFromOffset(currentOffset), remainingLength);
        nonVolatileStorage::readInPage(getAddressFromOffset(currentOffset), remainingData, bytesInThisPage);
        currentOffset += bytesInThisPage;
        remainingData += bytesInThisPage;
        remainingLength -= bytesInThisPage;
    }

    aMeasurementGroup.fromBytes(buffer);

    // TODO : put NVS back to sleep if it was sleeping
}

uint32_t measurementGroupCollection::getFreeSpace() {
    if (oldestMeasurementOffset == 0 && newMeasurementsOffset == 0) {
        return nonVolatileStorage::getMeasurementsAreaSize();
    } else {
        return ((nonVolatileStorage::getMeasurementsAreaSize() + oldestMeasurementOffset) - newMeasurementsOffset) % nonVolatileStorage::getMeasurementsAreaSize();
    }
}