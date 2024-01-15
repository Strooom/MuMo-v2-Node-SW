// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>

enum class transmitPriority : uint8_t {
    doNotTransmit = 0b0000'0000,        // this will be stored in EEPROM, but not transmitted (over LoRaWAN) to the cloud
    low           = 0b0100'0000,        // transmitted only when network payload is full
    medium        = 0b1000'0000,        // this will be transmitted before the 'low', but only when the network payload is full
    high          = 0b1100'0000,        // this will cause a immediate network transmission, so the measurement goes witout any delay to the cloud
};
