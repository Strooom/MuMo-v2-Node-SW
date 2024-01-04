#include "sensordevicetype.h"

const char* toString(sensorDeviceType theType) {
    switch (theType) {
        case sensorDeviceType::battery:
            return "battery";
            break;

        case sensorDeviceType::mcu:
            return "mcu";
            break;

        case sensorDeviceType::bme680:
            return "bme680";
            break;

        case sensorDeviceType::bme688:
            return "bme688";
            break;

        case sensorDeviceType::tsl2591:
            return "tsl2591";
            break;

        case sensorDeviceType::sht40:
            return "sht40";
            break;

        case sensorDeviceType::veml7700:
            return "veml7700";
            break;

        case sensorDeviceType::lis3dh:
            return "lis3dh";
            break;

        default:
            return "unknown";
            break;
    }
}