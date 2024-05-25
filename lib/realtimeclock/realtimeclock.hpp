// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <stdint.h>
#include <ctime>
#include <buildinfo.hpp>

class realTimeClock {
  public:
    static void set(time_t unixTime = buildInfo::buildEpoch);
    static void set(tm brokenDownTime);
    static time_t get();

#ifndef unitTesting

  private:
#endif

    static time_t unixTimeFromGpsTime(uint32_t gpsTime);
};