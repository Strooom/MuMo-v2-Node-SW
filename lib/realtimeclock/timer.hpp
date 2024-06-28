// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################
#pragma once

#include <stdint.h>
#include <ctime>

class timer {
  public:
    void start(uint32_t periodInSeconds);
    void stop();
    bool expired();

#ifndef unitTesting

  private:
#endif
    time_t startTime;
    uint32_t periodInSeconds;
    bool running;
};