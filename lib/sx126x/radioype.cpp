// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <radiotype.hpp>

const char *toString(radioType theVersion) {
    switch (theVersion) {
        case radioType::highPower:
            return "LoRa-E5-HF";

        case radioType::lowPower:
            return "LoRa-E5-LE-HF";

        default:
            return "unknown radio type";
    }
}