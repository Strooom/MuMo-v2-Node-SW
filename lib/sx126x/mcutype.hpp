// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

    enum class mcuType : uint8_t {
        lowPower,
        highPower,
        nmbrMcuTypes
    };

constexpr uint32_t nmbrMcuTypes{static_cast<uint32_t>(mcuType::nmbrMcuTypes)};
const char *toString(mcuType theVersion);