#include <screen.hpp>
#include <display.hpp>
#include <graphics.hpp>
#include <ux.hpp>
#include <font.hpp>
#include <stdio.h>                           // snprintf
#include <sensordevicecollection.hpp>        //
#include <cstring>                           // strncmp, strncpy

bool screen::isModified{false};
char screen::bigText[maxTextLength + 1][numberOfLines]{};
char screen::smallText[maxTextLength + 1][numberOfLines]{};

uint32_t screen::deviceIndex[numberOfLines]{0, 1, 1, 2};
uint32_t screen::channelIndex[numberOfLines]{0, 0, 1, 0};

extern font roboto36bold;
extern font tahoma24bold;

void screen::initialize() {

}

void screen::show() {
    isModified = false;
    getContents();

    if (isModified) {
        drawContents();
    }
}

void screen::getContents() {
    for (uint32_t lineIndex = 0; lineIndex < numberOfLines; lineIndex++) {
        float value       = sensorDeviceCollection::valueAsFloat(deviceIndex[lineIndex], channelIndex[lineIndex]);
        uint32_t decimals = sensorDeviceCollection::channelDecimals(deviceIndex[lineIndex], channelIndex[lineIndex]);

        uint32_t integerPart;
        uint32_t fractionalPart;

        integerPart = static_cast<uint32_t>(value);        // TODO : take care of rounding io truncating

        float remainder = value - integerPart;
        for (uint32_t n = 0; n < decimals; n++) {
            remainder *= 10.0F;
        }
        fractionalPart = static_cast<uint32_t>(remainder);        // TODO : take care of rounding io truncating

        char base[8];
        char suffix[8];

        snprintf(base, 8, "%d", integerPart);
        if (strncmp(base, bigText[lineIndex], maxTextLength) != 0) {
            strncpy(bigText[lineIndex], base, maxTextLength);
            isModified = true;
        }
        snprintf(suffix, 8, "%d%s", fractionalPart, sensorDeviceCollection::channelUnits(deviceIndex[lineIndex], channelIndex[lineIndex]));
        if (strncmp(suffix, smallText[lineIndex], maxTextLength) != 0) {
            strncpy(smallText[lineIndex], suffix, maxTextLength);
            isModified = true;
        }
    }
}

void screen::drawContents() {
    display::initialize();

    graphics::drawFilledRectangle(ux::marginLeft, 49, display::widthInPixels - ux::marginLeft, 50, graphics::color::black);
    graphics::drawFilledRectangle(ux::marginLeft, 99, display::widthInPixels - ux::marginLeft, 100, graphics::color::black);
    graphics::drawFilledRectangle(ux::marginLeft, 149, display::widthInPixels - ux::marginLeft, 150, graphics::color::black);

    for (uint32_t lineIndex = 0; lineIndex < numberOfLines; lineIndex++) {
        graphics::drawText(ux::marginLeft, 6 * (lineIndex * 50), roboto36bold, bigText[lineIndex]);
        graphics::drawText(100, 6 * (lineIndex * 50), roboto36bold, smallText[lineIndex]);
    }

    display::update(updateMode::full);
    display::goSleep();
}