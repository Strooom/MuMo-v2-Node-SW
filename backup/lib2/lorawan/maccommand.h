
// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include "linkdirection.h"

enum class macCommand : uint32_t {
    linkCheckRequest              = 0x02,
    linkCheckAnswer               = 0x02,
    linkAdaptiveDataRateRequest   = 0x03,
    linkAdaptiveDataRateAnswer    = 0x03,
    dutyCycleRequest              = 0x04,
    dutyCycleAnswer               = 0x04,
    receiveParameterSetupRequest  = 0x05,
    receiveParameterSetupAnswer   = 0x05,
    deviceStatusRequest           = 0x06,
    deviceStatusAnswer            = 0x06,
    newChannelRequest             = 0x07,
    newChannelAnswer              = 0x07,
    receiveTimingSetupRequest     = 0x08,
    receiveTimingSetupAnswer      = 0x08,
    transmitParameterSetupRequest = 0x09,
    transmitParameterSetupAnswer  = 0x09,
    downlinkChannelRequest        = 0x0A,
    downlinkChannelAnswer         = 0x0A,
    deviceTimeRequest             = 0x0D,
    deviceTimeAnswer              = 0x0D
};

const char* toString(macCommand aCommand, linkDirection aDirection);
