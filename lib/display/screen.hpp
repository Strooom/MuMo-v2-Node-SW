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
    static void clearConsole();

    static void setType(screenType newType);

    static void setText(uint32_t lineIndex, const char* textBig, const char* textSmall);
    static void setUsbStatus(const bool hasUsbPower);
    static void setNetworkStatus(const uint32_t networkSignalStrength);
    static void setBatteryStatus(uint32_t batteryLevel);
    static void setName(const char* newName);
    static void setUidAsHex(const char* newUidAsHex);

    static void waitForUserToRead();

    static constexpr uint32_t nmbrOfMeasurementTextLines{3U};
    static constexpr uint32_t maxMeasurementTextLength{8U};
    static constexpr uint32_t nmbrOfConsoleTextLines{10U};
    static constexpr uint32_t maxConsoleTextLength{30U};
    static constexpr uint32_t maxNodeNameLength{8U};
#ifndef unitTesting

  private:
#endif
    static bool modified;
    static screenType activeScreenType;

    // Properties for screenType::measurements

    static char bigText[nmbrOfMeasurementTextLines][maxMeasurementTextLength + 1U];
    static char smallText[nmbrOfMeasurementTextLines][maxMeasurementTextLength + 1U];
    static bool hasUsbPower;
    static uint32_t networkStrength;
    static uint32_t batteryLevel;
    static char uidAsHex[17];
    static char name[maxNodeNameLength + 1U];

    // Properties for screenType::message

    static char consoleText[nmbrOfConsoleTextLines][maxConsoleTextLength + 1U];

    static void showLogo();
    static void showConsole();
    static void showUid();
    static void showMeasurements();
    
};