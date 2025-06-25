#include <framecount.hpp>

void frameCount::increment() {
    ++frameCountAsWord;
    syncBytesFromWord();
}

void frameCount::guessFromUint16(uint16_t frameCount16Lsb) {
    for (uint32_t index = 0; index < maximumGap; index++) {
        if (((frameCountAsWord + index) & 0x0000FFFF) == frameCount16Lsb) {
            frameCountAsWord += index;
            syncBytesFromWord();
            return;
        }
    }
}

void frameCount::setFromByteArray(const uint8_t bytes[lengthInBytes]) {
    (void)memcpy(frameCountAsBytes, bytes, lengthInBytes);
    syncWordFromBytes();
}
void frameCount::setFromWord(const uint32_t wordIn) {
    frameCountAsWord = wordIn;
    syncBytesFromWord();
}
