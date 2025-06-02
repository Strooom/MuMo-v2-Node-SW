// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include <clicommand.hpp>

class cli {
  public:
    static bool hasCommand();
    static void getCommand(cliCommand& aCommand);
    static void sendResponse(const char* response);

#ifndef unitTesting

  private:
#endif
    static constexpr char commandArgumentSeparator{' '};

    static void parseCommandLine(cliCommand& aCommand, const char* commandLine);
    static uint32_t countArguments(const char* commandLine);
    static int32_t getSeparatorPosition(const char* commandLine, const uint32_t separatorIndex);
    static void getSegment(char* destination, const char* commandLine, const uint32_t segmentIndex);
    static uint32_t hash(const char* someCommand);
    static uint8_t toLowerCase(const uint8_t characterToConvert);
    static void toLowerCase(char* commandLine);
};
