// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <stdint.h>

class lptim {
  public:
    static void initialize();
    static void start(uint32_t timeOutIn4096zTicks);
    static void stop();
    static bool isRunning() { return running; }
    static constexpr uint32_t timerOffset{128U};        // offset to get timing of LPTIM1 accurate, compensating for start/stop overhead
    static uint32_t ticksFromSeconds(uint32_t seconds);
#ifndef unitTesting

  private:
#endif
    static bool running;
};
