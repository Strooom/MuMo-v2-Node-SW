// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <cstdint>

enum class transmitPolicy : uint8_t {
    asSoonAsPossible,        // transmit measurements immediately
    fillPacket,              // collect enough measurements to transmit a full packet
    timeBased,               // transmit at regular intervals, eg, every hour
};
