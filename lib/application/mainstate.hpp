// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>

enum class mainState : uint32_t {
    boot,
    networkCheck,
    idle,
    measuring,
    logging,
    networking,
    networkError,
    test
};

const char* toString(const mainState aState);