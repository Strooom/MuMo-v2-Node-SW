// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>

enum class screenType : uint32_t {
    logo,
    version,
    uid,
    hwConfig,
    measurements,
    loraConfig,
    loraStatus,
    main
};