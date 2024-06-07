#include <sensordevicecollection.hpp>
#include <logging.hpp>
#include <battery.hpp>
#include <bme680.hpp>
#include <sht40.hpp>
#include <tsl2591.hpp>
// All known sensordevices' include files are to be added here
#include <measurementcollection.hpp>
#include <float.hpp>

bool sensorDeviceCollection::isPresent[static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices)]{false};
uint32_t sensorDeviceCollection::actualNumberOfDevices{0};
sensorChannel sensorDeviceCollection::dummy = {0, "", ""};

void sensorDeviceCollection::discover() {
    isPresent[static_cast<uint32_t>(sensorDeviceType::battery)] = true;
    battery::initalize();
    actualNumberOfDevices++;

    if (bme680::isPresent()) {
        isPresent[static_cast<uint32_t>(sensorDeviceType::bme680)] = true;
        bme680::initialize();
        actualNumberOfDevices++;
    }
    if (sht40::isPresent()) {
        isPresent[static_cast<uint32_t>(sensorDeviceType::sht40)] = true;
        sht40::initialize();
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
                case sensorDeviceType::sht40:
                    sht40::tick();
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
                case sensorDeviceType::sht40:
                    sht40::run();
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
                case sensorDeviceType::sht40:
                    if (sht40::getState() != sensorDeviceState::sleeping) {
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

bool sensorDeviceCollection::hasNewMeasurements(uint32_t deviceIndex) {
    if (isPresent[deviceIndex]) {
        switch (static_cast<sensorDeviceType>(deviceIndex)) {
            case sensorDeviceType::battery:
                for (uint32_t channelIndex = 0U; channelIndex < battery::nmbrChannels; channelIndex++) {
                    if (battery::channels[channelIndex].hasNewValue) {
                        return true;
                    }
                }
                return false;
                break;
            case sensorDeviceType::bme680:
                for (uint32_t channelIndex = 0U; channelIndex < bme680::nmbrChannels; channelIndex++) {
                    if (bme680::channels[channelIndex].hasNewValue) {
                        return true;
                    }
                }
                return false;
                break;
            case sensorDeviceType::sht40:
                for (uint32_t channelIndex = 0U; channelIndex < sht40::nmbrChannels; channelIndex++) {
                    if (sht40::channels[channelIndex].hasNewValue) {
                        return true;
                    }
                }
                return false;
                break;
            case sensorDeviceType::tsl2591:
                for (uint32_t channelIndex = 0U; channelIndex < tsl2591::nmbrChannels; channelIndex++) {
                    if (tsl2591::channels[channelIndex].hasNewValue) {
                        return true;
                    }
                }
                return false;
                break;
            // Add more types of sensors here
            default:
                return false;
                break;
        }
    }
    return false;
}

bool sensorDeviceCollection::hasNewMeasurements() {
    for (auto deviceIndex = 0U; deviceIndex < static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices); deviceIndex++) {
        if (hasNewMeasurements(deviceIndex)) {
            return true;
        }
    }
    return false;
}

const char* sensorDeviceCollection::name(uint32_t index) {
    switch (static_cast<sensorDeviceType>(index)) {
        case sensorDeviceType::battery:
            return "battery";
        case sensorDeviceType::bme680:
            return "BME680";
        case sensorDeviceType::sht40:
            return "SHT40";
        case sensorDeviceType::tsl2591:
            return "TSL2591";
        // Add more types of sensors here
        default:
            return "invalid deviceIndex";
    }
}

float sensorDeviceCollection::channelValue(uint32_t deviceIndex, uint32_t channelIndex) {
    if (!isValidChannelIndex(deviceIndex, channelIndex)) {
        return channel(deviceIndex, channelIndex).value();
    }
    return 0.0F;
}

uint32_t sensorDeviceCollection::channelDecimals(uint32_t deviceIndex, uint32_t channelIndex) {
    if (isValidChannelIndex(deviceIndex, channelIndex)) {
        return channel(deviceIndex, channelIndex).decimals;
    }
    return 0;
}

const char* sensorDeviceCollection::channelName(uint32_t deviceIndex, uint32_t channelIndex) {
    if (isValidChannelIndex(deviceIndex, channelIndex)) {
        return channel(deviceIndex, channelIndex).name;
    }
    return "invalid index";
}

const char* sensorDeviceCollection::channelUnits(uint32_t deviceIndex, uint32_t channelIndex) {
    if (isValidChannelIndex(deviceIndex, channelIndex)) {
        return channel(deviceIndex, channelIndex).unit;
    }
    return "invalid index";
}

void sensorDeviceCollection::log(uint32_t deviceIndex, uint32_t channelIndex) {
    uint32_t maxNmbrChannels = nmbrOfChannels(deviceIndex);
    if (channelIndex < maxNmbrChannels) {
        float value       = channelValue(deviceIndex, channelIndex);
        uint32_t decimals = channelDecimals(deviceIndex, channelIndex);
        uint32_t intPart  = integerPart(value, decimals);
        if (decimals > 0) {
            uint32_t fracPart = fractionalPart(value, decimals);
            logging::snprintf(logging::source::sensorData, "%s = %d.%d %s\n", channelName(deviceIndex, channelIndex), intPart, fracPart, channelUnits(deviceIndex, channelIndex));
        } else {
            logging::snprintf(logging::source::sensorData, "%s = %d %s\n", channelName(deviceIndex, channelIndex), intPart, channelUnits(deviceIndex, channelIndex));
        }
    }
}

void sensorDeviceCollection::log(uint32_t deviceIndex) {
    if (isValidDeviceIndex(deviceIndex)) {
        for (uint32_t channelIndex = 0U; channelIndex < nmbrOfChannels(deviceIndex); channelIndex++) {
            log(deviceIndex, channelIndex);
        }
    }
}

void sensorDeviceCollection::log() {
    for (uint32_t deviceIndex = 0U; deviceIndex < static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices); deviceIndex++) {
        log(deviceIndex);
    }
}

uint32_t sensorDeviceCollection::nmbrOfNewMeasurements(uint32_t deviceIndex) {
    uint32_t result{0};
    if (isValidDeviceIndex(deviceIndex)) {
        switch (static_cast<sensorDeviceType>(deviceIndex)) {
            case sensorDeviceType::battery:
                for (uint32_t channelIndex = 0U; channelIndex < battery::nmbrChannels; channelIndex++) {
                    if (battery::channels[channelIndex].hasNewValue) {
                        result++;
                    }
                }
                break;
            case sensorDeviceType::bme680:
                for (uint32_t channelIndex = 0U; channelIndex < bme680::nmbrChannels; channelIndex++) {
                    if (bme680::channels[channelIndex].hasNewValue) {
                        result++;
                    }
                }
                break;
            case sensorDeviceType::sht40:
                for (uint32_t channelIndex = 0U; channelIndex < sht40::nmbrChannels; channelIndex++) {
                    if (sht40::channels[channelIndex].hasNewValue) {
                        result++;
                    }
                }
                break;
            case sensorDeviceType::tsl2591:
                for (uint32_t channelIndex = 0U; channelIndex < tsl2591::nmbrChannels; channelIndex++) {
                    if (tsl2591::channels[channelIndex].hasNewValue) {
                        result++;
                    }
                }
                break;
            // Add more types of sensors here
            default:
                break;
        }
    }
    return result;
}

uint32_t sensorDeviceCollection::nmbrOfNewMeasurements() {
    uint32_t result{0};
    for (uint32_t deviceIndex = 0U; deviceIndex < static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices); deviceIndex++) {
        result += nmbrOfNewMeasurements(deviceIndex);
    }
    return result;
}

