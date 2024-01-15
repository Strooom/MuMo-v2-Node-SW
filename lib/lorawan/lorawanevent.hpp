// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

enum class loRaWanEvent : uint8_t {
    none = 0x00,             // when an eventBuffer underflows, it pops this dummy event
    sx126xCadEnd,            //
    sx126xTxComplete,        // transmit complete interrupt from the SX126x
    sx126xRxComplete,        // receive complete interrupt from the SX126x
    sx126xTimeout,           // interrupt from the SX126x timeout
    timeOut,                 // interrupt from the LPTIM1
};

const char* toString(loRaWanEvent anEvent);