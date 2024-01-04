// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

enum class spreadingFactor : uint8_t {
    SF7  = 0x07,
    SF8  = 0x08,
    SF9  = 0x09,
    SF10 = 0x0A,
    SF11 = 0x0B,
    SF12 = 0x0C
};

const char* toString(spreadingFactor aSpreadingFactor);