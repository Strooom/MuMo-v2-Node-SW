// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ###                                                                                ###
// ### class to do polynome match, as needed for Reed-Solomon Error Correction        ###
// ######################################################################################

#pragma once
#include <gf256.hpp>
#include <stdint.h>
#include <cstring>

// ax^2 + bx + c
// order = 2
// number of terms = 3
// coefficients[2..0] = {a, b, c}

class polynome {
  public:
    explicit polynome(const uint32_t toBeOrder) {
        uint32_t nmbrOfTerms = toBeOrder + 1;
        if (nmbrOfTerms <= maxNmbrOfTerms) {
            order = toBeOrder;
        }
    }

    explicit polynome(const uint8_t *newCoefficients, uint32_t toBeOrder) {
        uint32_t nmbrOfTerms = toBeOrder + 1;
        if (nmbrOfTerms <= maxNmbrOfTerms) {
            order = toBeOrder;
            memcpy(coefficients, newCoefficients, nmbrOfTerms);
        }
    }

    explicit polynome() {        // zero order polynome with all zero coefficients
    }

    uint32_t getOrder() const {
        return order;
    }

    void remainder(const uint32_t nmbr) {        // divides the polynome by a generator polynome of order nmbr. Quotient is discarded, remainder is kept in the original polynome object
        uint32_t termsBefore    = order + 1;
        uint32_t termsGenerator = nmbr + 1;
        while (!isZero()) {
            auto divisor = polynome();
            divisor.generateGeneratorPolynome(nmbr);
            uint32_t factor = firstNonZeroTerm();
            divisor.multiplyScalar(factor);
            uint32_t shift = firstNonZeroTermIndex();
            for (uint32_t i = 0; i <= divisor.order; i++) {
                coefficients[i + shift] = gf256::subtract(coefficients[i + shift], divisor.coefficients[i]);
            }
        }
        memmove(coefficients, coefficients + termsBefore, termsGenerator);
        memset(coefficients + termsGenerator, 0, maxNmbrOfTerms - termsGenerator);
        order = nmbr - 1;
    }

    void getCoefficients(uint8_t *output) {
        memcpy(output, coefficients, order + 1);
    }

#ifndef unitTesting

  private:
#endif

    uint32_t firstNonZeroTermIndex() const {
        for (uint32_t index = 0; index <= order; index++) {
            if (coefficients[index] != 0) {
                return index;
            }
        }
        return 0;
    }

    bool isZero() const {
        for (uint32_t index = 0; index <= order; index++) {
            if (coefficients[index] != 0) {
                return false;
            }
        }
        return true;
    }

    uint32_t firstNonZeroTerm() const {
        return coefficients[firstNonZeroTermIndex()];
    }

    void multiply(const polynome &polynome1) {
        if (order + polynome1.order > maxNmbrOfTerms) {
            return;        // resulting polynome would be too large to fit in the storage
        }
        uint8_t tempStorage[polynome::maxNmbrOfTerms]{};
        memcpy(tempStorage, coefficients, polynome::maxNmbrOfTerms);
        memset(coefficients, 0, polynome::maxNmbrOfTerms);
        for (uint32_t i = 0; i <= polynome1.order; i++) {
            for (uint32_t j = 0; j <= order; j++) {
                coefficients[i + j] ^= gf256::mul1(polynome1.coefficients[i], tempStorage[j]);
            }
        }
        order += polynome1.order;
    }

    void multiply(const polynome &polynome1, const polynome &polynome2) {
        memset(coefficients, 0, order);
        for (uint32_t i = 0; i <= polynome1.order; i++) {
            for (uint32_t j = 0; j <= polynome2.order; j++) {
                coefficients[i + j] ^= gf256::mul1(polynome1.coefficients[i], polynome2.coefficients[j]);
            }
        }
    }

    void multiplyPower(const uint32_t xxx) {
        if (order + xxx > maxNmbrOfTerms) {
            return;
        }
        order = order + xxx;
    }

    void multiplyScalar(const uint32_t xxx) {
        for (uint32_t i = 0; i <= order; i++) {
            coefficients[i] = gf256::mul1(static_cast<uint8_t>(xxx), coefficients[i]);
        }
    }

    void generateGeneratorPolynome(uint32_t nmbr) {
        order = 0;
        memset(coefficients, 0, polynome::maxNmbrOfTerms);
        coefficients[0]    = 1;
        auto polynomeRight = polynome();
        for (uint32_t index = 0; index < nmbr; index++) {
            polynomeRight.setGeneratorFactor(index);
            multiply(polynomeRight);
        }
    };

    void setGeneratorFactor(uint32_t nmbr) {
        order           = 1;
        coefficients[0] = 1;
        coefficients[1] = gf256::gf256exp[nmbr];
    }

    static constexpr uint32_t maxNmbrOfTerms{64};
    uint8_t coefficients[maxNmbrOfTerms]{};        // initialize all coefficients to zero
    uint32_t order{0};
};
