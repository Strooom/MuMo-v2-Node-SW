// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>

class screen {
  public:
    static void initialize();
    static void show();

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

    static int integerPart(float value, uint32_t decimals);
    static int fractionalPart(float value, uint32_t decimals);
    static float factorFloat(uint32_t decimals);
    static int factorInt(uint32_t decimals);
    static void buildBigTextString(uint32_t value, uint32_t lineIndex);
    static void buildSmallTextString(uint32_t value, uint32_t decimals, const char* suffix, uint32_t lineIndex);
};