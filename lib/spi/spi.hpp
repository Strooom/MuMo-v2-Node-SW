// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <stdint.h>

class spi {
  public:
    static void wakeUp();
    static void goSleep();
    static bool isInitialized() { return initialized; }

#ifndef unitTesting

  private:
#endif
    static bool initialized;
};
