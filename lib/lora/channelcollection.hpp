// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <lorachannel.hpp>

class loRaChannelCollection {
  public:
    static void selectRandomChannelIndex();
    static uint32_t getCurrentChannelIndex();
    static constexpr uint32_t maxNmbrChannels{16};        // Regional Parameters 1.0.3 line 320

    static loRaChannel txRxChannels[maxNmbrChannels];
    static loRaChannel rx2Channel;

#ifndef unitTesting
  private:
#endif

    static uint32_t getRandomNumber();
    static uint32_t currentChannelIndex;
    static void selectNextActiveChannelIndex();
};
