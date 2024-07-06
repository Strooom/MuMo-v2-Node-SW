#include <framecontrol.hpp>

frameControl::frameControl() {
    theLinkDirection = linkDirection::uplink;
    ADR              = false;
    ADRACKReq        = false;
    ACK              = false;
    ClassB           = false;
    FPending         = false;
    FOptsLen         = 0;
}

frameControl::frameControl(linkDirection newLinkDirection) {
    theLinkDirection = newLinkDirection;
    if (theLinkDirection == linkDirection::uplink) {
        ADR       = false;
        ADRACKReq = false;
        ACK       = false;
        ClassB    = false;
        FPending  = false;
        FOptsLen  = 0;
    } else {
        //     when we transmit an uplink, we need to set these bits, when we receive a downlink, we need to read them from the received data
    }
}

uint8_t frameControl::asByte() const {
    uint8_t result{0};
    result = result | (static_cast<uint8_t>(ADR) << 7U);
    result = result | (static_cast<uint8_t>(ADRACKReq) << 6U);
    result = result | (static_cast<uint8_t>(ACK) << 5U);
    result = result | (static_cast<uint8_t>(ClassB) << 4U);
    result = result | (FOptsLen & 0x0F);
    return result;
}

void frameControl::set(uint8_t theByte) {
    theLinkDirection = linkDirection::downlink;
    ADR              = ((theByte & 0x80) == 0x80);
    ADRACKReq        = ((theByte & 0x40) == 0x40);
    ACK              = ((theByte & 0x20) == 0x20);
    FPending         = ((theByte & 0x10) == 0x10);
    FOptsLen         = (theByte & 0x0F);
}
