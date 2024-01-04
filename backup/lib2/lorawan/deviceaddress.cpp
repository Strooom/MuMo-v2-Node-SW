#include "deviceaddress.h"

deviceAddress::deviceAddress() {}

deviceAddress::deviceAddress(uint32_t theDeviceAddress) : asUint32(theDeviceAddress) {}

deviceAddress::deviceAddress(uint8_t theDeviceAddress[4]) {
    asUint8[0] = theDeviceAddress[0];
    asUint8[1] = theDeviceAddress[1];
    asUint8[2] = theDeviceAddress[2];
    asUint8[3] = theDeviceAddress[3];
}

void deviceAddress::set(uint32_t theDeviceAddress) {
    asUint32 = theDeviceAddress;
}
void deviceAddress::set(uint8_t theDeviceAddress[4]) {
    asUint8[0] = theDeviceAddress[0];
    asUint8[1] = theDeviceAddress[1];
    asUint8[2] = theDeviceAddress[2];
    asUint8[3] = theDeviceAddress[3];
}

deviceAddress& deviceAddress::operator=(uint32_t theDeviceAddress) {
    asUint32 = theDeviceAddress;
    return *this;
}
