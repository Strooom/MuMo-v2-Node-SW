#include <sensortype.hpp>
#include <battery.hpp>
#include <settingscollection.hpp>
#include <sensorchannel.hpp>

// ### initialize static members ###
batteryType battery::type{batteryType::liFePO4_700mAh};
sensorDeviceState battery::state{sensorDeviceState::unknown};
sensorChannel battery::channels[nmbrChannels]{
    {sensorChannelType::batteryVoltage, 0, 0, 0, 0},
    {sensorChannelType::batteryChargeLevel, 0, 0, 0, 0},
};

void battery::initalize() {
    uint8_t typeIndex = settingsCollection::read<uint8_t>(settingsCollection::settingIndex::batteryVersion);
    if (typeIndex >= nmbrBatteryTypes) {
        typeIndex = 0;
    }
    type = static_cast<batteryType>(typeIndex);
    state = sensorDeviceState::sleeping;
}

float battery::getLastChannelValue(uint32_t index) {
    return channels[index].getOutput();
}

void battery::tick() {
    if (anyChannelNeedsSampling()) {
        // TODO : should I check if sensorDevice is in sleeping state ?
        // ADC.initalize();
        // ADC.startSampling();
        state = sensorDeviceState::sampling;
    } else {
        channels[voltage].adjustCounters();
        channels[percentCharged].adjustCounters();
    }
}

void battery::run() {
    if ((state == sensorDeviceState::sampling)  /*&& stm32wle5_adc::isReady()*/) {
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


// void battery::initalize() {
//     type = constrainToValidValue(settingsCollection::read<uint8_t>(settingsCollection::settingIndex::batteryVersion));
// #ifndef generic
//     hadc.Instance                        = ADC;
//     hadc.Init.ClockPrescaler             = ADC_CLOCK_SYNC_PCLK_DIV1;
//     hadc.Init.Resolution                 = ADC_RESOLUTION_12B;
//     hadc.Init.DataAlign                  = ADC_DATAALIGN_RIGHT;
//     hadc.Init.ScanConvMode               = ADC_SCAN_DISABLE;
//     hadc.Init.EOCSelection               = ADC_EOC_SINGLE_CONV;
//     hadc.Init.LowPowerAutoWait           = DISABLE;
//     hadc.Init.LowPowerAutoPowerOff       = ENABLE;
//     hadc.Init.ContinuousConvMode         = DISABLE;
//     hadc.Init.NbrOfConversion            = 1;
//     hadc.Init.DiscontinuousConvMode      = DISABLE;
//     hadc.Init.ExternalTrigConv           = ADC_SOFTWARE_START;
//     hadc.Init.ExternalTrigConvEdge       = ADC_EXTERNALTRIGCONVEDGE_NONE;
//     hadc.Init.DMAContinuousRequests      = DISABLE;
//     hadc.Init.Overrun                    = ADC_OVR_DATA_PRESERVED;
//     hadc.Init.SamplingTimeCommon1        = ADC_SAMPLETIME_79CYCLES_5;
//     hadc.Init.SamplingTimeCommon2        = ADC_SAMPLETIME_79CYCLES_5;
//     hadc.Init.OversamplingMode           = DISABLE;
//     hadc.Init.Oversampling.Ratio         = ADC_OVERSAMPLING_RATIO_8;
//     hadc.Init.Oversampling.RightBitShift = ADC_RIGHTBITSHIFT_NONE;
//     hadc.Init.Oversampling.TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;
//     hadc.Init.TriggerFrequencyMode       = ADC_TRIGGER_FREQ_HIGH;
//     if (HAL_ADC_Init(&hadc) != HAL_OK) {
//         Error_Handler();
//     }
// #endif
// }

// void battery::sample() {
// #ifndef generic

//     HAL_ADCEx_Calibration_Start(&hadc);
//     ADC_ChannelConfTypeDef theAdcConfig;
//     theAdcConfig.Channel      = ADC_CHANNEL_VREFINT;
//     theAdcConfig.Rank         = ADC_REGULAR_RANK_1;
//     theAdcConfig.SamplingTime = ADC_SAMPLETIME_160CYCLES_5;
//     HAL_ADC_ConfigChannel(&hadc, &theAdcConfig);
//     HAL_ADC_Start(&hadc);
//     HAL_ADC_PollForConversion(&hadc, 1);
//     uint32_t adcRawResult = HAL_ADC_GetValue(&hadc);
//     batteryVoltage        = static_cast<float>(__HAL_ADC_CALC_VREFANALOG_VOLTAGE((adcRawResult), ADC_RESOLUTION_12B)) / 1000.0f;
// #else
//     batteryVoltage = 3.2F;
// #endif
//     batteryChargeLevel = calculateChargeLevel(batteryVoltage);
// }

