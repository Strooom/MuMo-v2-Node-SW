#include <txchannelcollection.hpp>
#include <swaplittleandbigendian.hpp>

loRaChannel::loRaChannel(uint32_t theFrequency, uint8_t theMinimumDataRateIndex, uint8_t theMaximumDataRateIndex) : frequencyInHz{theFrequency}, minimumDataRateIndex{theMinimumDataRateIndex}, maximumDataRateIndex{theMaximumDataRateIndex} {
}

void loRaChannel::toBytes(uint8_t *bytes) {
    bytes[0] = bigEndianWordToByte0(frequencyInHz);
    bytes[1] = bigEndianWordToByte1(frequencyInHz);
    bytes[2] = bigEndianWordToByte2(frequencyInHz);
    bytes[3] = bigEndianWordToByte3(frequencyInHz);
    bytes[4] = maximumDataRateIndex;
    bytes[5] = minimumDataRateIndex;
}

void loRaChannel::fromBytes(uint8_t *bytes) {
    frequencyInHz = bytesToBigEndianWord(bytes[0], bytes[1], bytes[2], bytes[3]);
    maximumDataRateIndex = bytes[4];
    minimumDataRateIndex = bytes[5];
}
