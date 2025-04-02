// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <font.hpp>
#include <bitmap.hpp>

class graphics {
  public:
    enum class color : uint32_t { black = 0,          // when a pixel is 0 in the displayBuffer, it is black on the display
                                  white = 1 };        // when a pixel is 1 in the displayBuffer, it is white on the display

    enum class colorMode : uint32_t { normal   = 0,          // black on white background
                                      inverted = 1 };        // white on black background

    static void drawPixel(const uint32_t x, const uint32_t y, const color theColor);
    static void drawHorizontalLine(const uint32_t xStart, const uint32_t xEnd, const uint32_t y, const color theColor);
    static void drawVerticalLine(const uint32_t x, const uint32_t yStart, const uint32_t yEnd, const color theColor);
    static void drawLine(uint32_t xStart, uint32_t yStart, uint32_t xEnd, uint32_t yEnd, const color theColor);
    static void drawRectangle(uint32_t xStart, uint32_t yStart, const uint32_t xEnd, uint32_t yEnd, const color theLineColor);
    static void drawFilledRectangle(uint32_t xStart, uint32_t yStart, uint32_t xEnd, uint32_t yEnd, const color theFillColor);
    static void drawCircle(const uint32_t x, const uint32_t y, const uint32_t radius, const color theColor);
    static void drawFilledCircle(const uint32_t x, const uint32_t y, const uint32_t radius, const color theColor);
    static void drawBitMap(const uint32_t xStart, const uint32_t yStart, const bitmap& theBitmap);
    static void drawCharacter(const uint32_t xStart, const uint32_t yStart, const font& theFont, const uint8_t theCharacter);
    static void drawText(const uint32_t xStart, const uint32_t yStart, const font& theFont, const char* theText);
    static uint32_t getTextwidth(const font& theFont, const char* theText);

    static void drawBatteryIcon(const uint32_t xStart, const uint32_t yStart, const uint32_t level);                      // level goes from 0 to 100
    static void drawNetworkSignalStrengthIcon(const uint32_t xStart, const uint32_t yStart, const uint32_t level);        // level goes from 0 to 100
    static void drawQrCode(const uint32_t xStart, const uint32_t yStart);


#ifndef unitTesting

  private:
#endif
    static void sort(uint32_t& c1, uint32_t& c2);
};
