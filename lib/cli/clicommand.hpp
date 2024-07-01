// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

class cliCommand {
  public:
    static constexpr uint32_t maxShortNameLength{8};
    static constexpr uint32_t maxLongNameLength{20};
    static constexpr uint32_t maxArgumentLength{36};
    static constexpr uint32_t maxNmbrOfArguments{4};

    const char shortName[maxShortNameLength];
    const char longName[maxLongNameLength];
    const uint32_t nmbrOfArguments;
    void (*const handler)();
};
