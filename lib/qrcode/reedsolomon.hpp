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
    static void appendErrorCorrectionBytes(uint8_t *unprotectedMessage, const uint32_t unprotectedMessageLength, const uint32_t protectionLength);

  private:
    static void initialize(uint8_t degree, uint8_t *coeff);
    static uint8_t multiply(uint8_t x, uint8_t y);
    static void remainder(const uint8_t degree, const uint8_t *coeff, const uint8_t *data, const uint8_t length, uint8_t *result, const uint8_t stride);
};
