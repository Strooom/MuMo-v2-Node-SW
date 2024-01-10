// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <sensordevicetype.hpp>

class screen {
  public:
    static void show();

  private:
    static constexpr uint32_t numberOfLines{4};
    static constexpr uint32_t maxTextLength{8};

    static sensorDeviceType Device[numberOfLines];
    static uint32_t lineChannel[numberOfLines];
    static char bigText[maxTextLength + 1][numberOfLines];
    static char smallText[maxTextLength + 1][numberOfLines];

    static bool isModified;

    static void getContents();
    static void drawContents();
};