#include "sensordevicetype.hpp"

const char* toString(sensorDeviceType theType) {
    switch (theType) {
        case sensorDeviceType::battery:
            return "battery";
            break;

        case sensorDeviceType::mcu:
            return "STM32WLE5";
            break;

        case sensorDeviceType::bme680:
            return "BME680";
            break;

        case sensorDeviceType::tsl2591:
            return "TSL2591";
            break;

        case sensorDeviceType::sht40:
            return "SHT40";
            break;

        case sensorDeviceType::lis3dh:
            return "LIS3DH";
            break;

        default:
            return "unknown";
            break;
    }
}