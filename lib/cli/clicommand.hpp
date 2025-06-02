// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

class cliCommand {
  public:
    static constexpr uint32_t maxCommandLineLength{128};
    static constexpr uint32_t maxCommandLength{4};
    static constexpr uint32_t maxArgumentLength{36};
    static constexpr uint32_t maxNmbrOfArguments{4};

    static constexpr uint32_t help{'?'};


    uint32_t nmbrOfArguments{0};
    char commandAsString[maxCommandLineLength];
    uint32_t commandHash{0};
    char arguments[maxNmbrOfArguments][maxArgumentLength];
};
