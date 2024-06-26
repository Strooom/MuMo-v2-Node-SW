#include <screen.hpp>
#include <display.hpp>
#include <graphics.hpp>
#include <ux.hpp>
#include <font.hpp>
#include <stdio.h>                           // snprintf
#include <sensordevicecollection.hpp>        //
#include <cstring>                           // strncmp, strncpy
#include <cmath>                             //
#include <inttypes.h>                        // for PRIu32
#include <float.hpp>
#include <spi.hpp>

bool screen::modified{false};
screenType screen::currentScreenType{screenType::message};

char screen::bigText[numberOfLines][maxTextLength + 1]{};
char screen::smallText[numberOfLines][maxTextLength + 1]{};
char screen::consoleText[numberOfLines2][maxTextLength2 + 1]{};

bool screen::hasUsbPower{false};
uint32_t screen::netwerkStrength{0};
uint32_t screen::batteryLevel{0};

extern font roboto36bold;
extern font tahoma24bold;
extern font lucidaConsole12;
extern bitmap usbIcon;

void screen::update() {
    display::clearAllPixels();
    switch (currentScreenType) {
        case screenType::measurements: {
            graphics::drawFilledRectangle(ux::marginLeft, 49, display::widthInPixels - ux::marginLeft, 50, graphics::color::black);
            graphics::drawFilledRectangle(ux::marginLeft, 99, display::widthInPixels - ux::marginLeft, 100, graphics::color::black);
            graphics::drawFilledRectangle(ux::marginLeft, 149, display::widthInPixels - ux::marginLeft, 150, graphics::color::black);

            for (uint32_t lineIndex = 0; lineIndex < numberOfLines; lineIndex++) {
                uint32_t leftOffset = ux::mid - (graphics::getTextwidth(roboto36bold, bigText[lineIndex]) + roboto36bold.properties.spaceBetweenCharactersInPixels);
                graphics::drawText(leftOffset, ux::marginBottomLarge + ((3U - lineIndex) * 50U), roboto36bold, bigText[lineIndex]);
                graphics::drawText(ux::mid + tahoma24bold.properties.spaceBetweenCharactersInPixels, ux::marginBottomSmall + ((3U - lineIndex) * 50U), tahoma24bold, smallText[lineIndex]);
            }

            uint32_t batteryLevel = static_cast<uint32_t>(sensorDeviceCollection::value(1, 1) * 100.0F);
            graphics::drawBatteryIcon(display::widthInPixels - (1 + ux::marginLeft + ux::batteryIconWidth), 2, batteryLevel);
            graphics::drawNetworkSignalStrengthIcon(display::widthInPixels - (1 + ux::marginLeft + ux::marginLeft + ux::batteryIconWidth + ux::netwerkSignalStrengthWidth), 2, netwerkStrength);
            if (hasUsbPower) {
                graphics::drawBitMap(ux::marginLeft + 100U, 2, usbIcon);
            }
        } break;

        case screenType::message: {
            for (uint32_t lineIndex = 0; lineIndex < numberOfLines2; lineIndex++) {
                graphics::drawText(2, (182 - (20 * lineIndex)), lucidaConsole12, consoleText[lineIndex]);
            }
        } break;

        default:
            return;
            break;
    }
    modified = false;
    display::update();
}

#pragma region consoleScreen

void screen::setText(uint32_t lineIndex, const char* text) {
    if (lineIndex < numberOfLines2) {
        if (currentScreenType != screenType::message) {
            currentScreenType = screenType::message;
            modified          = true;
        }
        if (strncmp(consoleText[lineIndex], text, maxTextLength2) != 0) {
            strncpy(consoleText[lineIndex], text, maxTextLength2);
            modified = true;
        }
    }
}

void screen::clearAllTexts() {
    for (uint32_t lineIndex = 0; lineIndex < numberOfLines2; lineIndex++) {
        if (strncmp(consoleText[lineIndex], "", maxTextLength2) != 0) {
            strncpy(consoleText[lineIndex], "", maxTextLength2);
            modified = true;
        }
    }
}

#pragma endregion
#pragma region measurementsScreen

void screen::setText(uint32_t lineIndex, const char* newBigText, const char* newSmallText) {
    if (lineIndex < numberOfLines) {
        if (currentScreenType != screenType::measurements) {
            currentScreenType = screenType::measurements;
            modified          = true;
        }
        if (strncmp(bigText[lineIndex], newBigText, maxTextLength) != 0) {
            strncpy(bigText[lineIndex], newBigText, maxTextLength);
            modified = true;
        }
        if (strncmp(smallText[lineIndex], newSmallText, maxTextLength) != 0) {
            strncpy(smallText[lineIndex], newSmallText, maxTextLength);
            modified = true;
        }
    }
};

void screen::setUsbStatus(const bool newHasUsbPower) {
    if (hasUsbPower != newHasUsbPower) {
        hasUsbPower = newHasUsbPower;
        modified    = true;
    }
}

void screen::setNetworkStatus(const uint32_t newNetworkSignalStrength) {
    if (netwerkStrength != newNetworkSignalStrength) {
        netwerkStrength = newNetworkSignalStrength;
        modified        = true;
    }
}

void screen::setBatteryStatus(uint32_t newBatterylevel) {
    if (batteryLevel != newBatterylevel) {
        batteryLevel = newBatterylevel;
        modified     = true;
    }
}

#pragma endregion
