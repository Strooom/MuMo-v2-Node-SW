#include <txchannelcollection.hpp>
#ifndef generic
#include "main.h"
extern RNG_HandleTypeDef hrng;
#else
#include <cstdlib>
#endif

loRaChannel loRaTxChannelCollection::channel[maxNmbrChannels]{
    loRaChannel(868'100'000U, 0, 5),
    loRaChannel(868'300'000U, 0, 5),
    loRaChannel(868'500'000U, 0, 5),
    loRaChannel(0U, 0, 5),
    loRaChannel(0U, 0, 5),
    loRaChannel(0U, 0, 5),
    loRaChannel(0U, 0, 5),
    loRaChannel(0U, 0, 5),
    loRaChannel(0U, 0, 5),
    loRaChannel(0U, 0, 5),
    loRaChannel(0U, 0, 5),
    loRaChannel(0U, 0, 5),
    loRaChannel(0U, 0, 5),
    loRaChannel(0U, 0, 5),
    loRaChannel(0U, 0, 5),
    loRaChannel(0U, 0, 5)};

// loRaChannel loRaTxChannelCollection::rx2Channel{869'525'000U, 0, 0};

uint32_t loRaTxChannelCollection::currentChannelIndex{0};

void loRaTxChannelCollection::selectNextActiveChannelIndex() {
    uint32_t loopCount{0}; // this will prevent an infinite loop if no active channel is found... there should always be at least 3 active channels
    currentChannelIndex = (currentChannelIndex + 1) % maxNmbrChannels;
    while ((channel[currentChannelIndex].frequencyInHz == 0) && loopCount < maxNmbrChannels) {
        currentChannelIndex = (currentChannelIndex + 1) % maxNmbrChannels;
        loopCount++;
    }
}

void loRaTxChannelCollection::selectRandomChannelIndex() {
    uint32_t randomCount = getRandomNumber() % maxNmbrChannels;
    for (uint32_t count = 0; count < randomCount; count++) {
        selectNextActiveChannelIndex();
    }
}

uint32_t loRaTxChannelCollection::getCurrentChannelIndex() {
    return currentChannelIndex;
}

uint32_t loRaTxChannelCollection::getRandomNumber() {
    uint32_t result{0};
#ifndef generic
    HAL_RNG_GenerateRandomNumber(&hrng, &result);
#else
    result = rand()%32U;
#endif
    return result;
}
