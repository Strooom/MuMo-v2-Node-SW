// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "messagetype.h"

const char* toString(messageType type) {
    switch (type) {
        case messageType::application:
            return "application";
            break;

        case messageType::lorawanMac:
            return "lorawanMac";
            break;

        default:
            return "invalid / unknown";
            break;
    }
}
