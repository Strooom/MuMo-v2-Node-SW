// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <stdint.h>

class lptim {
  public:
    static void start(uint32_t timeOutIn4096zTicks);
    static void stop();
    static bool isRunning() { return running; }  

#ifndef unitTesting

  private:
#endif
    static bool running;
};
