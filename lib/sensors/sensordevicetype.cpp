// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <sensordevicetype.hpp>

const char *toString(sensorDeviceType theDevice){
    switch (theDevice) {
        case sensorDeviceType::mcu:
            return "STM32WLE5";
        case sensorDeviceType::battery:
            return "Battery";
        case sensorDeviceType::bme680:
            return "BME680";
        case sensorDeviceType::tsl2591:
            return "TSL2591";
        case sensorDeviceType::sht40:
            return "SHT40";
        case sensorDeviceType::lis3dh:
            return "LIS3DH";
        case sensorDeviceType::sths34:
            return "STHS34";
        case sensorDeviceType::scd40:
            return "SCD40";
        case sensorDeviceType::sps30:
            return "SPS30";
        case sensorDeviceType::xa1110:
            return "XA1110";
        default:
            return "unknown";
    }
}

