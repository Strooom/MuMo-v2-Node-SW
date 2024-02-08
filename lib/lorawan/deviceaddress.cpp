#include <deviceaddress.hpp>

deviceAddress::deviceAddress() : asUint32(0) {}

deviceAddress::deviceAddress(uint32_t theDeviceAddress) : asUint32(theDeviceAddress) {}

deviceAddress& deviceAddress::operator=(const uint32_t theDeviceAddress) {
    asUint32 = theDeviceAddress;
    return *this;
}

deviceAddress& deviceAddress::operator=(const deviceAddress &theDeviceAddress) {
    asUint32 = theDeviceAddress.asUint32;
    return *this;
}
bool deviceAddress::operator==(const deviceAddress &theDeviceAddress) {
    return (asUint32 == theDeviceAddress.asUint32);
}

bool deviceAddress::operator!=(const deviceAddress &theDeviceAddress) {
    return (asUint32 != theDeviceAddress.asUint32);
}