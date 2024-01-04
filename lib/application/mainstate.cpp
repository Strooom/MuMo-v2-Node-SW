// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <mainstate.hpp>

const char *toString(mainState aState) {
    switch (aState) {
        case mainState::boot:
            return "boot";
            break;
        case mainState::idle:
            return "idle";
            break;
        case mainState::measuring:
            return "measuring";
            break;
        case mainState::storingAndDisplaying:
            return "storing and displaying";
            break;
        case mainState::networking:
            return "networking";
            break;
        case mainState::sleeping:
            return "sleeping";
            break;
        default:
            return "unknown";
            break;
    }
}
