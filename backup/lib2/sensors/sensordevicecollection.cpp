#include "sensordevicecollection.h"
// All known sensor devices are to be included here
#include "bme680.h"
#include "tsl2591.hpp"

bool sensorDeviceCollection::isPresent[static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices)]{false};
uint32_t sensorDeviceCollection::actualNumberOfDevices{0};

void sensorDeviceCollection::discover() {
    isPresent[static_cast<uint32_t>(sensorDeviceType::battery)] = true;
    actualNumberOfDevices++;

    if (bme680::isPresent()) {
        isPresent[static_cast<uint32_t>(sensorDeviceType::bme680)] = true;
        actualNumberOfDevices++;
    }
    if (tsl2591::isPresent()) {
        isPresent[static_cast<uint32_t>(sensorDeviceType::tsl2591)] = true;
        actualNumberOfDevices++;
    }
    // Add more types of sensors here
}

void sensorDeviceCollection::run() {
    for (auto index = 0U; index < static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices); index++) {
        if (isPresent[index]) {
            switch (static_cast<sensorDeviceType>(index)) {
                case sensorDeviceType::battery:
                    //battery::run();
                    break;
                case sensorDeviceType::bme680:
                    //bme680::run();
                    break;
                case sensorDeviceType::tsl2591:
                    //tsl2591::run();
                    break;
                // Add more types of sensors here
                default:
                    break;
            }
        }
    }
}