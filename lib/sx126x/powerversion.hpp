// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

    enum class powerVersion : uint8_t {
        lowPower,
        highPower,
        nmbrPowerVersions
    };

const char *toString(powerVersion theVersion);