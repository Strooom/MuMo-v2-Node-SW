// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

class cliCommand {
  public:
    static constexpr uint32_t maxCommandLineLength{128};
    static constexpr uint32_t maxCommandOrArgumentLength{36};
    static constexpr uint32_t maxNmbrOfArguments{4};

    const char* shortName;
    const char* longName;
    const uint32_t nmbrOfArguments;
    void (*const handler)();
};
