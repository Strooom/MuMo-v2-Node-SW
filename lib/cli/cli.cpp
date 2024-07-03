#include <cli.hpp>
#include <uart.hpp>
#include <cstring>        // for strcmp
#include <clicommand.hpp>

void setDevAddr() {
    // TODO : implement this
}

void showHelp() {
    // TODO : implement this
}

char cli::command[cliCommand::maxCommandOrArgumentLength]{};
uint32_t cli::nmbrOfArguments{0};
char cli::arguments[cliCommand::maxNmbrOfArguments][cliCommand::maxCommandOrArgumentLength]{};
char cli::error[cliCommand::maxCommandLineLength]{};

cliCommand cli::commands[nmbrOfCommands]{
    {"?", "help", 0, showHelp},
    {"ssw", "show-software", 0, nullptr},        // version, build date, build time
    {"shw", "show-hardware", 0, nullptr},        // settings, battery-voltage
    {"ss", "show-sensors", 0, nullptr},          // list sensors present and their channel settings
    {"sl", "show-lorawan", 0, nullptr},
    {"sda", "set-devaddr", 1, setDevAddr},
    {"snk", "set-nwkkey", 1, nullptr},
    {"sak", "set-appkey", 1, nullptr},
    {"rml", "reset-mac-layer", 0, nullptr},
    {"sn", "set-name", 0, nullptr},
    {"sls", "set-logging-sources", 1, nullptr},
    {"res", "restart", 0, nullptr}};        // will restart the software and jump to the bootloader for firmware update

void cli::run() {
    char commandLine[cliCommand::maxCommandLineLength];
    if (uart2::commandCount() > 0) {
        uart2::read(commandLine);
        executeCommand(commandLine);
    }
}

uint32_t cli::countArguments(const char* commandLine) {
    uint32_t count{0};
    uint32_t commandLineLength{0};
    commandLineLength = strnlen(commandLine, cliCommand::maxCommandLineLength);
    for (uint32_t i{0}; i < commandLineLength; i++) {
        if (commandLine[i] == ' ') {
            count++;
        }
    }
    if (count > cliCommand::maxNmbrOfArguments) {
        count = cliCommand::maxNmbrOfArguments;
    }
    return count;
}

int32_t cli::getSeparatorPosition(const char* commandLine, const uint32_t separatorIndex) {
    uint32_t slashCount{0};
    uint32_t dataLength = strnlen(commandLine, cliCommand::maxCommandLineLength);
    for (uint32_t position = 0; position < dataLength; position++) {
        if (commandLine[position] == commandArgumentSeparator) {
            if (slashCount == separatorIndex) {
                return position;
            }
            slashCount++;
        }
    }
    return -1;
}

void cli::getSegment(char* destination, const char* commandLine, const uint32_t segmentIndex) {
    uint32_t startIndex;
    uint32_t endIndex;
    uint32_t nmbrOfArguments = countArguments(commandLine);

    if (segmentIndex > nmbrOfArguments) {
        destination[0] = '\0';
        return;
    }

    if (segmentIndex == 0) {
        startIndex = 0;
    } else {
        startIndex = getSeparatorPosition(commandLine, segmentIndex - 1) + 1;
    }

    if (segmentIndex == nmbrOfArguments) {
        endIndex = strnlen(commandLine, cliCommand::maxCommandLineLength);
    } else {
        endIndex = getSeparatorPosition(commandLine, segmentIndex);
    }

    for (uint32_t index = startIndex; index < endIndex; index++) {
        destination[index - startIndex] = commandLine[index];
    }
    destination[endIndex - startIndex] = '\0';
}

void cli::splitCommandLine(const char* commandLine) {
    nmbrOfArguments = countArguments(commandLine);
    getSegment(command, commandLine, 0);
    for (uint32_t argumentIndex = 0; argumentIndex < cliCommand::maxNmbrOfArguments; argumentIndex++) {
        if (argumentIndex < nmbrOfArguments) {
            getSegment(arguments[argumentIndex], commandLine, argumentIndex + 1);
        } else {
            arguments[argumentIndex][0] = '\0';
        }
    }
}

int32_t cli::findCommandIndex() {
    for (uint32_t commandIndex = 0; commandIndex < nmbrOfCommands; commandIndex++) {
        if (strcmp(command, commands[commandIndex].longName) == 0) {
            return commandIndex;
        }
        if (strcmp(command, commands[commandIndex].shortName) == 0) {
            return commandIndex;
        }
    }
    return -1;
}

void cli::executeCommand(const char* commandLine) {
    splitCommandLine(commandLine);
    int32_t commandIndex = findCommandIndex();
    if (commandIndex >= 0) {
        if (commands[commandIndex].handler != nullptr) {
            commands[commandIndex].handler();
        }
    } else {
        // respond with error message "unrecognized command 'xxxxx'"
    }
}
