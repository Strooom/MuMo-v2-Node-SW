// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <stdint.h>
#include <ctime>

class realTimeClock {
  public:
    static void set();        // from build time - not very accurate but better than 1-jan-2000 00:00:00
    static void set(time_t unixTime);
    static void set(tm brokenDownTime);
    static time_t get();

#ifndef unitTesting

  private:
#endif

    static time_t unixTimeFromGpsTime(uint32_t gpsTime);
};