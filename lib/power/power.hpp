// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <stdint.h>

class power {
  public:
    static bool isUsbConnected();
    static bool isUsbRemoved();
    static bool hasUsbPower();
    static bool noUsbPowerDelayed() { return (usbPowerOffCount == 0); };

#ifndef unitTesting

  private:
#endif
    static bool usbPower;
    static uint32_t usbPowerOffCount;
    static constexpr uint32_t usbPowerOffCountMax{16U};
#ifdef generic
    static bool mockUsbPower;
#endif
};
