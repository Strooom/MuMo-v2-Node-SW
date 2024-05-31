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

float bytesToFloat(uint8_t bytes[4]) {
    static union {
        float asFloat;
        uint8_t asBytes[4];
    } convertor;

    for (uint32_t n = 0; n < 4; n++) {
        convertor.asBytes[n] = bytes[n];
    }
    return convertor.asFloat;
}

uint8_t* floatToBytes(float value) {
    static union {
        float asFloat;
        uint8_t asBytes[4];
    } convertor;

    convertor.asFloat = value;
    return convertor.asBytes;
}