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
#include <uniqueid.hpp>
#include <qrcode.hpp>
#include <hexascii.hpp>

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
                uint32_t leftOffset = ux::mid60 - (graphics::getTextwidth(roboto36bold, bigText[lineIndex]) + roboto36bold.properties.spaceBetweenCharactersInPixels);
                graphics::drawText(leftOffset, ux::marginBottomLarge + ((3U - lineIndex) * 50U), roboto36bold, bigText[lineIndex]);
                graphics::drawText(ux::mid60 + tahoma24bold.properties.spaceBetweenCharactersInPixels, ux::marginBottomSmall + ((3U - lineIndex) * 50U), tahoma24bold, smallText[lineIndex]);
            }

            uint32_t batteryLevel = static_cast<uint32_t>(sensorDeviceCollection::value(1, 1) * 100.0F);
            graphics::drawBatteryIcon(display::widthInPixels - (ux::marginLeft + ux::iconWidth), 2, batteryLevel);
            graphics::drawNetworkSignalStrengthIcon(display::widthInPixels - (ux::marginLeft + (2 * ux::iconWidth)), 2, netwerkStrength);
            if (hasUsbPower) {
                graphics::drawBitMap(display::widthInPixels - (ux::marginLeft + (3 * ux::iconWidth)), 2, usbIcon);
            }
        } break;

        case screenType::message: {
            for (uint32_t lineIndex = 0; lineIndex < numberOfLines2; lineIndex++) {
                graphics::drawText(2, (182 - (20 * lineIndex)), lucidaConsole12, consoleText[lineIndex]);
            }
        } break;

        case screenType::qrcode: {
            QRCode qrcode;
            uint8_t qrcodeData[qrcode_getBufferSize(2)];

            char tmpKeyAsHexAscii[17];
            hexAscii::uint64ToHexString(tmpKeyAsHexAscii, uniqueId::get());
            qrcode_initText(&qrcode, qrcodeData, 2, ECC_MEDIUM, tmpKeyAsHexAscii);

            display::clearAllPixels();

            for (uint8_t y = 0; y < qrcode.size; y++) {
                for (uint8_t x = 0; x < qrcode.size; x++) {
                    if (qrcode_getModule(&qrcode, x, y)) {
                        graphics::drawFilledRectangle(ux::qrCodeOffset + (x * ux::qrCodeScale), ux::qrCodeOffset + (y * ux::qrCodeScale), ux::qrCodeOffset + ((x + 1) * ux::qrCodeScale), ux::qrCodeOffset + ((y + 1) * ux::qrCodeScale), graphics::color::black);
                    }
                }
            }

            uint32_t textLength = graphics::getTextwidth(lucidaConsole12, tmpKeyAsHexAscii);
            graphics::drawText((display::widthInPixels - textLength) / 2, 10, lucidaConsole12, tmpKeyAsHexAscii);
        }

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
        setType(screenType::measurements);
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
