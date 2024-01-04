// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include "measurement.h"

// This implements a large circular buffer to store samples.
// Samples are always written to EEPROM, but the head and level are kept in RAM, after reset, we can recover the head and level from scanning the EEPROM

class measurementCollection {
  public:
    static void add(measurement newSample);
    static void get();
    static uint32_t getNmbrToBeTransmitted();        // how many measurements are still to be transmitted

#ifndef unitTesting

  private:
#endif
    static uint32_t head;
    static uint32_t level;
    static uint32_t measurementWriteIndex;                                        // measurement setting index where we will write the next measurement
    static uint32_t oldestUnsentMeasurementIndex;                                 // measurement setting index of the oldest unconfirmed measurement
    static uint32_t oldestUnconfirmedMeasurementindex;                            // measurement setting index of the oldest unsent measurement
                                                                                  //    uint32_t nmbrMeasurementBlocks{12288};                // (120*1024)/100 = 12288
    static constexpr uint32_t nmbrMeasurementBlocks{6144};                        // (60*1024)/100 = 6144
    static constexpr uint32_t measurementBlockLength{10};                         // 10 bytes
    static constexpr uint32_t measurementsAddressOffset{4096};                    // First 4K is for settings, Last 60K for measurements

    static void read(uint32_t measurementIndex, measurement& destination);        // reads the measurement from EEPROM and stores it in the destination measurement object
    static void write(uint32_t measurementIndex, measurement& source);            // write data from source measurement object to the EEPROM at specific location
};
