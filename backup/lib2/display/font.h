// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>

class font {
  public:
    font(const uint32_t thePixelHeight, const uint32_t thePixelWidth, const uint8_t* thePixelData) : height(thePixelHeight), width(thePixelWidth), bytesPerRow(((thePixelWidth - 1) / 8) + 1), bytesPerCharacter(height * bytesPerRow), pixelData(thePixelData){};
    bool getPixel(uint32_t x, uint32_t y, uint8_t asciiCode) const;

    const uint32_t height;        // [pixels]
    const uint32_t width;         // [pixels]
    const uint32_t bytesPerRow;
    const uint32_t bytesPerCharacter;
    const uint8_t* pixelData;

    static constexpr uint8_t asciiStart = 32;        // making this static implies that ALL fonts have the characterset : 32..126
    static constexpr uint8_t asciiEnd   = 126;

#ifndef unitTesting

  private:
#endif
    inline static bool isInBounds(uint8_t asciiCode) { return (asciiCode >= asciiStart && asciiCode <= asciiEnd); }
    inline bool isInBounds(uint32_t x, uint32_t y) const { return (x < width && y < height); };
    uint32_t getByteIndex(uint8_t asciiCode) const { return (asciiCode - asciiStart) * bytesPerRow * height; }           // no bounds checking here, private helper function
    uint32_t getByteIndex(uint32_t x, uint32_t y) const { return (((height - 1) - y) * bytesPerRow) + (x / 8); };        // no bounds checking here, private helper function
    inline uint8_t getBitIndex(uint32_t x) const { return 7 - (x % 8); };                                                // no bounds checking here, private helper function
};
