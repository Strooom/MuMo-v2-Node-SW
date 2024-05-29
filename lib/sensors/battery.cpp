// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <sensordevicetype.hpp>
#include <battery.hpp>
#include <settingscollection.hpp>
#include <chargefromvoltage.hpp>
#include <logging.hpp>
#include <float.hpp>
#include <measurementcollection.hpp>

#ifndef generic
#include <main.h>
extern ADC_HandleTypeDef hadc;
#endif

// ### initialize static members ###
batteryType battery::type{batteryType::liFePO4_700mAh};
sensorDeviceState battery::state{sensorDeviceState::unknown};
sensorChannel battery::channels[nmbrChannels];
sensorChannelFormat battery::channelFormats[nmbrChannels] = {
    {"voltage", "V", 2},
    {"percentCharged", "%", 0},
};

void battery::initalize() {
    uint8_t typeIndex = settingsCollection::read<uint8_t>(settingsCollection::settingIndex::batteryType);
    if (typeIndex >= nmbrBatteryTypes) {
        typeIndex = 0;
    }
    type  = static_cast<batteryType>(typeIndex);
    state = sensorDeviceState::sleeping;

    logging::snprintf(logging::source::settings, "batteryType : %s\n", toString(type));
}

uint32_t battery::nmbrOfNewMeasurements() {
    uint32_t count{0};
    for (uint32_t channelIndex = 0; channelIndex < nmbrChannels; channelIndex++) {
        if (channels[channelIndex].hasNewValue) {
            count++;
        }
    }
    return count;
}

bool battery::hasNewMeasurement(uint32_t channelIndex) {
    return channels[channelIndex].hasNewValue;
}

void battery::clearNewMeasurement(uint32_t channelIndex) {
    channels[channelIndex].hasNewValue = false;
}

void battery::clearAllNewMeasurements() {
    for (uint32_t channelIndex = 0; channelIndex < nmbrChannels; channelIndex++) {
        channels[channelIndex].hasNewValue = false;
    }
}

uint32_t battery::nextNewMeasurementChannel(uint32_t startIndex) {
    for (uint32_t channelIndex = startIndex; channelIndex < nmbrChannels; channelIndex++) {
        if (channels[channelIndex].hasNewValue) {
            return channelIndex;
        }
    }
    return notFound;
}

float battery::valueAsFloat(uint32_t index) {
    return channels[index].getOutput();
}

void battery::tick() {
    if (anyChannelNeedsSampling()) {
        clearAllNewMeasurements();
        startSampling();
        state = sensorDeviceState::sampling;
    } else {
        adjustAllCounters();
    }
}

void battery::run() {
    if ((state == sensorDeviceState::sampling) && samplingIsReady()) {
        uint32_t rawADC = readSample();

        if (channels[voltage].needsSampling()) {
            float batteryVoltage = voltageFromRaw(rawADC);
            channels[voltage].addSample(batteryVoltage);
            if (channels[voltage].hasOutput()) {
                channels[voltage].hasNewValue = true;
            }
        }

        if (channels[percentCharged].needsSampling()) {
            float batteryVoltage        = voltageFromRaw(rawADC);
            float batteryPercentCharged = chargeFromVoltage::calculateChargeLevel(batteryVoltage, type);
            channels[percentCharged].addSample(batteryPercentCharged);
            if (channels[percentCharged].hasOutput()) {
                channels[percentCharged].hasNewValue = true;
            }
        }

        state = sensorDeviceState::sleeping;
        adjustAllCounters();
    }
}

bool battery::anyChannelNeedsSampling() {
    return (channels[voltage].needsSampling() || channels[percentCharged].needsSampling());
}

void battery::adjustAllCounters() {
    channels[voltage].adjustCounters();
    channels[percentCharged].adjustCounters();
}

void battery::startSampling() {
#ifndef generic
    ADC_ChannelConfTypeDef theAdcConfig;
    theAdcConfig.Channel      = ADC_CHANNEL_VREFINT;
    theAdcConfig.Rank         = ADC_REGULAR_RANK_1;
    theAdcConfig.SamplingTime = ADC_SAMPLETIME_160CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc, &theAdcConfig);
    HAL_ADC_Start(&hadc);
#endif
}

bool battery::samplingIsReady() {
#ifndef generic
    HAL_StatusTypeDef result = HAL_ADC_PollForConversion(&hadc, 0);
    return (result == HAL_OK);
#else
    return true;
#endif
}

uint32_t battery::readSample() {
#ifndef generic
    return HAL_ADC_GetValue(&hadc);
#else
    return 1234;        // MOCK value
#endif
}

float battery::voltageFromRaw(uint32_t rawADC) {
#ifndef generic
    return static_cast<float>(__HAL_ADC_CALC_VREFANALOG_VOLTAGE((rawADC), ADC_RESOLUTION_12B)) / 1000.0f;
#else
    return 3.2F;        // MOCK value
#endif
}

void battery::addNewMeasurements() {
    for (uint32_t channelIndex = 0; channelIndex < nmbrChannels; channelIndex++) {
        if (channels[channelIndex].hasNewValue) {
            measurementCollection::addMeasurement(static_cast<uint32_t>(sensorDeviceType::battery), channelIndex, channels[channelIndex].getOutput());
        }
    }
}

void battery::log() {
    for (uint32_t channelIndex = 0; channelIndex < nmbrChannels; channelIndex++) {
        if (channels[channelIndex].hasNewValue) {
            float value       = valueAsFloat(channelIndex);
            uint32_t decimals = channelFormats[channelIndex].decimals;
            uint32_t intPart  = integerPart(value, decimals);
            uint32_t fracPart = fractionalPart(value, decimals);
            logging::snprintf(logging::source::sensorData, "%s = %d.%d %s\n", channelFormats[channelIndex].name, intPart, fracPart, channelFormats[channelIndex].unit);
        }
    }
}
