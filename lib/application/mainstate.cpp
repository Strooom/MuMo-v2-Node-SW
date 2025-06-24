// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <mainstate.hpp>

const char *toString(mainState aState) {
    switch (aState) {
        case mainState::boot:
            return "boot";
            break;
        case mainState::networkCheck:
            return "networkCheck";
            break;
        case mainState::idle:
            return "idle";
            break;
        case mainState::sampling:
            return "sampling";
            break;
        case mainState::networking:
            return "networking";
            break;
        case mainState::fatalError:
            return "fatalError";
            break;
        case mainState::test:
            return "test";
            break;
        default:
            return "unknown";
            break;
    }
}
