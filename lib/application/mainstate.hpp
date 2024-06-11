// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>

enum class mainState : uint32_t {
    boot,
    waitForBootScreen,
    waitForNetworkRequest,
    waitForNetworkResponse,
    idle,
    measuring,
    logging,
    displaying, 
    networking,
    sleeping,
    networkError,
    test
};

const char* toString(const mainState aState);