#include "font.hpp"

bool font::charIsInBounds(uint8_t asciiCode) const {
    if ((asciiCode < properties.asciiStart) || (asciiCode > properties.asciiEnd)) {
        return false;
    }
    if (characters[asciiCode - properties.asciiStart].widthInPixels == 0) {
        return false;
    }
    return true;
}

uint32_t font::getCharacterWidthInPixels(uint8_t asciiCode) const {
    if ((asciiCode < properties.asciiStart) || (asciiCode > properties.asciiEnd)) {
        return 0;
    } else {
        return characters[asciiCode - properties.asciiStart].widthInPixels;
    }
}

uint32_t font::getOffsetInBytes(uint8_t asciiCode) const {
    if ((asciiCode < properties.asciiStart) || (asciiCode > properties.asciiEnd)) {
        return 0;
    } else {
        return characters[asciiCode - properties.asciiStart].offsetInBytes;
    }
}


bool font::getPixel(uint32_t x, uint32_t y, uint32_t pixelDataOffset) const {
    return ((pixelData[pixelDataOffset + getOffsetInBytes(x, y)] & (1 << getBitIndex(y))) != 0);
}

