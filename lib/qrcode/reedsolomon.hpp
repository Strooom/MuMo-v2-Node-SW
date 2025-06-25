// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once

#include <stdint.h>
#include <polynome.hpp>

class reedSolomon {
  public:
    static void getErrorCorrectionBytes(uint8_t *output, const uint32_t protectionLength, const uint8_t *unprotectedMessage, uint32_t unprotectedMessageLength);

};
