// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <stdint.h>

class power {
  public:
    static bool isUsbConnected();        // detect insertion of USB cable event
    static bool isUsbRemoved();          // detect removal of USB cable event
    static bool hasUsbPower();

#ifndef unitTesting

  private:
#endif
    static bool usbPower;
#ifdef generic
    static bool mockUsbPower;
#endif
};
