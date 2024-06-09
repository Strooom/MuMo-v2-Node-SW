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
        if (isValidDeviceIndex(index)) {
            updateCounters(index);
            if (needsSampling(index)) {
                switch (static_cast<sensorDeviceType>(index)) {
                    case sensorDeviceType::battery:
                        battery::startSampling();
                        break;
                    case sensorDeviceType::bme680:
                        bme680::startSampling();
                        break;
                    case sensorDeviceType::sht40:
                        sht40::startSampling();
                        break;
                    case sensorDeviceType::tsl2591:
                        tsl2591::startSampling();
                        break;
                    // Add more types of sensors here
                    default:
                        break;
                }
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

bool sensorDeviceCollection::needsSampling(uint32_t deviceIndex, uint32_t channelIndex) {
    if (!isValidChannelIndex(deviceIndex, channelIndex)) {
        return channel(deviceIndex, channelIndex).needsSampling();
    }
    return false;
}

bool sensorDeviceCollection::needsSampling(uint32_t deviceIndex) {
    if (isValidDeviceIndex(deviceIndex)) {
        for (uint32_t channelIndex = 0U; channelIndex < nmbrOfChannels(deviceIndex); channelIndex++) {
            if (isValidChannelIndex(deviceIndex, channelIndex)) {
                if (channel(deviceIndex, channelIndex).needsSampling()) {
                    return true;
                };
            }
        }
    }
    return false;
}

void sensorDeviceCollection::updateCounters(uint32_t deviceIndex, uint32_t channelIndex) {
    if (isValidChannelIndex(deviceIndex, channelIndex)) {
        channel(deviceIndex, channelIndex).updateCounters();
    }
}

void sensorDeviceCollection::updateCounters(uint32_t deviceIndex) {
    if (isValidDeviceIndex(deviceIndex)) {
        for (uint32_t channelIndex = 0U; channelIndex < nmbrOfChannels(deviceIndex); channelIndex++) {
            if (isValidChannelIndex(deviceIndex, channelIndex)) {
                channel(deviceIndex, channelIndex).updateCounters();
            }
        }
    }
}

void sensorDeviceCollection::updateCounters() {
    for (auto deviceIndex = 0U; deviceIndex < static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices); deviceIndex++) {
        updateCounters(deviceIndex);
    }
}

bool sensorDeviceCollection::hasNewMeasurements(uint32_t deviceIndex) {
    if (isValidDeviceIndex(deviceIndex)) {
        for (uint32_t channelIndex = 0U; channelIndex < nmbrOfChannels(deviceIndex); channelIndex++) {
            if (isValidChannelIndex(deviceIndex, channelIndex)) {
                if (channel(deviceIndex, channelIndex).hasNewValue) {
                    return true;
                };
            }
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

float sensorDeviceCollection::value(uint32_t deviceIndex, uint32_t channelIndex) {
    if (!isValidChannelIndex(deviceIndex, channelIndex)) {
        return channel(deviceIndex, channelIndex).value();
    }
    return 0.0F;
}

uint32_t sensorDeviceCollection::decimals(uint32_t deviceIndex, uint32_t channelIndex) {
    if (isValidChannelIndex(deviceIndex, channelIndex)) {
        return channel(deviceIndex, channelIndex).decimals;
    }
    return 0;
}

const char* sensorDeviceCollection::name(uint32_t deviceIndex, uint32_t channelIndex) {
    if (isValidChannelIndex(deviceIndex, channelIndex)) {
        return channel(deviceIndex, channelIndex).name;
    }
    return "invalid index";
}

const char* sensorDeviceCollection::units(uint32_t deviceIndex, uint32_t channelIndex) {
    if (isValidChannelIndex(deviceIndex, channelIndex)) {
        return channel(deviceIndex, channelIndex).unit;
    }
    return "invalid index";
}

void sensorDeviceCollection::log(uint32_t deviceIndex, uint32_t channelIndex) {
    uint32_t maxNmbrChannels = nmbrOfChannels(deviceIndex);
    if (channelIndex < maxNmbrChannels) {
        float fvalue       = value(deviceIndex, channelIndex);
        uint32_t ddecimals = decimals(deviceIndex, channelIndex);
        uint32_t intPart   = integerPart(fvalue, ddecimals);
        if (ddecimals > 0) {
            uint32_t fracPart = fractionalPart(fvalue, ddecimals);
            logging::snprintf(logging::source::sensorData, "%s = %d.%d %s\n", name(deviceIndex, channelIndex), intPart, fracPart, units(deviceIndex, channelIndex));
        } else {
            logging::snprintf(logging::source::sensorData, "%s = %d %s\n", name(deviceIndex, channelIndex), intPart, units(deviceIndex, channelIndex));
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

uint32_t sensorDeviceCollection::nmbrOfNewMeasurements() {
    uint32_t result{0};
    for (uint32_t deviceIndex = 0U; deviceIndex < static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices); deviceIndex++) {
        result += nmbrOfNewMeasurements(deviceIndex);
    }
    return result;
}

uint32_t sensorDeviceCollection::nmbrOfNewMeasurements(uint32_t deviceIndex) {
    uint32_t result{0};
    if (isValidDeviceIndex(deviceIndex)) {
        for (uint32_t channelIndex = 0U; channelIndex < nmbrOfChannels(deviceIndex); channelIndex++) {
            if (isValidChannelIndex(deviceIndex, channelIndex)) {
                if (channel(deviceIndex, channelIndex).hasNewValue) {
                    result++;
                };
            }
        }
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
        for (uint32_t channelIndex = 0U; channelIndex < nmbrOfChannels(deviceIndex); channelIndex++) {
            if (isValidChannelIndex(deviceIndex, channelIndex)) {
                channel(deviceIndex, channelIndex).hasNewValue = false;
            };
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
