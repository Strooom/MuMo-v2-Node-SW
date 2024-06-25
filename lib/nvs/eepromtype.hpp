// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>

// Different EEPROM types have different page size and memory size, etc.

enum class eepromType : uint8_t {
    unknown  = 0x00,
    BR24G512 = 0x01,
    M24M01E  = 0x02
};