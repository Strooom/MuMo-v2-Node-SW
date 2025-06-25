// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include <stdarg.h>        // required so we can define functions with variable number of arguments
#include <clicommand.hpp>

class cli {
  public:
    static bool hasCommand();
    static void getCommand(cliCommand& aCommand);
    static uint32_t sendResponse(const char* format, ...);

#ifndef unitTesting

  private:
#endif
    static constexpr uint32_t bufferLength{256};
    static constexpr char commandArgumentSeparator{' '};
    static char buffer[bufferLength];        // in this buffer the snprintf is expanded before the contents is being sent to one or more of the destinations

    static void parseCommandLine(cliCommand& aCommand, const char* commandLine);
    static uint32_t countArguments(const char* commandLine);
    static int32_t getSeparatorPosition(const char* commandLine, const uint32_t separatorIndex);
    static void getSegment(char* destination, const char* commandLine, const uint32_t segmentIndex);
    static uint32_t hash(const char* someCommand);
    static uint8_t toLowerCase(const uint8_t characterToConvert);
    static void toLowerCase(char* commandLine);
};
