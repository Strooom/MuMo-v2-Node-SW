// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include "spreadingfactor.h"


class dataRate {
  public:
    spreadingFactor theSpreadingFactor;
    uint32_t maximumPayloadLength;

  private:
};

class dataRates {
  public:
    static constexpr uint32_t nmbrUsedDataRates{6};
    static const dataRate theDataRates[nmbrUsedDataRates];
    static uint32_t getDownlinkDataRateIndex(uint32_t uplinkDataRateIndex, uint32_t Rx1DataRateOffset);

  private:
};