void sensorDeviceCollection::collectNewMeasurements() {
    for (uint32_t deviceIndex = 0U; deviceIndex < static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices); deviceIndex++) {
        collectNewMeasurements(deviceIndex);
    }
}

void sensorDeviceCollection::collectNewMeasurements(uint32_t deviceIndex) {
    if (isValidDeviceIndex(deviceIndex)) {
        for (uint32_t channelIndex = 0U; channelIndex < nmbrOfChannels(deviceIndex); channelIndex++) {
            if (isValidChannelIndex(deviceIndex, channelIndex)) {
                if (channel(deviceIndex, channelIndex).hasNewValue) {
                    measurementCollection::addMeasurement(deviceIndex, channelIndex, channel(deviceIndex, channelIndex).value());
                };
            }
        }
    }
}

void sensorDeviceCollection::clearNewMeasurements() {
    for (uint32_t deviceIndex = 0U; deviceIndex < static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices); deviceIndex++) {
        clearNewMeasurements(deviceIndex);
    }
}

void sensorDeviceCollection::clearNewMeasurements(uint32_t deviceIndex) {
    if (isValidDeviceIndex(deviceIndex)) {
        switch (static_cast<sensorDeviceType>(deviceIndex)) {
            case sensorDeviceType::battery:
                for (uint32_t channelIndex = 0U; channelIndex < battery::nmbrChannels; channelIndex++) {
                    battery::channels[channelIndex].hasNewValue = false;
                }
                break;
            case sensorDeviceType::bme680:
                for (uint32_t channelIndex = 0U; channelIndex < bme680::nmbrChannels; channelIndex++) {
                    bme680::channels[channelIndex].hasNewValue = false;
                }
                break;
            case sensorDeviceType::sht40:
                for (uint32_t channelIndex = 0U; channelIndex < sht40::nmbrChannels; channelIndex++) {
                    sht40::channels[channelIndex].hasNewValue = false;
                }
                break;
            case sensorDeviceType::tsl2591:
                for (uint32_t channelIndex = 0U; channelIndex < tsl2591::nmbrChannels; channelIndex++) {
                    tsl2591::channels[channelIndex].hasNewValue = false;
                }
                break;
            // Add more types of sensors here
            default:
                break;
        }
    }
}

