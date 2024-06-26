// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <screentype.hpp>

class screen {
  public:
    static bool isModified() { return modified; }
    static void update();
    static void setText(uint32_t lineIndex, const char* text);
    static void clearAllTexts();

    static void setText(uint32_t lineIndex, const char* textBig, const char* textSmall);
    static void setUsbStatus(const bool hasUsbPower);
    static void setNetworkStatus(const uint32_t networkSignalStrength);
    static void setBatteryStatus(uint32_t batterylevel);

    static constexpr uint32_t numberOfLines{3};
    static constexpr uint32_t maxTextLength{8};
    static constexpr uint32_t numberOfLines2{10};
    static constexpr uint32_t maxTextLength2{24};
#ifndef unitTesting

  private:
#endif
    static bool modified;
    static screenType currentScreenType;


    // Properties for screenType::measurements

    static char bigText[numberOfLines][maxTextLength + 1];
    static char smallText[numberOfLines][maxTextLength + 1];
    static bool hasUsbPower;
    static uint32_t netwerkStrength;
    static uint32_t batteryLevel;

    // Properties for screenType::message

    static char consoleText[numberOfLines2][maxTextLength2 + 1];

};