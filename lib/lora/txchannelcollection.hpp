// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <lorachannel.hpp>

class loRaTxChannelCollection {
  public:
    static void selectRandomChannelIndex();
    static uint32_t getCurrentChannelIndex();
    static constexpr uint32_t maxNmbrChannels{16};        // Regional Parameters 1.0.3 line 320

    static loRaChannel channel[maxNmbrChannels];

#ifndef unitTesting
  private:
#endif

    static uint32_t getRandomNumber();
    static uint32_t currentChannelIndex;
    static void selectNextActiveChannelIndex();
};
