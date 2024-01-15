// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

// We need to respect the amount of transmit airtime as follows:
// ETSI 300 220 : 1% duty cycle for 868 MHz, measured over 1 hour ( < 36 seconds / hour)
// The Things Network : 30 seconds total transmission time, measured over 24 hours ( < 30 seconds / 24 hours)
// clearly the TTN requirement is more stringent than the ETSI requirement, so FTTB we only check this one.

struct transmitRecord {
    uint32_t timestamp{0};        // from RTC
    uint32_t duration{0};         // [ms]
};

class dutyCycle {
  public:
    void addRecord(transmitRecord newRecord);
    void purgeRecords();                            // remove all older than 24 hours
    uint32_t getRemainingTransmitTimeInMs();        // check how much is left from our 30 seconds budget

  private:
    static constexpr uint32_t numberOfRecords{128};
    transmitRecord records[numberOfRecords];
};