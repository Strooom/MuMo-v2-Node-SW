// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <screentype.hpp>

class screen {
  public:
    static void show(screenType theScreenType);
    static void setText(uint32_t lineIndex, const char* text);
    static void clearAllTexts();

    static constexpr uint32_t numberOfLines{4};
    static constexpr uint32_t maxTextLength{8};
    static constexpr uint32_t numberOfLines2{10};
    static constexpr uint32_t maxTextLength2{24};
#ifndef unitTesting

  private:
#endif
    // Properties for screenType::measurements

    static uint32_t deviceIndex[numberOfLines];         // which device is shown on each line
    static uint32_t channelIndex[numberOfLines];        // which channel of the device is shown on each line

    static char bigText[numberOfLines][maxTextLength + 1];
    static char smallText[numberOfLines][maxTextLength + 1];

    // Properties for screenType::message
    static char consoleText[numberOfLines2][maxTextLength2 + 1];




    static bool isModified;

    static void getContents();
    static void drawContents();

    static void buildBigTextString(int32_t value, uint32_t lineIndex);
    static void buildSmallTextString(uint32_t value, uint32_t decimals, const char* suffix, uint32_t lineIndex);

    static void showMeasurements();
    static void showMessage();
};