#include <applicationevent.hpp>

const char* toString(applicationEvent anEvent) {
    switch (anEvent) {
        case applicationEvent::none:
            return "none";

        case applicationEvent::usbConnected:
            return "usbConnected";

        case applicationEvent::usbRemoved:
            return "usbRemoved";

        case applicationEvent::realTimeClockTick:
            return "realTimeClockTick";

        case applicationEvent::lowPowerTimerExpired:
            return "lowPowerTimerExpired";

        case applicationEvent::applicationButtonPressed:
            return "applicationButtonPressed";

        case applicationEvent::downlinkApplicationPayloadReceived:
            return "downlinkApplicationPayloadReceived";

        case applicationEvent::downlinkMacCommandReceived:
            return "downlinkMACReceived";

        case applicationEvent::sx126xCadEnd:
            return "sx126xCadEnd";

        case applicationEvent::sx126xTxComplete:
            return "sx126xTxComplete";

        case applicationEvent::sx126xRxComplete:
            return "sx126xRxComplete";

        case applicationEvent::sx126xTimeout:
            return "sx126xTimeout";

        default:
            return "unknownApplicationEvent";
    }
}
