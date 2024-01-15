// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <battery.hpp>
#include <settingscollection.hpp>
#include <chargefromvoltage.hpp>
#include <logging.hpp>

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
    uint8_t typeIndex = settingsCollection::read<uint8_t>(settingsCollection::settingIndex::batteryVersion);
    if (typeIndex >= nmbrBatteryTypes) {
        typeIndex = 0;
    }

    // TODO : need to read the sensorChannel settins from EEPROM and restore them
    // channels[voltage].set(0,1,0,1);
    // channels[percentCharged].set(0,1,0,1);

    type  = static_cast<batteryType>(typeIndex);
    state = sensorDeviceState::sleeping;
}

bool battery::hasNewMeasurement() {
    return (channels[voltage].hasNewValue || channels[percentCharged].hasNewValue);
}

void battery::clearNewMeasurements() {
    channels[voltage].hasNewValue        = false;
    channels[percentCharged].hasNewValue = false;
}

float battery::valueAsFloat(uint32_t index) {
    return channels[index].getOutput();
}

void battery::tick() {
    if (state != sensorDeviceState::sleeping) {
        adjustAllCounters();
        return;
    }

    if (anyChannelNeedsSampling()) {
        clearNewMeasurements();
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
            // logging::snprintf(logging::source::sensorData, "%s = %.2f %s\n", toString(channels[voltage].type), batteryVoltage, postfix(channels[voltage].type));
            if (channels[voltage].hasOutput()) {
                channels[voltage].hasNewValue = true;
            }
        }

        if (channels[percentCharged].needsSampling()) {
            float batteryVoltage        = voltageFromRaw(rawADC);
            float batteryPercentCharged = chargeFromVoltage::calculateChargeLevel(batteryVoltage, type);
            channels[percentCharged].addSample(batteryPercentCharged);
            // logging::snprintf(logging::source::sensorData, "%s = %.2f\n", toString(channels[percentCharged].type), batteryPercentCharged, postfix(channels[percentCharged].type));
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
