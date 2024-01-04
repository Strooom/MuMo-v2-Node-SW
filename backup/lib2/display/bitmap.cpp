#include "bitmap.h"

bool bitmap::getPixel(uint32_t x, uint32_t y) const {
    if (isInBounds(x, y)) {
        return ((pixelData[getByteIndex(x, y)] & (1 << getBitIndex(x))) != 0);
    }
    return false;
}
