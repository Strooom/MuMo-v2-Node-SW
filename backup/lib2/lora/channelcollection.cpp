#include "channelcollection.h"
#ifndef generic
#include "main.h"
extern RNG_HandleTypeDef hrng;
#else
#include <cstdlib>
#endif

loRaChannel loRaChannelCollection::txRxChannels[maxNmbrChannels]{
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

loRaChannel loRaChannelCollection::rx2Channel{869'525'000U, 0, 0};

uint32_t loRaChannelCollection::currentChannelIndex{0};

void loRaChannelCollection::selectNextActiveChannelIndex() {
    currentChannelIndex = (currentChannelIndex + 1) % maxNmbrChannels;
    while (txRxChannels[currentChannelIndex].frequency == 0) {
        currentChannelIndex = (currentChannelIndex + 1) % maxNmbrChannels;
    }
}

void loRaChannelCollection::selectRandomChannelIndex() {
    uint32_t randomCount = getRandomNumber() % maxNmbrChannels;
    for (uint32_t count = 0; count < randomCount; count++) {
        selectNextActiveChannelIndex();
    }
}

uint32_t loRaChannelCollection::getCurrentChannelIndex() {
    return currentChannelIndex;
}

uint32_t loRaChannelCollection::getRandomNumber() {
    uint32_t result{0};
#ifndef generic
    HAL_RNG_GenerateRandomNumber(&hrng, &result);
#else
    result = rand();
#endif
    return result;
}
