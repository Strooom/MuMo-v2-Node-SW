// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################
// based on the work of Richard Moore : https://github.com/ricmoo/QRCode
// based on the work of Nayuki : https://www.nayuki.io/page/qr-code-generator-library

#pragma once

#include <stdint.h>


enum class errorCorrectionLevel {
    low      = 0,
    medium   = 1,
    quartile = 2,
    high     = 3,
    nmbrOfErrorCorrectionLevels = 4
};



class reedSolomon {
  public:

    static void initialize(uint8_t degree, uint8_t *coeff);
    static uint8_t multiply(uint8_t x, uint8_t y);
    static void remainder(uint8_t degree, uint8_t *coeff, uint8_t *data, uint8_t length, uint8_t *result, uint8_t stride);
};
