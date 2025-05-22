// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

    enum class radioType : uint8_t {
        lowPower,
        highPower,
        nmbrRadioTypes
    };

constexpr uint32_t nmbrRadioTypes{static_cast<uint32_t>(radioType::nmbrRadioTypes)};
const char *toString(radioType theVersion);