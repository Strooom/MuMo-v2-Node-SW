// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>

class fontProperties {
  public:
    fontProperties(const uint32_t thePixelHeight, const uint32_t theSpaceBetweenCharactersInPixels, const uint8_t theAsciiStart, const uint8_t theAsciiEnd) : heightInPixels(thePixelHeight), spaceBetweenCharactersInPixels(theSpaceBetweenCharactersInPixels), bytesPerRow(((thePixelHeight - 1) / 8) + 1), asciiStart(theAsciiStart), asciiEnd(theAsciiEnd){};

    const uint32_t heightInPixels;
    const uint32_t spaceBetweenCharactersInPixels;
    const uint32_t bytesPerRow;
    const uint8_t asciiStart;
    const uint8_t asciiEnd;
};

class characterProperties {
  public:
    const uint32_t widthInPixels;
    const uint32_t offsetInBytes;
};

class font {
  public:
    font(const fontProperties &theProperties, const characterProperties *theCharacters, const uint8_t *robotoMedium24BoldPixelData) : properties(theProperties), characters(theCharacters), pixelData(robotoMedium24BoldPixelData){};

    bool charIsInBounds(uint8_t asciiCode) const;
    uint32_t getCharacterWidthInPixels(uint8_t asciiCode) const;
    uint32_t getOffsetInBytes(uint8_t asciiCode) const;
    bool getPixel(uint32_t x, uint32_t y, uint32_t pixelDataOffset) const;
    const fontProperties &properties;
    const characterProperties *characters;
    const uint8_t *pixelData;

#ifndef unitTesting

  private:
#endif
    uint32_t getOffsetInBytes(uint32_t x, uint32_t y) const { return (properties.bytesPerRow * x) + (y / 8); };
    static inline uint8_t getBitIndex(uint32_t y) { return 7 - (y % 8); };
};
