// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <cstdint>

enum class applicationEvent : uint8_t {
    none = 0x00,        // when an eventBuffer underflows, it pops this dummy event
    usbConnected,
    usbRemoved,
    realTimeClockTick,
    lowPowerTimerExpired,
    downlinkApplicationPayloadReceived,
    downlinkMacCommandReceived,
    sx126xCadEnd,
    sx126xTxComplete,
    sx126xRxComplete,
    sx126xTimeout,
};

const char* toString(applicationEvent anEvent);
