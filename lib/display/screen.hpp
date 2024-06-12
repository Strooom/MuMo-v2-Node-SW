// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>

class screen {
  public:
    static void showMeasurements();
    static void showMessage(const char* line1, const char* line2);
    static void setText(uint32_t lineIndex, const char* text);
    static uint32_t lineBottom(uint32_t lineIndex) { return 180 - (20 * lineIndex); }
    static uint32_t lineTop(uint32_t lineIndex) { return 180 - (20 * (lineIndex + 1)); }

#ifndef unitTesting

  private:
#endif
    static constexpr uint32_t numberOfLines{4};
    static uint32_t deviceIndex[numberOfLines];         // which device is shown on each line
    static uint32_t channelIndex[numberOfLines];        // which channel of the device is shown on each line

    static constexpr uint32_t maxTextLength{8};
    static char bigText[numberOfLines][maxTextLength + 1];
    static char smallText[numberOfLines][maxTextLength + 1];

    static bool isModified;

    static void getContents();
    static void drawContents();

    static void buildBigTextString(int32_t value, uint32_t lineIndex);
    static void buildSmallTextString(uint32_t value, uint32_t decimals, const char* suffix, uint32_t lineIndex);
};