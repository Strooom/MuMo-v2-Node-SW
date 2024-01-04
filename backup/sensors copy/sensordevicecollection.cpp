#include <sensordevicecollection.hpp>

#include <battery.hpp>
#include <bme680.hpp>
#include <tsl2591.hpp>
// All known sensordevices' include files are to be added here

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

void sensorDeviceCollection::tick() {
    for (auto index = 0U; index < static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices); index++) {
        if (isPresent[index]) {
            switch (static_cast<sensorDeviceType>(index)) {
                case sensorDeviceType::battery:
                    battery::tick();
                    break;
                case sensorDeviceType::bme680:
                    bme680::tick();
                    break;
                case sensorDeviceType::tsl2591:
                    tsl2591::tick();
                    break;
                // Add more types of sensors here
                default:
                    break;
            }
        }
    }
}

void sensorDeviceCollection::run() {
    for (auto index = 0U; index < static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices); index++) {
        if (isPresent[index]) {
            switch (static_cast<sensorDeviceType>(index)) {
                case sensorDeviceType::battery:
                    // battery::run();
                    break;
                case sensorDeviceType::bme680:
                    // bme680::run();
                    break;
                case sensorDeviceType::tsl2591:
                    // tsl2591::run();
                    break;
                // Add more types of sensors here
                default:
                    break;
            }
        }
    }
}

bool sensorDeviceCollection::isSleeping() {
    for (auto index = 0U; index < static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices); index++) {
        if (isPresent[index]) {
            switch (static_cast<sensorDeviceType>(index)) {
                case sensorDeviceType::battery:
                    if (battery::getState() != sensorDeviceState::sleeping) {
                        return false;
                    }
                    break;
                case sensorDeviceType::bme680:
                    if (bme680::getState() != sensorDeviceState::sleeping) {
                        return false;
                    }
                    break;
                case sensorDeviceType::tsl2591:
                    if (tsl2591::getState() != sensorDeviceState::sleeping) {
                        return false;
                    }
                    break;
                // Add more types of sensors here
                default:
                    break;
            }
        }
    }
    return true;
}