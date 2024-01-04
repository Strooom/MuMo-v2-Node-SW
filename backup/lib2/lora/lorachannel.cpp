#include "channelcollection.h"

loRaChannel::loRaChannel(uint32_t theFrequency, uint32_t theMinimumDataRateIndex, uint32_t theMaximumDataRateIndex) : frequency{theFrequency}, minimumDataRateIndex{theMinimumDataRateIndex}, maximumDataRateIndex{theMaximumDataRateIndex} {
}
