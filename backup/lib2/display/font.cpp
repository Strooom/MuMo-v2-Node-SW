#include "font.h"

bool font::getPixel(uint32_t x, uint32_t y, uint8_t asciiCode) const {
    if (!isInBounds(asciiCode)) {
        return false;
    }
    if (!isInBounds(x, y)) {
        return false;
    }
    return ((pixelData[getByteIndex(asciiCode) + getByteIndex(x, y)] & (1 << getBitIndex(x))) != 0);
}
