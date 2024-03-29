#include <cmath>
#include <inttypes.h>
#include <float.hpp>

int32_t integerPart(float value, uint32_t decimals) {
    return (static_cast<int>(round(value * factorFloat(decimals))) / factorInt(decimals));
}

uint32_t fractionalPart(float value, uint32_t decimals) {
    return static_cast<int>(round(value * factorFloat(decimals))) - (integerPart(value, decimals) * factorInt(decimals));
}

float factorFloat(uint32_t decimals) {
    float result{1.0F};
    for (uint32_t n = 0; n < decimals; n++) {
        result *= 10.0F;
    }
    return result;
}

uint32_t factorInt(uint32_t decimals) {
    uint32_t result{1U};
    for (uint32_t n = 0; n < decimals; n++) {
        result *= 10;
    }
    return result;
}
