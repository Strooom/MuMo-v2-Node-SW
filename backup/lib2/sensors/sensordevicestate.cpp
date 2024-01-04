#include "sensordevicestate.h"

const char *toString(sensorDeviceState aState) {
    switch (aState) {
        case sensorDeviceState::unknown:
            return "unknown";
        case sensorDeviceState::unpowered:
            return "unpowered";
        case sensorDeviceState::sleeping:
            return "sleeping";
        case sensorDeviceState::standby:
            return "standby";
        case sensorDeviceState::sampling:
            return "sampling";
        default:
            return "unknown";
    }
}