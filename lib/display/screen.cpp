#include <cmath>          //
#include <cstring>        // strncmp, strncpy
#include <display.hpp>
#include <float.hpp>
#include <font.hpp>
#include <graphics.hpp>
#include <hexascii.hpp>
#include <lucidaconsole12.hpp>
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

bool screen::modified{false};
screenType screen::currentScreenType{screenType::message};

char screen::bigText[numberOfLines][maxTextLength + 1]{};
char screen::smallText[numberOfLines][maxTextLength + 1]{};
char screen::consoleText[numberOfLines2][maxTextLength2 + 1]{};

bool screen::hasUsbPower{false};
uint32_t screen::netwerkStrength{0};
uint32_t screen::batteryLevel{0};
char screen::name[screen::maxNameLength + 1]{0};

void screen::update() {
    display::clearAllPixels();
    switch (currentScreenType) {
        case screenType::measurements:
            showMeasurements();
            break;

        case screenType::message:
            showDeviceInfo();
            break;

        default:
            return;
            break;
    }
    modified = false;
    display::update();
}

void screen::setType(screenType newType) {
    if (currentScreenType != newType) {
        currentScreenType = newType;
        modified          = true;
    }
}

#pragma region consoleScreen

void screen::setText(uint32_t lineIndex, const char* text) {
    if (lineIndex < numberOfLines2) {
        setType(screenType::message);
        if (strncmp(consoleText[lineIndex], text, maxTextLength2) != 0) {
            strlcpy(consoleText[lineIndex], text, maxTextLength2);
            modified = true;
        }
    }
}

void screen::clearAllTexts() {
    for (uint32_t lineIndex = 0; lineIndex < numberOfLines2; lineIndex++) {
        if (strncmp(consoleText[lineIndex], "", maxTextLength2) != 0) {
            strlcpy(consoleText[lineIndex], "", maxTextLength2);
            modified = true;
        }
    }
}

#pragma endregion
#pragma region measurementsScreen

void screen::setText(uint32_t lineIndex, const char* newBigText, const char* newSmallText) {
    if (lineIndex < numberOfLines) {
        setType(screenType::measurements);
        if (strncmp(bigText[lineIndex], newBigText, maxTextLength) != 0) {
            strlcpy(bigText[lineIndex], newBigText, maxTextLength);
            modified = true;
        }
        if (strncmp(smallText[lineIndex], newSmallText, maxTextLength) != 0) {
            strlcpy(smallText[lineIndex], newSmallText, maxTextLength);
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
    if (strncmp(name, newName, maxNameLength) != 0) {
        strlcpy(name, newName, maxNameLength);
        modified = true;
    }
}

#pragma endregion

void screen::showMeasurements() {
    graphics::drawFilledRectangle(ux::marginLeft, 49, display::widthInPixels - ux::marginLeft, 50, graphics::color::black);
    graphics::drawFilledRectangle(ux::marginLeft, 99, display::widthInPixels - ux::marginLeft, 100, graphics::color::black);
    graphics::drawFilledRectangle(ux::marginLeft, 149, display::widthInPixels - ux::marginLeft, 150, graphics::color::black);

    for (uint32_t lineIndex = 0; lineIndex < numberOfLines; lineIndex++) {
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
void screen::showDeviceInfo() {
    for (uint32_t lineIndex = 0; lineIndex < numberOfLines2; lineIndex++) {
        graphics::drawText(2, (182 - (20 * lineIndex)), lucidaConsole12, consoleText[lineIndex]);
    }

    // case screenType::qrcode: {
    //     QRCode qrcode;
    //     uint8_t qrcodeData[qrcode_getBufferSize(2)];

    //     char tmpKeyAsHexAscii[17];
    //     hexAscii::uint64ToHexString(tmpKeyAsHexAscii, uniqueId::get());
    //     qrcode_initText(&qrcode, qrcodeData, 2, ECC_MEDIUM, tmpKeyAsHexAscii);

    //     display::clearAllPixels();

    //     for (uint8_t y = 0; y < qrcode.size; y++) {
    //         for (uint8_t x = 0; x < qrcode.size; x++) {
    //             if (qrcode_getModule(&qrcode, x, y)) {
    //                 graphics::drawFilledRectangle(ux::qrCodeOffset + (x * ux::qrCodeScale), ux::qrCodeOffset + (y * ux::qrCodeScale), ux::qrCodeOffset + ((x + 1) * ux::qrCodeScale), ux::qrCodeOffset + ((y + 1) * ux::qrCodeScale), graphics::color::black);
    //             }
    //         }
    //     }

    //     uint32_t textLength = graphics::getTextwidth(lucidaConsole12, tmpKeyAsHexAscii);
    //     graphics::drawText((display::widthInPixels - textLength) / 2, 10, lucidaConsole12, tmpKeyAsHexAscii);
    // }

    // break;
}
