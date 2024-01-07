#include <sensorchanneltype.hpp>
#include <battery.hpp>
#include <settingscollection.hpp>
#include <sensorchannel.hpp>
#include <chargefromvoltage.hpp>


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
    if (state != sensorDeviceState::sleeping) {
        adjustAllCounters();
        return;
    }

    if (anyChannelNeedsSampling()) {
        battery::startSampling();
        state = sensorDeviceState::sampling;
    } else {
        adjustAllCounters();
    }
}

void battery::run() {
    if ((state == sensorDeviceState::sampling) && battery::samplingIsReady()) {
        uint32_t rawADC = readSample();

        if (channels[voltage].needsSampling()) {
            float batteryVoltage = voltageFromRaw(rawADC);
            channels[voltage].addSample(batteryVoltage);
            if (channels[voltage].hasOutput()) {
                channels[voltage].hasNewValue = true;
            }
        }

        if (channels[percentCharged].needsSampling()) {
            float batteryVoltage = voltageFromRaw(rawADC);
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
    return static_cast<float>(__HAL_ADC_CALC_VREFANALOG_VOLTAGE((rawADC), ADC_RESOLUTION_12B)) / 125.0f;
#else
    return 3.2F;        // MOCK value
#endif
}