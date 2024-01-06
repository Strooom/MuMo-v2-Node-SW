#include <sensortype.hpp>
#include <battery.hpp>
#include <settingscollection.hpp>
#include <sensorchannel.hpp>

#ifndef generic
#include <main.h>
extern ADC_HandleTypeDef hadc;
#endif

// ### initialize static members ###
batteryType battery::type{batteryType::liFePO4_700mAh};
sensorDeviceState battery::state{sensorDeviceState::unknown};
sensorChannel battery::channels[nmbrChannels]{
    {sensorChannelType::batteryVoltage, 0, 1, 0, 1},
    {sensorChannelType::batteryChargeLevel, 0, 0, 0, 0},
};

void battery::initalize() {
    uint8_t typeIndex = settingsCollection::read<uint8_t>(settingsCollection::settingIndex::batteryVersion);
    if (typeIndex >= nmbrBatteryTypes) {
        typeIndex = 0;
    }
    type  = static_cast<batteryType>(typeIndex);
    state = sensorDeviceState::sleeping;
}

float battery::getLastChannelValue(uint32_t index) {
    return channels[index].getOutput();
}

void battery::tick() {
    if (anyChannelNeedsSampling()) {
        // TODO : should I check if sensorDevice is in sleeping state ?
        battery::startSampling();
        state = sensorDeviceState::sampling;
    } else {
        channels[voltage].adjustCounters();
        channels[percentCharged].adjustCounters();
    }
}

void battery::run() {
    if ((state == sensorDeviceState::sampling) && battery::samplingIsReady()) {
        uint32_t rawADC = readSample();
        // volatile float batteryVoltage  = static_cast<float>(__HAL_ADC_CALC_VREFANALOG_VOLTAGE((rawADC), ADC_RESOLUTION_12B)) / 1000.0f;


    	if (channels[voltage].needsSampling()) {
            // channels[voltage].lastValue = ADC::getVoltage();
        }
        if (channels[percentCharged].needsSampling()) {
            // channels[percentCharged].lastValue = ADC::getChargeLevel();
        }

        for (auto index = 0U; index < nmbrChannels; index++) {
            if (channels[index].hasOutput()) {
                channels[index].hasNewValue = true;
            }
        }

        state = sensorDeviceState::sleeping;
        channels[voltage].adjustCounters();
        channels[percentCharged].adjustCounters();
    }
}

bool battery::anyChannelNeedsSampling() {
    return (channels[voltage].needsSampling() || channels[percentCharged].needsSampling());
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
    return (HAL_ADC_GetState(&hadc) == HAL_ADC_STATE_REG_EOC);
#else
    return true;
#endif
}

uint32_t battery::readSample() {
#ifndef generic
    uint32_t adcRawResult = HAL_ADC_GetValue(&hadc);
#else
    uint32_t adcRawResult = 1234;        // MOCK value
#endif
    return adcRawResult;
}
