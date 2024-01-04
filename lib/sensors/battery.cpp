#include <sensortype.hpp>
#include <battery.hpp>
//#include <settingscollection.hpp>
#include <sensorchannel.hpp>

// ### initialize static members ###
sensorDeviceState battery::state{sensorDeviceState::unknown};
sensorChannel battery::channels[nmbrChannels]{
    {sensorChannelType::batteryVoltage, 0, 0, 0, 0},
    {sensorChannelType::batteryChargeLevel, 0, 0, 0, 0},
};

void battery::initalize() {
//    type = constrainToValidValue(settingsCollection::read<uint8_t>(settingsCollection::settingIndex::batteryVersion));
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
