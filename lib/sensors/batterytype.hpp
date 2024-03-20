// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include "stdint.h"

enum class batteryType : uint8_t {
    liFePO4_700mAh   = 0x00,        // AA LiFePO4 battery
    liFePO4_1500mAh  = 0x01,        // 18650 LiFePO4 battery
    alkaline_1200mAh = 0x02,        // 2 AA Alkaline batteries
    saft_ls_142250   = 0x03,        // 1/2 AA Saft LS14250 battery
    saft_ls_145000   = 0x04,        // 1 AA Saft LS14500 battery
};

constexpr uint32_t nmbrBatteryTypes{5};
