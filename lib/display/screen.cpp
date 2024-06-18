#include <screen.hpp>
#include <display.hpp>
#include <graphics.hpp>
#include <ux.hpp>
#include <font.hpp>
#include <stdio.h>                           // snprintf
#include <sensordevicecollection.hpp>        //
#include <cstring>                           // strncmp, strncpy
#include <cmath>
#include <inttypes.h>        // for PRIu32
#include <float.hpp>
#include <spi.hpp>

bool screen::isModified{false};
char screen::bigText[numberOfLines][maxTextLength + 1]{};
char screen::smallText[numberOfLines][maxTextLength + 1]{};
char screen::consoleText[numberOfLines2][maxTextLength2 + 1]{};

uint32_t screen::deviceIndex[numberOfLines]{1, 3, 2, 2};
uint32_t screen::channelIndex[numberOfLines]{0, 0, 1, 0};

extern font roboto36bold;
extern font tahoma24bold;
extern font lucidaConsole12;

void screen::show(screenType theScreenType) {
    switch (theScreenType) {
        case screenType::measurements:
            showMeasurements();
            break;
        case screenType::message:
            showMessage();
            break;
    }
}

void screen::showMeasurements() {
    getContents();
    if (isModified) {
        drawContents();
    }
}

void screen::setText(uint32_t lineIndex, const char* text) {
    if (lineIndex < numberOfLines2) {
        strncpy(consoleText[lineIndex], text, maxTextLength2);
    }
}

void screen::clearAllTexts() {
    for (uint32_t lineIndex = 0; lineIndex < numberOfLines2; lineIndex++) {
        strncpy(consoleText[lineIndex], "", maxTextLength2);
    }
}

void screen::showMessage() {
    display::clearAllPixels();
    for (uint32_t lineIndex = 0; lineIndex < numberOfLines2; lineIndex++) {
        graphics::drawText(2, (182 - (20 * lineIndex)), lucidaConsole12, consoleText[lineIndex]);
    }
    display::update();

}

void screen::getContents() {
    isModified = false;
    for (uint32_t lineIndex = 1; lineIndex < numberOfLines; lineIndex++) {
        float value       = sensorDeviceCollection::value(deviceIndex[lineIndex], channelIndex[lineIndex]);
        uint32_t decimals = sensorDeviceCollection::decimals(deviceIndex[lineIndex], channelIndex[lineIndex]);

        buildBigTextString(integerPart(value, decimals), lineIndex);
        buildSmallTextString(fractionalPart(value, decimals), decimals, sensorDeviceCollection::units(deviceIndex[lineIndex], channelIndex[lineIndex]), lineIndex);
    }
}

void screen::drawContents() {
    display::clearAllPixels();
    graphics::drawFilledRectangle(ux::marginLeft, 49, display::widthInPixels - ux::marginLeft, 50, graphics::color::black);
    graphics::drawFilledRectangle(ux::marginLeft, 99, display::widthInPixels - ux::marginLeft, 100, graphics::color::black);
    graphics::drawFilledRectangle(ux::marginLeft, 149, display::widthInPixels - ux::marginLeft, 150, graphics::color::black);

    for (uint32_t lineIndex = 1; lineIndex < numberOfLines; lineIndex++) {
        uint32_t leftOffset = ux::mid - (graphics::getTextwidth(roboto36bold, bigText[lineIndex]) + roboto36bold.properties.spaceBetweenCharactersInPixels);
        graphics::drawText(leftOffset, ux::marginBottomLarge + (lineIndex * 50), roboto36bold, bigText[lineIndex]);
        graphics::drawText(ux::mid + tahoma24bold.properties.spaceBetweenCharactersInPixels, ux::marginBottomSmall + (lineIndex * 50), tahoma24bold, smallText[lineIndex]);
    }

    uint32_t batteryLevel = static_cast<uint32_t>(sensorDeviceCollection::value(1, 1) * 100.0F);
    graphics::drawBatteryIcon(display::widthInPixels - (1 + ux::marginLeft + ux::batteryIconWidth), 2, batteryLevel);
    graphics::drawNetworkSignalStrengthIcon(display::widthInPixels - (1 + ux::marginLeft + ux::marginLeft + ux::batteryIconWidth + ux::netwerkSignalStrengthWidth), 2, 50);
    display::update();
}

void screen::buildBigTextString(int32_t value, uint32_t lineIndex) {
    char base[8];
    snprintf(base, 8, "%" PRId32, value);
    if (strncmp(base, bigText[lineIndex], maxTextLength) != 0) {
        strncpy(bigText[lineIndex], base, maxTextLength);
        isModified = true;
    }
}

void screen::buildSmallTextString(uint32_t value, uint32_t decimals, const char* suffix, uint32_t lineIndex) {
    char suffixString[8];
    if (decimals > 0) {
        snprintf(suffixString, 8, "%" PRIu32 " %s", value, suffix);
        if (strncmp(suffixString, smallText[lineIndex], maxTextLength) != 0) {
            strncpy(smallText[lineIndex], suffixString, maxTextLength);
            isModified = true;
        }
    } else {
        snprintf(suffixString, 8, "%s", suffix);
        if (strncmp(suffixString, smallText[lineIndex], maxTextLength) != 0) {
            strncpy(smallText[lineIndex], suffixString, maxTextLength);
            isModified = true;
        }
    }
}
