// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

enum class frameType : uint8_t {
    joinRequest         = 0b000,
    joinAccept          = 0b001,
    unconfirmedDataUp   = 0b010,
    unconfirmedDataDown = 0b011,
    confirmedDataUp     = 0b100,
    confirmedDataDown   = 0b101,
    rejoinRequest       = 0b110,
    proprietary         = 0b111
};

const char* toString(frameType aFrameType);