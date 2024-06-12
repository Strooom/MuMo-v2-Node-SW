#include <applicationevent.hpp>

const char* toString(applicationEvent anEvent) {
    switch (anEvent) {
        case applicationEvent::none:
            return "none";
            break;

        case applicationEvent::usbConnected:
            return "usbConnected";
            break;

        case applicationEvent::usbRemoved:
            return "usbRemoved";
            break;

        case applicationEvent::realTimeClockTick:
            return "realTimeClockTick";
            break;

        case applicationEvent::lowPowerTimerExpired:
            return "lowPowerTimerExpired";
            break;

        case applicationEvent::downlinkApplicationPayloadReceived:
            return "downlinkApplicationPayloadReceived";
            break;

        case applicationEvent::downlinkMacCommandReceived:
            return "downlinkMACReceived";
            break;

        case applicationEvent::sx126xCadEnd:
            return "sx126xCadEnd";
            break;

        case applicationEvent::sx126xTxComplete:
            return "sx126xTxComplete";
            break;

        case applicationEvent::sx126xRxComplete:
            return "sx126xRxComplete";
            break;

        case applicationEvent::sx126xTimeout:
            return "sx126xTimeout";
            break;

        default:
            return "unknownApplicationEvent";
            break;
    }
}
