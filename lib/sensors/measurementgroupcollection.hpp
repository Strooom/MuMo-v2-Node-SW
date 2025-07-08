// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <measurementgroup.hpp>
#include <nvs.hpp>

class measurementGroupCollection {
  public:
    measurementGroupCollection() = delete;
    static void initialize();
    static void reset();
    static void eraseOldest();
    static void addNew(const measurementGroup &aMeasurementGroup);
    static void get(measurementGroup &aMeasurementGroup, uint32_t offset);

    static uint32_t getOldestMeasurementOffset() { return oldestMeasurementOffset; };
    static uint32_t getNewMeasurementsOffset() { return newMeasurementsOffset; };
    static uint32_t getFreeSpace();

#ifndef unitTesting

  private:
#endif
    static uint32_t oldestMeasurementOffset;
    static uint32_t newMeasurementsOffset;
    static bool isValid() { return ((oldestMeasurementOffset != 0xFFFFFFFF) && (newMeasurementsOffset != 0xFFFFFFFF)); };
    static uint32_t getAddressFromOffset(uint32_t offset) { return nonVolatileStorage::measurementsStartAddress + (offset % nonVolatileStorage::getMeasurementsAreaSize()); };
};
