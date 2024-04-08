// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>

class linkQuality {
  public:
    static void tick();
    static bool needsUpdate();

  private:
    static constexpr uint32_t rtcTicksBetweenUpdate{2880};        // 1 rtc-tick = 30 seconds, so 2880 ticks = 24 hours
    static uint32_t rtcTicksSinceLastUpdate;
};