#include <channelcollection.hpp>

loRaChannel::loRaChannel(uint32_t theFrequency, uint8_t theMinimumDataRateIndex, uint8_t theMaximumDataRateIndex) : frequencyInHz{theFrequency}, minimumDataRateIndex{theMinimumDataRateIndex}, maximumDataRateIndex{theMaximumDataRateIndex} {
}
