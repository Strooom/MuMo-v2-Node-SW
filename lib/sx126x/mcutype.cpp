// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <mcutype.hpp>

const char *toString(mcuType theVersion) {
    switch (theVersion) {
        case mcuType::highPower:
            return "LoRa-E5-HF";

        case mcuType::lowPower:
            return "LoRa-E5-LE-HF";

        default:
            return "unknown";
    }
}