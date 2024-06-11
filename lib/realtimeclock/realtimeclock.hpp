// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################
#pragma once

#include <stdint.h>
#include <ctime>
#include <buildinfo.hpp>

class realTimeClock {
  public:
    static void initialize();
    static void set(time_t unixTime = buildInfo::buildEpoch);
    static void set(tm brokenDownTime);
    static time_t get();
    static uint8_t* time_tToBytes(time_t input);
    static time_t bytesToTime_t(uint8_t* input);
    static time_t gpsTimeToUnixTime(uint32_t gpsTime);

#ifndef unitTesting

  private:
#endif
    static union convert {
        uint32_t asUint32;
        uint8_t asBytes[4];
    } convertor;
};