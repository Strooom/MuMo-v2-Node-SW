// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

enum class applicationEvent : uint8_t {
    usbConnected,
    usbRemoved,
    realTimeClockTick,
    lowPowerTimerExpired,
    applicationButtonPressed,
    downlinkApplicationPayloadReceived,
    downlinkMacCommandReceived,
    sx126xCadEnd,
    sx126xTxComplete,
    sx126xRxComplete,
    sx126xTimeout
};

static constexpr uint8_t nmbrOfApplicationEvents = static_cast<uint8_t>(applicationEvent::sx126xTimeout) + 1;

const char* toString(applicationEvent anEvent);
