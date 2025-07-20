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
    sampling,
    networking,
    fatalError
};

static constexpr uint8_t nmbrOfMainStates = static_cast<uint8_t>(mainState::fatalError) + 1;

const char* toString(const mainState aState);