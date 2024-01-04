#include "lorawanevent.h"

const char* toString(loRaWanEvent anEvent) {
    switch (anEvent) {
        case loRaWanEvent::none:
            return "none";
            break;
        case loRaWanEvent::sx126xCadEnd:
            return "sx126x Cad End";
            break;
        case loRaWanEvent::sx126xTxComplete:
            return "sx126x Tx Complete";
            break;
        case loRaWanEvent::sx126xRxComplete:
            return "sx126x Rx Complete";
            break;
        case loRaWanEvent::sx126xTimeout:
            return "sx126x Timeout";
            break;
        case loRaWanEvent::timeOut:
            return "timeOut";
            break;
        default:
            return "unknown event";
            break;
    }
}