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
        logging::snprintf("line[%d] : was [%s] - [%s]", lineIndex, bigText[lineIndex], smallText[lineIndex]);
        float value       = sensorDeviceCollection::valueAsFloat(deviceIndex[lineIndex], channelIndex[lineIndex]);
        uint32_t decimals = sensorDeviceCollection::channelDecimals(deviceIndex[lineIndex], channelIndex[lineIndex]);
        uint32_t integerPart;

        integerPart = static_cast<uint32_t>(value);        // TODO : take care of rounding io truncating

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
            uint32_t fractionalPart;
            fractionalPart = static_cast<uint32_t>(remainder);        // TODO : take care of rounding io truncating

            snprintf(suffix, 8, ".%d %s", fractionalPart, sensorDeviceCollection::channelUnits(deviceIndex[lineIndex], channelIndex[lineIndex]));
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
        logging::snprintf(", now [%s] - [%s] - %s\n", bigText[lineIndex], smallText[lineIndex], isModified ? "modified" : "not modified");
    }
}

void screen::drawContents() {
    display::initialize();

    graphics::drawFilledRectangle(ux::marginLeft, 49, display::widthInPixels - ux::marginLeft, 50, graphics::color::black);
    graphics::drawFilledRectangle(ux::marginLeft, 99, display::widthInPixels - ux::marginLeft, 100, graphics::color::black);
    graphics::drawFilledRectangle(ux::marginLeft, 149, display::widthInPixels - ux::marginLeft, 150, graphics::color::black);

    for (uint32_t lineIndex = 0; lineIndex < numberOfLines; lineIndex++) {
        graphics::drawText(ux::marginLeft, ux::marginBottomLarge + (lineIndex * 50), roboto36bold, bigText[lineIndex]);
        graphics::drawText(100, ux::marginBottomSmall + (lineIndex * 50), tahoma24bold, smallText[lineIndex]);
    }

    display::update();
    display::goSleep();
}
