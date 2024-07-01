// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include <clicommand.hpp>

class cli {
  public:
    static void run();

#ifndef unitTesting

  private:
#endif
    static constexpr char commandArgumentSeparator{' '};
    static constexpr uint32_t maxCommandLineLength{128};
    static constexpr uint32_t nmbrOfCommands{11};
    static cliCommand commands[nmbrOfCommands];
    static char command[cliCommand::maxLongNameLength];
    static uint32_t nmbrOfArguments;
    static char arguments[cliCommand::maxNmbrOfArguments][cliCommand::maxArgumentLength];
    static char error[maxCommandLineLength];

    static void splitCommandLine(char* commandLine);
    static uint32_t countArguments(char* commandLine);
    static int32_t findCommandIndex();
    static int32_t getSeparatorPosition(char* commandLine, uint32_t separatorIndex);
    static void getSegment(char* destination, char* commandLine, uint32_t segmentIndex);
    static void executeCommand(char* commandLine);
};
