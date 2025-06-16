#include <deviceaddress.hpp>
#include <hexascii.hpp>

void deviceAddress::setFromByteArray(const uint8_t bytes[lengthInBytes]) {
    (void)memcpy(devAddrAsBytes, bytes, lengthInBytes);
    syncWordFromBytes();
    syncHexStringFromBytes();
}

void deviceAddress::setFromWord(const uint32_t wordIn) {
    devAddrAsWord = wordIn;
    syncBytesFromWord();
    syncHexStringFromBytes();
}

void deviceAddress::setFromHexString(const char* string) {
    hexAscii::hexStringToByteArray(devAddrAsBytes, string, lengthAsHexAscii);
    syncHexStringFromBytes();
    syncWordFromBytes();
}
