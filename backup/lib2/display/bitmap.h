// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>



class bitmap {
  public:
    bitmap(const uint32_t theWidth, const uint32_t theHeight, const uint8_t* thePixelData) : width(theWidth), height(theHeight), bytesPerRow(((theWidth - 1) / 8) + 1), pixelData(thePixelData){};
    bool getPixel(uint32_t x, uint32_t y) const;

    const uint32_t width;
    const uint32_t height;
    const uint32_t bytesPerRow;

#ifndef unitTesting

  private:
#endif
    inline bool isInBounds(uint32_t x, uint32_t y) const { return (x < width && y < height); };
    inline uint8_t getBitIndex(uint32_t x) const { return 7 - (x % 8); };                                                       // no bounds checking here, private helper function
    inline uint32_t getByteIndex(uint32_t x, uint32_t y) const { return (((height - 1) - y) * bytesPerRow) + (x / 8); };        // no bounds checking here, private helper function
    const uint8_t* const pixelData;
};