// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <reedsolomon.hpp>
#include <polynome.hpp>
#include <cstring>

void reedSolomon::getErrorCorrectionBytes(uint8_t *output, const uint32_t protectionLength, const uint8_t *unprotectedMessage, uint32_t unprotectedMessageLength) {
    // polynome message(unprotectedMessage, unprotectedMessageLength);        // create polynome from message
    // polynome generator{protectionLength};                                  // create generator polynome = divisor, from correct length
    // polynome::remainder(generator.order, generator.coefficients, message.coefficients, message.order, message.coefficients, 1);
    // polynome::getCoefficients(output, protectionLength, message.coefficients, message.order);
}

void reedSolomon::appendErrorCorrectionBytes(uint8_t *unprotectedMessage, const uint32_t unprotectedMessageLength, const uint32_t protectionLength) {
    // TODO: implement
}

uint8_t reedSolomon::multiply(uint8_t x, uint8_t y) {
    // Russian peasant multiplication
    // See: https://en.wikipedia.org/wiki/Ancient_Egyptian_multiplication
    uint16_t z = 0;
    for (int8_t i = 7; i >= 0; i--) {
        z = (z << 1) ^ ((z >> 7) * 0x11D);
        z ^= ((y >> i) & 1) * x;
    }
    return z;
}

void reedSolomon::initialize(uint8_t degree, uint8_t *coeff) {
    memset(coeff, 0, degree);
    coeff[degree - 1] = 1;

    // Compute the product polynomial (x - r^0) * (x - r^1) * (x - r^2) * ... * (x - r^{degree-1}),
    // drop the highest term, and store the rest of the coefficients in order of descending powers.
    // Note that r = 0x02, which is a generator element of this field GF(2^8/0x11D).
    uint16_t root = 1;
    for (uint8_t i = 0; i < degree; i++) {
        // Multiply the current product by (x - r^i)
        for (uint8_t j = 0; j < degree; j++) {
            coeff[j] = multiply(coeff[j], root);
            if (j + 1 < degree) {
                coeff[j] ^= coeff[j + 1];
            }
        }
        root = (root << 1) ^ ((root >> 7) * 0x11D);        // Multiply by 0x02 mod GF(2^8/0x11D)
    }
}

void reedSolomon::remainder(const uint8_t degree, const uint8_t *coeff, const uint8_t *data, const uint8_t length, uint8_t *result, const uint8_t stride) {
    // Compute the remainder by performing polynomial division
    for (uint8_t i = 0; i < length; i++) {
        uint8_t factor = data[i] ^ result[0];
        for (uint8_t j = 1; j < degree; j++) {
            result[(j - 1) * stride] = result[j * stride];
        }
        result[(degree - 1) * stride] = 0;

        for (uint8_t j = 0; j < degree; j++) {
            result[j * stride] ^= multiply(coeff[j], factor);
        }
    }
}
