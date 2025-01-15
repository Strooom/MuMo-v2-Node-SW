// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <sensordevicetype.hpp>
#include <battery.hpp>
#include <chargefromvoltage.hpp>
#include <logging.hpp>
#include <float.hpp>

#ifndef generic
#include <main.h>
extern ADC_HandleTypeDef hadc;
#else
uint32_t battery::mockBatteryRawADC{0};
float battery::mockBatteryVoltage{0};
#endif

batteryType battery::type{batteryType::nmbrBatteryTypes};
sensorDeviceState battery::state{sensorDeviceState::unknown};
sensorChannel battery::channels[nmbrChannels] = {
    {2, "voltage", "V"},
    {0, "percentCharged", "%"},
};

void battery::setType(uint8_t index) {
    if (index < static_cast<uint8_t>(batteryType::nmbrBatteryTypes)) {
        type = static_cast<batteryType>(index);
    }
}

bool battery::isValidType(batteryType newBatteryType) {
    return (newBatteryType < batteryType::nmbrBatteryTypes);
}

void battery::initialize(batteryType newBatteryType) {
    type  = newBatteryType;
    state = sensorDeviceState::sleeping;
    for (uint32_t channelIndex = 0; channelIndex < nmbrChannels; channelIndex++) {
        channels[channelIndex].set(0, 0);
        channels[channelIndex].hasNewValue = false;
    }
}

void battery::run() {
    if ((state == sensorDeviceState::sampling) && samplingIsReady()) {
        uint32_t rawADC             = readSample();
        float batteryVoltage        = voltageFromRaw(rawADC);
        float batteryPercentCharged = chargeFromVoltage::calculateChargeLevel(batteryVoltage, type);

        if (channels[voltage].needsSampling()) {
            channels[voltage].addSample(batteryVoltage);
            if (channels[voltage].hasOutput()) {
                channels[voltage].hasNewValue = true;
            }
        }
        if (channels[percentCharged].needsSampling()) {
            channels[percentCharged].addSample(batteryPercentCharged);
            if (channels[percentCharged].hasOutput()) {
                channels[percentCharged].hasNewValue = true;
            }
        }
        state = sensorDeviceState::sleeping;
    }
}

void battery::startSampling() {
    state = sensorDeviceState::sampling;
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
    return mockBatteryRawADC;
#endif
}

float battery::voltageFromRaw(uint32_t rawADC) {
#ifndef generic
    return static_cast<float>(__HAL_ADC_CALC_VREFANALOG_VOLTAGE((rawADC), ADC_RESOLUTION_12B)) / 1000.0f;
#else
    return mockBatteryVoltage;
#endif
}
