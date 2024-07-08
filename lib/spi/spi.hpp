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
    static constexpr uint32_t spiDisplayTimeout{20U}; // CPU clock = 16 MHz, Divider = 2 -> 8MHz SPI. Sending 5K bytes takes ~5 ms. Taking 20 ms to give it some margin

#ifndef unitTesting

  private:
#endif
    static bool initialized;
};
