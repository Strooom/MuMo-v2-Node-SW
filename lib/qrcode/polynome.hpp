// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <gf256.hpp>
#include <stdint.h>
#include <cstring>

class polynome {
  public:
    explicit polynome() {
        memset(coefficients, 0, maxNmbrOfTerms);
    }
    explicit polynome(uint32_t newOrder) {
        if (newOrder < (maxNmbrOfTerms - 1)) {
            order = newOrder;
        }
        memset(coefficients, 0, maxNmbrOfTerms);
    }
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
    void generatePolynome(uint32_t nmbr) {
        order = 0;
        memset(coefficients, 0, polynome::maxNmbrOfTerms);
        coefficients[0] = 1;
        auto polynomeRight = polynome();
        for (uint32_t index = 0; index < nmbr; index++) {
            polynomeRight.setGeneratorPolynome(index);
            multiply(polynomeRight);
        }
    };
    void setGeneratorPolynome(uint32_t nmbr) {
        order           = 1;
        coefficients[0] = 1;
        coefficients[1] = gf256::gf256exp[nmbr];
    }
    void remainder(uint32_t nmbr) {
        while (!isZero()) {
            auto divisor = polynome();
            divisor.generatePolynome(nmbr);
            uint32_t factor = firstNonZeroTerm();
            divisor.multiplyScalar(factor);
            uint32_t shift = firstNonZeroTermIndex();
            for (uint32_t i = 0; i <= divisor.order; i++) {
                coefficients[i + shift] = gf256::subtract(coefficients[i + shift], divisor.coefficients[i]);
            }
        }
    }

#ifndef unitTesting

  private:
#endif
    static constexpr uint32_t maxNmbrOfTerms{64};
    uint8_t coefficients[maxNmbrOfTerms];
    uint32_t order{0};
};
