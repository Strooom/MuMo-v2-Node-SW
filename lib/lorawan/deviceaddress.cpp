#include <deviceaddress.hpp>

void deviceAddress::setFromByteArray(const uint8_t bytes[lengthInBytes]) {
    (void)memcpy(devAddrAsBytes, bytes, lengthInBytes);
    // syncWordFromBytes();
    syncHexStringFromBytes();
}

void deviceAddress::setFromWord(const uint32_t wordIn) {
    (void)memcpy(devAddrAsBytes, &wordIn, lengthInBytes);
    syncHexStringFromBytes();
}

void deviceAddress::setFromHexString(const char* string) {
    hexAscii::hexStringToByteArrayReversed(devAddrAsBytes, string, lengthAsHexAscii);
    syncHexStringFromBytes();
    // syncWordFromBytes();
}

uint32_t deviceAddress::getAsWord() const {
    uint32_t result;
    (void)memcpy(&result, devAddrAsBytes, lengthInBytes);
    return result;
}