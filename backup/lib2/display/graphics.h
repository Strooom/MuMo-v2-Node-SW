// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include "font.h"
#include "bitmap.h"

class graphics {
  public:
    enum class color : uint32_t { black = 0,                 // when a pixel is 0 in the displayBuffer, it is black on the display
                                  white = 1 };               // when a pixel is 1 in the displayBuffer, it is white on the display

    enum class colorMode : uint32_t { normal   = 0,          // black on white background
                                      inverted = 1 };        // white on black background

    static void drawPixel(uint32_t x, uint32_t y, color theColor);
    static void drawHorizontalLine(uint32_t xStart, uint32_t xEnd, uint32_t y, color theColor);
    static void drawVerticalLine(uint32_t x, uint32_t yStart, uint32_t yEnd, color theColor);
    static void drawRectangle(uint32_t xStart, uint32_t yStart, uint32_t xEnd, uint32_t yEnd, color theLineColor);
    static void drawFilledRectangle(uint32_t xStart, uint32_t yStart, uint32_t xEnd, uint32_t yEnd, color theFillColor);

    //static void drawLine(uint32_t xStart, uint32_t yStart, uint32_t xEnd, uint32_t yEnd, color theColor);

    static void drawBitMap(uint32_t xStart, uint32_t yStart, const bitmap& theBitmap);
    static void drawCharacter(uint32_t xStart, uint32_t yStart, const font& theFont, uint8_t theCharacter);
    static void drawText(uint32_t xStart, uint32_t yStart, const font& theFont, const char* theText);

#ifndef unitTesting

  private:
#endif
    static void sort(uint32_t& c1, uint32_t& c2);
};
