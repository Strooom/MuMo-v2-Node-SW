#include <deviceaddress.hpp>
#include <hexascii.hpp>

deviceAddress::deviceAddress() : asUint32(0) {}

deviceAddress::deviceAddress(uint32_t theDeviceAddress) : asUint32(theDeviceAddress) {}

deviceAddress& deviceAddress::operator=(const uint32_t theDeviceAddress) {
    asUint32 = theDeviceAddress;
    return *this;
}

deviceAddress& deviceAddress::operator=(const deviceAddress& theDeviceAddress) {
    asUint32 = theDeviceAddress.asUint32;
    return *this;
}
bool deviceAddress::operator==(const deviceAddress& theDeviceAddress) const {
    return (asUint32 == theDeviceAddress.asUint32);
}

bool deviceAddress::operator!=(const deviceAddress& theDeviceAddress) const {
    return (asUint32 != theDeviceAddress.asUint32);
}

const char* deviceAddress::asHexString() {
    hexAscii::uint32ToHexString(asHexString_, asUint32);
    return asHexString_;
}


// uint8_t deviceAddress::asUint8(uint32_t index) {
//     return 0; // TODO need to return the correct byte from the 32bit address
// }