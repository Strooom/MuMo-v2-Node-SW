// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

enum class displaySection : uint32_t {
    s1Rx1C,

    s1Rx2Cl_left,
    s1Rx2C_right,
    s2Rx1C_top,
    s2Rx1C_bottom,

    s4Rx1C_top,
    s4Rx1C_row1,
    s4Rx1C_row2,
    s4Rx1C_bottom,

    s4Rx2C_topLeft,
    s4Rx2C_row1Left,
    s4Rx2C_row2Left,
    s4Rx2C_bottomLeft,
    s4Rx2C_topRight,
    s4Rx2C_row1Right,
    s4Rx2C_row2Right,
    s4Rx2C_bottomRight,
};

const char* toString(displaySection aSection);
