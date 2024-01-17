// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>

enum class transmitPower : uint8_t {
    max = 0x00,
    maxMinus2dB,
    maxMinus4dB,
    maxMinus6dB,
    maxMinus8dB,
    maxMinus10dB,
    maxMinus12dB,
    maxMinus14dB,
};

