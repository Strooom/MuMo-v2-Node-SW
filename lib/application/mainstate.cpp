// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <mainstate.hpp>

const char *toString(mainState aState) {
    switch (aState) {
        case mainState::waitForNetworkRequest:
            return "waitForNetworkRequest";
            break;
        case mainState::waitForNetworkResponse:
            return "waitForNetworkResponse";
            break;
        case mainState::idle:
            return "idle";
            break;
        case mainState::measuring:
            return "measuring";
            break;
        case mainState::logging:
            return "logging";
            break;
        case mainState::displaying:
            return "displaying";
            break;
        case mainState::networking:
            return "networking";
            break;
        case mainState::networkError:
            return "networkError";
            break;
        case mainState::test:
            return "test";
            break;
        default:
            return "unknown";
            break;
    }
}
