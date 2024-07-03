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
    static constexpr uint32_t nmbrOfCommands{12};
    static cliCommand commands[nmbrOfCommands];

#ifndef unitTesting

  private:
#endif
    static constexpr char commandArgumentSeparator{' '};
    static char command[cliCommand::maxCommandOrArgumentLength];
    static uint32_t nmbrOfArguments;
    static char arguments[cliCommand::maxNmbrOfArguments][cliCommand::maxCommandOrArgumentLength];
    static char error[cliCommand::maxCommandLineLength];

    static void splitCommandLine(const char* commandLine);
    static uint32_t countArguments(const char* commandLine);
    static int32_t findCommandIndex();
    static int32_t getSeparatorPosition(const char* commandLine, const uint32_t separatorIndex);
    static void getSegment(char* destination, const char* commandLine, const uint32_t segmentIndex);
    static void executeCommand(const char* commandLine);
};
