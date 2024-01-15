// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include "stdint.h"

// DefaultFPort defines the default fPort value for Clock Synnchronization.
constexpr uint8_t defaultFPort{202};
constexpr uint8_t packageIdentifier{};
constexpr uint8_t packageVersion{};
int32_t timeCorrection{}; // NOTE : signed int !
uint8_t token{}; // is some kind of sequence number to match requests and answers


enum class commandId : uint8_t {

    PackageVersionReq           = 0x00,
    PackageVersionAns           = 0x00,
    AppTimeReq                  = 0x01,
    AppTimeAns                  = 0x01,
    DeviceAppTimePeriodicityReq = 0x02,
    DeviceAppTimePeriodicityAns = 0x02,
    ForceDeviceResyncReq        = 0x03
};
