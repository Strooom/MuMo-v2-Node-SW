#include "framecount.h"

frameCount::frameCount(){};

frameCount::frameCount(uint32_t initialValue) : asUint32(initialValue){};

void frameCount::set(uint32_t theFrameCount) {
    asUint32 = theFrameCount;
}

frameCount& frameCount::operator=(uint32_t theFrameCount) {
    asUint32 = theFrameCount;
    return *this;
}

void frameCount::set(uint8_t theFrameCount[4]) {
    asUint8[0] = theFrameCount[0];
    asUint8[1] = theFrameCount[1];
    asUint8[2] = theFrameCount[2];
    asUint8[3] = theFrameCount[3];
}

void frameCount::increment() {
    asUint32++;
}

uint32_t frameCount::guessFromUint16(uint32_t frameCount32, uint16_t frameCount16Lsb) {
    for (uint32_t index = 0; index < maximumGap; index++) {
        if (((frameCount32 + index) & 0x0000FFFF) == frameCount16Lsb) {
            return (frameCount32 + index);
        }
    }
    return frameCount32;        // no match found, return the current value
}
