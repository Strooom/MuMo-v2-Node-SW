// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <sensorchanneltype.hpp>

class screen {
  public:
    static void show();

  private:
    static constexpr uint32_t maxNumberOfLines{4};
    static constexpr uint32_t maxTextLength{8};

    static sensorChannelType lineTypes[maxNumberOfLines];
    static char bigText[maxTextLength + 1][maxNumberOfLines];
    static char smallText[maxTextLength + 1][maxNumberOfLines];

    static bool isModified;

    static void getContents();
        static void drawContents();
};