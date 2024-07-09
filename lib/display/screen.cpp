#include <cmath>          //
#include <cstring>        // strncmp, strncpy
#include <display.hpp>
#include <float.hpp>
#include <font.hpp>
#include <graphics.hpp>
#include <hexascii.hpp>
#include <lucidaconsole12.hpp>
#include <logo.hpp>
#include <qrcode.hpp>
#include <roboto36b.hpp>
#include <screen.hpp>
#include <sensordevicecollection.hpp>        //
#include <spi.hpp>
#include <stdio.h>         // snprintf
#include <strl.hpp>        // strlcpy
#include <tahoma24b.hpp>
#include <uniqueid.hpp>
#include <usbicon.hpp>
#include <ux.hpp>

#ifndef generic
#include "main.h"
#endif

bool screen::modified{false};
screenType screen::activeScreenType{screenType::logo};

char screen::bigText[nmbrOfMeasurementTextLines][maxMeasurementTextLength + 1]{};
char screen::smallText[nmbrOfMeasurementTextLines][maxMeasurementTextLength + 1]{};
char screen::consoleText[nmbrOfConsoleTextLines][maxConsoleTextLength + 1]{};

bool screen::hasUsbPower{false};
uint32_t screen::netwerkStrength{0};
uint32_t screen::batteryLevel{0};
char screen::uidAsHex[17]{0};
char screen::name[screen::maxNodeNameLength + 1]{0};

void screen::update() {
    if (display::isPresent()) {
        switch (activeScreenType) {
            case screenType::logo:
                showLogo();
                break;

            case screenType::main:
                showMeasurements();
                break;

            case screenType::version:             // intentional fallthrough - all these screenTypes use a console style
            case screenType::hwConfig:            // intentional fallthrough - all these screenTypes use a console style
            case screenType::loraConfig:          // intentional fallthrough - all these screenTypes use a console style
            case screenType::measurements:        // intentional fallthrough - all these screenTypes use a console style
            case screenType::loraStatus:          // intentional fallthrough - all these screenTypes use a console style
                showConsole();
                break;

            case screenType::uid:
                showUid();
                break;

            default:
                break;
        }
        modified = false;
        display::update();
    }
}

void screen::setType(screenType newType) {
    if (activeScreenType != newType) {
        activeScreenType = newType;
        modified         = true;
    }
}

#pragma region consoleScreen

void screen::setText(uint32_t lineIndex, const char* text) {
    if (lineIndex < nmbrOfConsoleTextLines) {
        if (strncmp(consoleText[lineIndex], text, maxConsoleTextLength) != 0) {
            strlcpy(consoleText[lineIndex], text, maxConsoleTextLength);
            modified = true;
        }
    }
}

void screen::clearConsole() {
    for (uint32_t lineIndex = 0; lineIndex < nmbrOfConsoleTextLines; lineIndex++) {
        if (strncmp(consoleText[lineIndex], "", maxConsoleTextLength) != 0) {
            strlcpy(consoleText[lineIndex], "", maxConsoleTextLength);
            modified = true;
        }
    }
}

void screen::setUidAsHex(const char* newUidAsHex) {
    if (memcmp(uidAsHex, newUidAsHex, 16U) != 0) {
        memcpy(uidAsHex, newUidAsHex, 16U);
        modified = true;
    }
}

#pragma endregion
#pragma region measurementsScreen

