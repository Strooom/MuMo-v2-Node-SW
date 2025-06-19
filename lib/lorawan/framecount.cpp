#include <framecount.hpp>

frameCount::frameCount() : asUint32(0) {
    frameCountAsWord = 0;
    syncBytesFromWord();
};

frameCount::frameCount(uint32_t initialValue) : asUint32(initialValue) {
    frameCountAsWord = initialValue;
    syncBytesFromWord();
};

frameCount& frameCount::operator=(const uint32_t theFrameCount) {
    asUint32         = theFrameCount;
    frameCountAsWord = theFrameCount;
    syncBytesFromWord();
    return *this;
}

frameCount& frameCount::operator=(const frameCount& theFrameCount) {
    asUint32         = theFrameCount.asUint32;
    frameCountAsWord = theFrameCount.frameCountAsWord;
    syncBytesFromWord();
    return *this;
}

// bool frameCount::operator==(const frameCount& theFrameCount) const {
//     return (asUint32 == theFrameCount.asUint32);
// }

// bool frameCount::operator!=(const frameCount& theFrameCount) const {
//     return (asUint32 != theFrameCount.asUint32);
// }

// bool frameCount::operator>(const frameCount& theFrameCount) const {
//     return (asUint32 > theFrameCount.asUint32);
// }

// frameCount& frameCount::operator++(int) {
//     asUint32++;
//     frameCountAsWord = asUint32;
//     syncBytesFromWord();
//     return *this;
// }

void frameCount::increment() {
    ++frameCountAsWord;
    asUint32 = frameCountAsWord;
    syncBytesFromWord();
}

uint8_t& frameCount::operator[](int index) {
    return asUint8[index];
}

void frameCount::guessFromUint16(uint16_t frameCount16Lsb) {
    for (uint32_t index = 0; index < maximumGap; index++) {
        if (((asUint32 + index) & 0x0000FFFF) == frameCount16Lsb) {
            asUint32 += index;
            frameCountAsWord = asUint32;
            syncBytesFromWord();
            return;
        }
    }
}

void frameCount::setFromByteArray(const uint8_t bytes[lengthInBytes]) {
    (void)memcpy(frameCountAsBytes, bytes, lengthInBytes);
    syncWordFromBytes();
    asUint32 = frameCountAsWord;
}
void frameCount::setFromWord(const uint32_t wordIn) {
    frameCountAsWord = wordIn;
    asUint32         = wordIn;
    syncBytesFromWord();
}
