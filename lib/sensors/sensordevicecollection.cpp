#include <sensordevicecollection.hpp>
#include <logging.hpp>
#include <battery.hpp>
#include <bme680.hpp>
#include <tsl2591.hpp>
// All known sensordevices' include files are to be added here

bool sensorDeviceCollection::isPresent[static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices)]{false};
uint32_t sensorDeviceCollection::actualNumberOfDevices{0};

void sensorDeviceCollection::discover() {
    isPresent[static_cast<uint32_t>(sensorDeviceType::battery)] = true;
    battery::initalize();
    actualNumberOfDevices++;

    if (bme680::isPresent()) {
        isPresent[static_cast<uint32_t>(sensorDeviceType::bme680)] = true;
        bme680::initialize();
        actualNumberOfDevices++;
    }
    if (tsl2591::isPresent()) {
        isPresent[static_cast<uint32_t>(sensorDeviceType::tsl2591)] = true;
        tsl2591::initialize();
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
                    battery::run();
                    break;
                case sensorDeviceType::bme680:
                    bme680::run();
                    break;
                case sensorDeviceType::tsl2591:
                    tsl2591::run();
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

bool sensorDeviceCollection::hasNewMeasurements() {
    for (auto index = 0U; index < static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices); index++) {
        if (isPresent[index]) {
            switch (static_cast<sensorDeviceType>(index)) {
                case sensorDeviceType::battery:
                    if (battery::hasNewMeasurement()) {
                        return true;
                    }
                    break;
                case sensorDeviceType::bme680:
                    if (bme680::hasNewMeasurement()) {
                        return true;
                    }
                    break;
                case sensorDeviceType::tsl2591:
                    if (tsl2591::hasNewMeasurement()) {
                        return true;
                    }
                    break;
                // Add more types of sensors here
                default:
                    break;
            }
        }
    }
    return false;
}

const char* sensorDeviceCollection::name(uint32_t index) {
    switch (static_cast<sensorDeviceType>(index)) {
        case sensorDeviceType::battery:
            return "battery";
        case sensorDeviceType::bme680:
            return "bme680";
        case sensorDeviceType::tsl2591:
            return "tsl2591";
        // Add more types of sensors here
        default:
            return "unknown";
    }
}

float sensorDeviceCollection::valueAsFloat(uint32_t deviceIndex, uint32_t channelIndex) {
    switch (static_cast<sensorDeviceType>(deviceIndex)) {
        case sensorDeviceType::battery:
            return battery::valueAsFloat(channelIndex);
            break;

        case sensorDeviceType::bme680:
            return bme680::valueAsFloat(channelIndex);
            break;

        case sensorDeviceType::tsl2591:
            return tsl2591::valueAsFloat(channelIndex);
            break;

        // Add more types of sensors here
        default:
            return 0.0F;
    }
}
uint32_t sensorDeviceCollection::channelDecimals(uint32_t deviceIndex, uint32_t channelIndex) {
    switch (static_cast<sensorDeviceType>(deviceIndex)) {
        case sensorDeviceType::battery:
            return battery::channelFormats[channelIndex].decimals;
            break;

        case sensorDeviceType::bme680:
            return bme680::channelFormats[channelIndex].decimals;
            break;

        case sensorDeviceType::tsl2591:
            return tsl2591::channelFormats[channelIndex].decimals;
            break;

        // Add more types of sensors here
        default:
            return 0;
    }
}

const char* sensorDeviceCollection::channelName(uint32_t deviceIndex, uint32_t channelIndex) {
    switch (static_cast<sensorDeviceType>(deviceIndex)) {
        case sensorDeviceType::battery:
            return battery::channelFormats[channelIndex].name;
            break;

        case sensorDeviceType::bme680:
            return bme680::channelFormats[channelIndex].name;
            break;

        case sensorDeviceType::tsl2591:
            return tsl2591::channelFormats[channelIndex].name;
            break;

        // Add more types of sensors here
        default:
            return "";
    }
}

const char* sensorDeviceCollection::channelUnits(uint32_t deviceIndex, uint32_t channelIndex) {
    switch (static_cast<sensorDeviceType>(deviceIndex)) {
        case sensorDeviceType::battery:
            return battery::channelFormats[channelIndex].unit;
            break;

        case sensorDeviceType::bme680:
            return bme680::channelFormats[channelIndex].unit;
            break;

        case sensorDeviceType::tsl2591:
            return tsl2591::channelFormats[channelIndex].unit;
            break;

        // Add more types of sensors here
        default:
            return "";
    }
}

void sensorDeviceCollection::log() {
    for (uint32_t index = 0U; index < static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices); index++) {
        if (isPresent[index]) {
            switch (static_cast<sensorDeviceType>(index)) {
                case sensorDeviceType::battery:
                    if (battery::hasNewMeasurement()) {
                        battery::log();
                    }
                    break;
                case sensorDeviceType::bme680:
                    if (bme680::hasNewMeasurement()) {
                        bme680::log();
                    }
                    break;
                case sensorDeviceType::tsl2591:
                    if (tsl2591::hasNewMeasurement()) {
                        tsl2591::log();
                    }
                    break;
                // Add more types of sensors here
                default:
                    break;
            }
        }
    }
}