// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <reedsolomon.hpp>
#include <polynome.hpp>
#include <cstring>

void reedSolomon::getErrorCorrectionBytes(uint8_t *output, const uint32_t protectionLength, const uint8_t *unprotectedMessage, uint32_t unprotectedMessageLength) {
    polynome message(unprotectedMessage, unprotectedMessageLength -1);
    message.remainder(protectionLength);
    message.getCoefficients(output);
}
