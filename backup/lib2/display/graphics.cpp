#include "graphics.h"
#include "display.h"

// Initialize the static variables

void graphics::drawPixel(uint32_t x, uint32_t y, color theColor) {
    if (theColor == color::black) {
        display::setPixel(x, y);
    } else {
        display::clearPixel(x, y);
    }
}

void graphics::drawHorizontalLine(uint32_t xStart, uint32_t xEnd, uint32_t y, color theColor) {
    sort(xStart, xEnd);
    for (uint32_t x = xStart; x <= xEnd; x++) {
        drawPixel(x, y, theColor);
    }
}

void graphics::drawVerticalLine(uint32_t x, uint32_t yStart, uint32_t yEnd, color theColor) {
    sort(yStart, yEnd);
    for (uint32_t y = yStart; y <= yEnd; y++) {
        drawPixel(x, y, theColor);
    }
}

// void graphics::drawLine(uint32_t xStart, uint32_t yStart, uint32_t xEnd, uint32_t yEnd, color theColor) {
//     // Catch vertical and horizontal lines and send them to the dedicated functions
//     if (xStart == xEnd) {
//         drawVerticalLine(xStart, yStart, yEnd, theColor);
//         return;
//     }
//     if (yStart == yEnd) {
//         drawHorizontalLine(xStart, xEnd, yStart, theColor);
//         return;
//     }
//     // Hello Bresenham, it's been 35 years since I first saw you ;-)
//     {
//         int32_t deltaX{xEnd - xStart};
//         int32_t deltaY{yEnd - yStart};
//         int32_t twiceDeltaY                 = 2 * deltaY;
//         int32_t twiceDeltaYMinusTwiceDeltaX = 2 * (deltaY - deltaX);
//         int32_t decisionMaker               = (2 * deltaY) - deltaX;

//         uint32_t buffer_x = xStart;
//         uint32_t buffer_y = yStart;

//         while (buffer_x != xEnd) {
//             drawPixel(buffer_x, buffer_y, theColor);
//             buffer_x++;

//             if (decisionMaker < 0) {
//                 decisionMaker += twiceDeltaY;
//             } else {
//                 buffer_y++;
//                 decisionMaker += twiceDeltaYMinusTwiceDeltaX;
//             }
//         }
//     }
// }

void graphics::drawRectangle(uint32_t xStart, uint32_t yStart, uint32_t xEnd, uint32_t yEnd, color theLineColor) {
    sort(xStart, xEnd);
    sort(yStart, yEnd);

    drawHorizontalLine(xStart, xEnd, yStart, theLineColor);
    drawHorizontalLine(xStart, xEnd, yEnd, theLineColor);
    drawVerticalLine(xStart, yStart, yEnd, theLineColor);
    drawVerticalLine(xEnd, yStart, yEnd, theLineColor);
}

void graphics::drawFilledRectangle(uint32_t xStart, uint32_t yStart, uint32_t xEnd, uint32_t yEnd, color theFillColor) {
    sort(xStart, xEnd);
    sort(yStart, yEnd);

    for (uint32_t yIndex = yStart; yIndex <= yEnd; yIndex++) {
        drawHorizontalLine(xStart, xEnd, yIndex, theFillColor);
    }
}

void graphics::drawBitMap(uint32_t xStart, uint32_t yStart, const bitmap &theBitmap) {
    for (uint32_t x = 0; x < theBitmap.width; x++) {
        for (uint32_t y = 0; y < theBitmap.height; y++) {
            if (theBitmap.getPixel(x, y)) {
                drawPixel(xStart + x, yStart + y, color::black);
            }
        }
    }
}

void graphics::drawCharacter(uint32_t xStart, uint32_t yStart, const font &theFont, uint8_t theCharacter) {
    for (uint32_t x = 0; x < theFont.width; x++) {
        for (uint32_t y = 0; y < theFont.height; y++) {
            if (theFont.getPixel(x, y, theCharacter)) {
                drawPixel(xStart + x, yStart + y, color::black);
            }
        }
    }
}

void graphics::drawText(uint32_t xStart, uint32_t yStart, const font &theFont, const char *theText) {
    uint32_t maxStringLength = ((display::widthInPixels - xStart) / theFont.width) + 1;
    uint32_t charachterIndex{0};
    while (theText[charachterIndex] != '\0' && charachterIndex < maxStringLength) {
        drawCharacter(xStart + (charachterIndex * theFont.width), yStart, theFont, theText[charachterIndex]);
        charachterIndex++;
    }
}

void graphics::sort(uint32_t &c1, uint32_t &c2) {
    if (c1 > c2) {
        uint32_t temp = c1;
        c1            = c2;
        c2            = temp;
    }
}
