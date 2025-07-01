#include <framecount.hpp>

void frameCount::increment() {
    uint32_t tmpWord = getAsWord();
    ++tmpWord;
    setFromWord(tmpWord);
}

void frameCount::guessFromUint16(uint16_t frameCount16Lsb) {
    uint32_t tmpWord = getAsWord();
    for (uint32_t index = 0; index < maximumGap; index++) {
        if (((tmpWord + index) & 0x0000FFFF) == frameCount16Lsb) {
            tmpWord += index;
            setFromWord(tmpWord);
            return;
        }
    }
}

void frameCount::setFromByteArray(const uint8_t bytes[lengthInBytes]) {
    (void)memcpy(frameCountAsBytes, bytes, lengthInBytes);
}

void frameCount::setFromWord(const uint32_t wordIn) {
    (void)memcpy(frameCountAsBytes, &wordIn, lengthInBytes);
}

uint32_t frameCount::getAsWord() const {
    uint32_t result;
    (void)memcpy(&result, frameCountAsBytes, lengthInBytes);
    return result;
}