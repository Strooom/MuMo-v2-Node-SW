#include <cli.hpp>
#include <uart2.hpp>
#include <cstring>        // for strcmp
#include <clicommand.hpp>

bool cli::hasCommand() {
    return (uart2::commandCount() > 0);
}

void cli::getCommand(cliCommand& aCommand) {
    char commandLine[cliCommand::maxCommandLineLength];
    uart2::receive(commandLine);
    parseCommandLine(aCommand, commandLine);
}

void cli::sendResponse(const char* response) {
    uart2::transmit(response);
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

void cli::parseCommandLine(cliCommand& aCommand, const char* commandLine) {
    aCommand.nmbrOfArguments = countArguments(commandLine);
    getSegment(aCommand.commandAsString, commandLine, 0);
    aCommand.commandHash = hash(aCommand.commandAsString);
    for (uint32_t argumentIndex = 0; argumentIndex < cliCommand::maxNmbrOfArguments; argumentIndex++) {
        if (argumentIndex < aCommand.nmbrOfArguments) {
            getSegment(aCommand.arguments[argumentIndex], commandLine, argumentIndex + 1);
        } else {
            aCommand.arguments[argumentIndex][0] = '\0';
        }
    }
}

uint32_t cli::hash(const char* someCommand) {
    uint32_t length = strnlen(someCommand, cliCommand::maxCommandLength);
    if (length == 0) {
        return 0;
    }
    uint32_t result{0};
    for (uint32_t index{0}; index < cliCommand::maxCommandLength; index++) {
        if (index >= length) {
            break;
        }
        result = (result << 8) + static_cast<uint32_t>(someCommand[index]);
    }
    return result;
}

uint8_t cli::toLowerCase(const uint8_t characterToConvert) {
    if (characterToConvert >= 'A' && characterToConvert <= 'Z') {
        return characterToConvert + 'a' - 'A';
    } else {
        return characterToConvert;
    }
}

void cli::toLowerCase(char* commandLine) {
    uint32_t length = strnlen(commandLine, cliCommand::maxCommandLineLength);
    for (uint32_t index{0}; index < length; index++) {
        commandLine[index] = toLowerCase(static_cast<uint8_t>(commandLine[index]));
    }
}