void screen::setText(uint32_t lineIndex, const char* newBigText, const char* newSmallText) {
    if (lineIndex < nmbrOfMeasurementTextLines) {
        setType(screenType::main);
        if (strncmp(bigText[lineIndex], newBigText, maxMeasurementTextLength) != 0) {
            strlcpy(bigText[lineIndex], newBigText, maxMeasurementTextLength);
            modified = true;
        }
        if (strncmp(smallText[lineIndex], newSmallText, maxMeasurementTextLength) != 0) {
            strlcpy(smallText[lineIndex], newSmallText, maxMeasurementTextLength);
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

void screen::setName(const char* newName) {
    if (strncmp(name, newName, maxNodeNameLength) != 0) {
        strlcpy(name, newName, maxNodeNameLength);
        modified = true;
    }
}

#pragma endregion

void screen::showLogo() {
    display::clearAllPixels();
    uint32_t xOffset;
    uint32_t yOffset;
    xOffset = (display::widthInPixels - (logo.width + ux::logoMargin));
    yOffset = (display::heightInPixels - logo.height) / 2U;
    graphics::drawBitMap(xOffset, yOffset, logo);
}

void screen::showConsole() {
    display::clearAllPixels();
    for (uint32_t lineIndex = 0; lineIndex < nmbrOfConsoleTextLines; lineIndex++) {
        graphics::drawText(2, (182 - (20 * lineIndex)), lucidaConsole12, consoleText[lineIndex]);
    }
}

void screen::showUid() {
    display::clearAllPixels();
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(2)];
    char tmpKeyAsHexAscii[17];
    hexAscii::uint64ToHexString(tmpKeyAsHexAscii, uniqueId::get());
    qrcode_initText(&qrcode, qrcodeData, 2, ECC_MEDIUM, tmpKeyAsHexAscii);
    graphics::drawQrcode(ux::qrCodeXOffset, ux::qrCodeYOffset, qrcode);
    uint32_t width    = graphics::getTextwidth(lucidaConsole12, tmpKeyAsHexAscii);
    uint32_t leftOffset = (display::widthInPixels - width) / 2;
    graphics::drawText(leftOffset, 2, lucidaConsole12, tmpKeyAsHexAscii);
}

void screen::showMeasurements() {
    display::clearAllPixels();
    graphics::drawFilledRectangle(ux::marginLeft, 49, display::widthInPixels - ux::marginLeft, 50, graphics::color::black);
    graphics::drawFilledRectangle(ux::marginLeft, 99, display::widthInPixels - ux::marginLeft, 100, graphics::color::black);
    graphics::drawFilledRectangle(ux::marginLeft, 149, display::widthInPixels - ux::marginLeft, 150, graphics::color::black);

    for (uint32_t lineIndex = 0; lineIndex < nmbrOfMeasurementTextLines; lineIndex++) {
        uint32_t leftOffset = ux::mid60 - (graphics::getTextwidth(roboto36bold, bigText[lineIndex]) + roboto36bold.properties.spaceBetweenCharactersInPixels);
        graphics::drawText(leftOffset, ux::marginBottomLarge + ((3U - lineIndex) * 50U), roboto36bold, bigText[lineIndex]);
        graphics::drawText(ux::mid60 + tahoma24bold.properties.spaceBetweenCharactersInPixels, ux::marginBottomSmall + ((3U - lineIndex) * 50U), tahoma24bold, smallText[lineIndex]);
    }

    uint32_t iconLeft;
    iconLeft     = display::widthInPixels - (ux::marginLeft + ux::iconWidth);
    batteryLevel = static_cast<uint32_t>(sensorDeviceCollection::value(1, 1) * 100.0F);
    graphics::drawBatteryIcon(iconLeft, 2, batteryLevel);
    iconLeft = display::widthInPixels - (ux::marginLeft + ux::iconWidth + ux::iconSpacing);
    graphics::drawNetworkSignalStrengthIcon(iconLeft, 2, netwerkStrength);
    if (hasUsbPower) {
        iconLeft = display::widthInPixels - (ux::marginLeft + ux::iconWidth + ux::iconSpacing + ux::iconSpacing);
        graphics::drawBitMap(iconLeft, 2, usbIcon);
    }
    graphics::drawText(ux::marginLeft, 2, tahoma24bold, name);
}

void screen::waitForUserToRead() {
    if (display::isPresent()) {
#ifndef generic
        HAL_Delay(ux::timeToReaddisplay);
#endif
    }
}