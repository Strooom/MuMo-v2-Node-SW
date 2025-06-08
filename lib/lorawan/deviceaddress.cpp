#include <deviceaddress.hpp>
#include <hexascii.hpp>

// deviceAddress& deviceAddress::operator=(const uint32_t theDeviceAddress) {
//     asUint32 = theDeviceAddress;
//     return *this;
// }

// deviceAddress& deviceAddress::operator=(const deviceAddress& theDeviceAddress) {
//     asUint32 = theDeviceAddress.asUint32;
//     return *this;
// }
// bool deviceAddress::operator==(const deviceAddress& theDeviceAddress) const {
//     return (asUint32 == theDeviceAddress.asUint32);
// }

// bool deviceAddress::operator!=(const deviceAddress& theDeviceAddress) const {
//     return (asUint32 != theDeviceAddress.asUint32);
// }

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
