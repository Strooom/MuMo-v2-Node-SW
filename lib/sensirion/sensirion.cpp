#include <sensirion.hpp>

uint8_t sensirion::crc(const uint8_t byte0, const uint8_t byte1) {
    uint8_t result = crcInit;
    result ^= byte0;
    div(result);
    result ^= byte1;
    div(result);
    return result;
}

bool sensirion::checkCrc(const uint8_t* data, uint32_t count) {
    bool result{true};
    for (uint32_t index = 0; index < count; index += 3) {
        if (data[index + 2] != crc(data[index], data[index + 1])) {
            result = false;
            break;
        }
    }
    return result;
}

void sensirion::insertCrc(uint8_t* data, uint32_t count) {
    for (uint32_t index = 0; index < count; index += 3) {
        data[index + 2] = crc(data[index], data[index + 1]);
    }
}

void sensirion::div(uint8_t& aByte) {
    for (uint8_t bitIndex = 0; bitIndex < 8; bitIndex++) {
        if (aByte & 0x80)
            aByte = (aByte << 1) ^ crcPolynome;
        else
            aByte = (aByte << 1);
    }
}
