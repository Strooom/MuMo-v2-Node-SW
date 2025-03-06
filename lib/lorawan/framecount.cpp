#include <framecount.hpp>

frameCount::frameCount() : asUint32(0) {};

frameCount::frameCount(uint32_t initialValue) : asUint32(initialValue) {};

frameCount& frameCount::operator=(const uint32_t theFrameCount) {
    asUint32 = theFrameCount;
    return *this;
}

frameCount& frameCount::operator=(const frameCount& theFrameCount) {
    asUint32 = theFrameCount.asUint32;
    return *this;
}

bool frameCount::operator==(const frameCount& theFrameCount) const {
    return (asUint32 == theFrameCount.asUint32);
}

bool frameCount::operator!=(const frameCount& theFrameCount) const {
    return (asUint32 != theFrameCount.asUint32);
}

bool frameCount::operator>(const frameCount& theFrameCount) const {
    return (asUint32 > theFrameCount.asUint32);
}

frameCount& frameCount::operator++(int) {
    asUint32++;
    return *this;
}

uint8_t& frameCount::operator[](int index) {
    return asUint8[index];
}

void frameCount::guessFromUint16(uint16_t frameCount16Lsb) {
    for (uint32_t index = 0; index < maximumGap; index++) {
        if (((asUint32 + index) & 0x0000FFFF) == frameCount16Lsb) {
            asUint32 += index;
            return;
        }
    }
}