uint32_t sensorDeviceCollection::nmbrOfChannels(uint32_t deviceIndex) {
    if (isValidDeviceIndex(deviceIndex)) {
        switch (static_cast<sensorDeviceType>(deviceIndex)) {
            case sensorDeviceType::battery:
                return battery::nmbrChannels;
                break;
            case sensorDeviceType::bme680:
                return bme680::nmbrChannels;
                break;
            case sensorDeviceType::sht40:
                return sht40::nmbrChannels;
                break;
            case sensorDeviceType::tsl2591:
                return tsl2591::nmbrChannels;
                break;
            // Add more types of sensors here
            default:
                return 0;
        }
    }
    return 0;
}

sensorChannel& sensorDeviceCollection::channel(uint32_t deviceIndex, uint32_t channelIndex) {
    if (isValidChannelIndex(deviceIndex, channelIndex)) {
        switch (static_cast<sensorDeviceType>(deviceIndex)) {
            case sensorDeviceType::battery:
                return battery::channels[channelIndex];
                break;
            case sensorDeviceType::bme680:
                return bme680::channels[channelIndex];
                break;
            case sensorDeviceType::sht40:
                return sht40::channels[channelIndex];
                break;
            case sensorDeviceType::tsl2591:
                return tsl2591::channels[channelIndex];
                break;
            // Add more types of sensors here
            default:
                return dummy;        // in case of an invalid index, we return this dummy channel
        }
    }
    return dummy;        // in case of an invalid index, we return this dummy channel
}

bool sensorDeviceCollection::isValidDeviceIndex(uint32_t deviceIndex) {
    if (deviceIndex < static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices)) {
        return isPresent[deviceIndex];
    }
    return false;
}

bool sensorDeviceCollection::isValidChannelIndex(uint32_t deviceIndex, uint32_t channelIndex) {
    if (isValidDeviceIndex(deviceIndex)) {
        return channelIndex < nmbrOfChannels(deviceIndex);
    }
    return false;
}
