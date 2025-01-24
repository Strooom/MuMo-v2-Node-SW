// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include "cstdint"

enum class batteryType : uint8_t {
    liFePO4_700mAh   = 0x00,        // AA LiFePO4 battery
    liFePO4_1500mAh  = 0x01,        // 18650 LiFePO4 battery
    alkaline_1200mAh = 0x02,        // 2 AA Alkaline batteries - 3V
    saft_ls_14250    = 0x03,        // 1/2 AA Saft LS14250 battery - 3.6V
    saft_ls_14500    = 0x04,        // 1 AA Saft LS14500 battery - 3.6V
    fdk_cr14250se    = 0x05,        // 1/2 AA FDK 14250 battery - 3V
    nmbrBatteryTypes = 0x06
};

constexpr uint32_t nmbrBatteryTypes{static_cast<uint32_t>(batteryType::nmbrBatteryTypes)};
const char* toString(batteryType type);
