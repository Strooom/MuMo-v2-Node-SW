#include <screen.hpp>
#include <display.hpp>
#include <graphics.hpp>
#include <ux.hpp>
#include <font.hpp>
#include <stdio.h>                           // snprintf
#include <sensordevicecollection.hpp>        //
#include <cstring>                           // strncmp, strncpy
#include <gpio.hpp>
#include <logging.hpp>
#include <cmath>

bool screen::isModified{false};
char screen::bigText[numberOfLines][maxTextLength + 1]{};
char screen::smallText[numberOfLines][maxTextLength + 1]{};

uint32_t screen::deviceIndex[numberOfLines]{1, 3, 2, 2};
uint32_t screen::channelIndex[numberOfLines]{0, 0, 1, 0};

extern font roboto36bold;
extern font tahoma24bold;

void screen::initialize() {
}

void screen::show() {
    getContents();

    if (isModified) {
        drawContents();
    }
}

void screen::getContents() {
    isModified = false;
    for (uint32_t lineIndex = 0; lineIndex < numberOfLines; lineIndex++) {
        float value       = sensorDeviceCollection::valueAsFloat(deviceIndex[lineIndex], channelIndex[lineIndex]);
        uint32_t decimals = sensorDeviceCollection::channelDecimals(deviceIndex[lineIndex], channelIndex[lineIndex]);
        int integerPart;

        if (decimals > 0) {
            integerPart = static_cast<int>(value);
        } else {
            integerPart = static_cast<int>(round(value));
        }

        float remainder = value - integerPart;
        for (uint32_t n = 0; n < decimals; n++) {
            remainder *= 10.0F;
        }

        char base[8];
        snprintf(base, 8, "%d", integerPart);
        if (strncmp(base, bigText[lineIndex], maxTextLength) != 0) {
            strncpy(bigText[lineIndex], base, maxTextLength);
            isModified = true;
        }

        char suffix[8];
        if (decimals > 0) {
            int fractionalPart;
            fractionalPart = static_cast<int>(round(remainder));

            snprintf(suffix, 8, "%d %s", fractionalPart, sensorDeviceCollection::channelUnits(deviceIndex[lineIndex], channelIndex[lineIndex]));
            if (strncmp(suffix, smallText[lineIndex], maxTextLength) != 0) {
                strncpy(smallText[lineIndex], suffix, maxTextLength);
                isModified = true;
            }
        } else {
            snprintf(suffix, 8, "%s", sensorDeviceCollection::channelUnits(deviceIndex[lineIndex], channelIndex[lineIndex]));
            if (strncmp(suffix, smallText[lineIndex], maxTextLength) != 0) {
                strncpy(smallText[lineIndex], suffix, maxTextLength);
                isModified = true;
            }
        }
    }
}

void screen::drawContents() {
    display::initialize();

    graphics::drawFilledRectangle(ux::marginLeft, 49, display::widthInPixels - ux::marginLeft, 50, graphics::color::black);
    graphics::drawFilledRectangle(ux::marginLeft, 99, display::widthInPixels - ux::marginLeft, 100, graphics::color::black);
    graphics::drawFilledRectangle(ux::marginLeft, 149, display::widthInPixels - ux::marginLeft, 150, graphics::color::black);

    for (uint32_t lineIndex = 0; lineIndex < numberOfLines; lineIndex++) {
        uint32_t leftOffset = ux::mid - (graphics::getTextwidth(roboto36bold, bigText[lineIndex]) + roboto36bold.properties.spaceBetweenCharactersInPixels);
        graphics::drawText(leftOffset, ux::marginBottomLarge + (lineIndex * 50), roboto36bold, bigText[lineIndex]);
        graphics::drawText(ux::mid + tahoma24bold.properties.spaceBetweenCharactersInPixels, ux::marginBottomSmall + (lineIndex * 50), tahoma24bold, smallText[lineIndex]);
    }

    display::update();
    display::goSleep();
}
