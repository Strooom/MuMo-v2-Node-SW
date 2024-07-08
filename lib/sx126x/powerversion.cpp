// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <powerversion.hpp>

const char *toString(powerVersion theVersion) {
    switch (theVersion) {
        case powerVersion::highPower:
            return "LoRa-E5-HF";

        case powerVersion::lowPower:
            return "LoRa-E5-LE-HF";

        default:
            return "unknown";
    }
}