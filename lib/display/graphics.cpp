#include <ux.hpp>
#include "graphics.hpp"
#include "display.hpp"

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

void graphics::drawLine(uint32_t xStart, uint32_t yStart, uint32_t xEnd, uint32_t yEnd, color theColor) {
    // Catch vertical and horizontal lines and send them to the dedicated functions
    if (xStart == xEnd) {
        drawVerticalLine(xStart, yStart, yEnd, theColor);
        return;
    }
    if (yStart == yEnd) {
        drawHorizontalLine(xStart, xEnd, yStart, theColor);
        return;
    }
    // Hello Bresenham, it's been 35 years since I first saw you ;-)
    {
        int dx  = xEnd - xStart >= 0 ? xEnd - xStart : xStart - xEnd;
        int sx  = xStart < xEnd ? 1 : -1;
        int dy  = yEnd - yStart <= 0 ? yEnd - yStart : yStart - yEnd;
        int sy  = yStart < yEnd ? 1 : -1;
        int err = dx + dy;

        while ((xStart != xEnd) && (yStart != yEnd)) {
            drawPixel(xStart, yStart, theColor);
            if (2 * err >= dy) {
                err += dy;
                xStart += sx;
            }
            if (2 * err <= dx) {
                err += dx;
                yStart += sy;
            }
        }
    }
}

void graphics::drawCircle(uint32_t x, uint32_t y, uint32_t radius, color theColor) {
    /* Bresenham algorithm */
    int x_pos = -radius;
    int y_pos = 0;
    int err   = 2 - 2 * radius;
    int e2;

    do {
        drawPixel(x - x_pos, y + y_pos, theColor);
        drawPixel(x + x_pos, y + y_pos, theColor);
        drawPixel(x + x_pos, y - y_pos, theColor);
        drawPixel(x - x_pos, y - y_pos, theColor);
        e2 = err;
        if (e2 <= y_pos) {
            err += ++y_pos * 2 + 1;
            if (-x_pos == y_pos && e2 <= x_pos) {
                e2 = 0;
            }
        }
        if (e2 > x_pos) {
            err += ++x_pos * 2 + 1;
        }
    } while (x_pos <= 0);
}

void graphics::drawFilledCircle(uint32_t x, uint32_t y, uint32_t radius, color theColor) {
    int x_pos = -radius;
    int y_pos = 0;
    int err   = 2 - 2 * radius;
    int e2;

    do {
        drawHorizontalLine(x + x_pos, x - x_pos, y + y_pos, theColor);
        drawHorizontalLine(x + x_pos, x - x_pos, y - y_pos, theColor);
        e2 = err;
        if (e2 <= y_pos) {
            err += ++y_pos * 2 + 1;
            if (-x_pos == y_pos && e2 <= x_pos) {
                e2 = 0;
            }
        }
        if (e2 > x_pos) {
            err += ++x_pos * 2 + 1;
        }
    } while (x_pos <= 0);
}

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

void graphics::drawCharacter(const uint32_t xStart, const uint32_t yStart, const font &theFont, const uint8_t theCharacter) {
    if (!theFont.charIsInBounds(theCharacter)) {
        return;
    }
    uint32_t widthInPixels          = theFont.getCharacterWidthInPixels(theCharacter);
    uint32_t characterOffsetInBytes = theFont.getOffsetInBytes(theCharacter);

    for (uint32_t x = 0; x < widthInPixels; x++) {
        for (uint32_t y = 0; y < theFont.properties.heightInPixels; y++) {
            if (theFont.getPixel(x, y, characterOffsetInBytes)) {
                drawPixel(xStart + x, yStart + y, color::black);
            }
        }
    }
}

void graphics::drawText(const uint32_t xStart, const uint32_t y, const font &theFont, const char *theText) {
    uint32_t xPos{xStart};
    uint32_t charachterIndex{0};
    while (theText[charachterIndex] != '\0' && xPos < display::widthInPixels) {
        drawCharacter(xPos, y, theFont, theText[charachterIndex]);
        xPos = xPos + theFont.getCharacterWidthInPixels(theText[charachterIndex]) + theFont.properties.spaceBetweenCharactersInPixels;
        charachterIndex++;
    }
}

void graphics::drawBatteryIcon(const uint32_t xStart, const uint32_t yStart, const uint32_t level) {
    for (uint32_t i = 0; i < ux::batteryIconThickness; i++) {
        drawRectangle(xStart + i, yStart + i, xStart + ux::batteryIconWidth - i, yStart + ux::batteryIconHeight - i, color::black);
    }
    drawFilledRectangle(xStart + ux::batteryIconTipLeft, yStart + ux::batteryIconHeight, xStart + ux::batteryIconTipLeft + ux::batteryIconTipWidth, yStart + ux::batteryIconHeight + ux::batteryIconTipHeight, color::black);
    uint32_t fill = (ux::batteryIconGaugeHeight * level) / 100;
    drawFilledRectangle(xStart, yStart, xStart + ux::batteryIconWidth, yStart + fill, color::black);
}

void graphics::drawNetworkSignalStrengthIcon(const uint32_t xStart, const uint32_t yStart, const uint32_t level) {
    for (uint32_t i = 0; i < 4; i++) {
        if (level >= (i + 1) * 25) {
            drawFilledRectangle(xStart + (i * ux::netwerkSignalStrengthBarWidth), yStart, xStart + (i * ux::netwerkSignalStrengthBarWidth) + ux::netwerkSignalStrengthBarWidth, yStart + ux::netwerkSignalStrengthBaseHeight + (i * ux::netwerkSignalStrengthStepHeight), color::black);
        } else {
            drawRectangle(xStart + (i * ux::netwerkSignalStrengthBarWidth), yStart, xStart + (i * ux::netwerkSignalStrengthBarWidth) + ux::netwerkSignalStrengthBarWidth, yStart + ux::netwerkSignalStrengthBaseHeight + (i * ux::netwerkSignalStrengthStepHeight), color::black);
            drawRectangle(xStart + (i * ux::netwerkSignalStrengthBarWidth) + 1, yStart + 1, xStart + (i * ux::netwerkSignalStrengthBarWidth) + ux::netwerkSignalStrengthBarWidth - 1, yStart + ux::netwerkSignalStrengthBaseHeight + (i * ux::netwerkSignalStrengthStepHeight) - 1, color::black);
        }
    }
}

uint32_t graphics::getTextwidth(const font &theFont, const char *theText) {
    uint32_t result{0};
    uint32_t charachterIndex{0};
    while (theText[charachterIndex] != '\0') {
        result = result + theFont.getCharacterWidthInPixels(theText[charachterIndex]);
        charachterIndex++;
        if (theText[charachterIndex] != '\0') {
            result = result + theFont.properties.spaceBetweenCharactersInPixels;
        }
    }
    return result;
}

void graphics::sort(uint32_t &c1, uint32_t &c2) {
    if (c1 > c2) {
        uint32_t temp = c1;
        c1            = c2;
        c2            = temp;
    }
}
