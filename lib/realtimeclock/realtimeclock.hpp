// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################
#pragma once

#include <cstdint>
#include <ctime>
#include <buildinfo.hpp>

class realTimeClock {
  public:
    static void initialize();
    static void set(time_t unixTime = buildInfo::buildEpoch);
    static void set(tm brokenDownTime);
    static time_t get();
    static uint8_t* bytesFromTime_t(time_t input);
    static time_t time_tFromBytes(uint8_t* input);
    static time_t unixTimeFromGpsTime(uint32_t gpsTime);
    static bool needsSync();
    static constexpr uint32_t rtcSyncPeriod = 60 * 60 * 24 * 7;

#ifndef unitTesting

  private:
#endif
    static union convert {
        uint32_t asUint32;
        uint8_t asBytes[4];
    } convertor;

    static time_t lastSyncTime;
};