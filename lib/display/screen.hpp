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

  private:
    static constexpr uint32_t numberOfLines{4};
    static uint32_t deviceIndex[numberOfLines];         // which device is shown on each line
    static uint32_t channelIndex[numberOfLines];        // which channel of the device is shown on each line

    static constexpr uint32_t maxTextLength{8};
    static char bigText[maxTextLength + 1][numberOfLines];
    static char smallText[maxTextLength + 1][numberOfLines];

    static bool isModified;

    static void getContents();
    static void drawContents();
};