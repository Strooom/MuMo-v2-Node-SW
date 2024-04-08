// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <datarate.hpp>

class loRaChannel {
  public:
    loRaChannel(uint32_t frequency, uint8_t minimumDataRateIndex, uint8_t maximumDataRateIndex);
    void toBytes(uint8_t *bytes);
    void fromBytes(uint8_t *bytes);

    uint32_t frequencyInHz{0};        // NOTE : a frequency of 0 means the channel is not used
    uint8_t minimumDataRateIndex{0};
    uint8_t maximumDataRateIndex{0};

    static constexpr uint32_t sizeInBytes{6};
#ifndef unitTesting

  private:
#endif

    friend class loRaChannelCollection;
};
