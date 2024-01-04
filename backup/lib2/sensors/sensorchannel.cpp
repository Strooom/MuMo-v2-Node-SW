#include "sensorchannel.h"
#include "power.h"

sensorChannel::sensorChannel() : type{sensorChannelType::none}, oversamplingLowPower{0}, prescalerLowPower{0}, oversamplingHighPower{0}, prescalerHighPower{0} {};

sensorChannel::sensorChannel(sensorChannelType type, uint32_t oversamplingLowPower, uint32_t prescalerLowPower, uint32_t oversamplingHighPower, uint32_t prescalerHighPower) : type{type},
                                                                                                                                                                               oversamplingLowPower{oversamplingLowPower},
                                                                                                                                                                               prescalerLowPower{prescalerLowPower},
                                                                                                                                                                               oversamplingHighPower{oversamplingHighPower},
                                                                                                                                                                               prescalerHighPower{prescalerHighPower} {
    limitOversamplingAndPrescaler();
}

void sensorChannel::limitOversamplingAndPrescaler() {
    if (oversamplingLowPower > maxOversampling) {
        oversamplingLowPower = maxOversampling;
    }
    if (prescalerLowPower > maxPrescaler) {
        prescalerLowPower = maxPrescaler;
    }
    if (oversamplingHighPower > maxOversampling) {
        oversamplingHighPower = maxOversampling;
    }
    if (prescalerHighPower > maxPrescaler) {
        prescalerHighPower = maxPrescaler;
    }
}

sensorChannel::action sensorChannel::getNextAction() {
    uint32_t prescaler    = getCurrentPrescaler();
    uint32_t oversampling = getCurrentOversampling();
    limitPrescaleCounter(prescaler);
    limitOversamplingCounter(oversampling);

    if (isActive()) {
        if (prescaleCounter > 0) {
            return action::prescale;
        } else {
            if (oversamplingCounter > 0) {
                return action::sample;
            } else {
                return action::output;
            }
        }
    } else {
        return action::none;
    }
}

void sensorChannel::adjustCounters() {
    if (isActive()) {
        uint32_t prescaler    = getCurrentPrescaler();
        uint32_t oversampling = getCurrentOversampling();
        limitPrescaleCounter(prescaler);
        limitOversamplingCounter(oversampling);
        if (prescaleCounter > 0) {
            prescaleCounter--;
        } else {
            prescaleCounter = getCurrentPrescaler() - 1;
            if (oversamplingCounter > 0) {
                oversamplingCounter--;
            } else {
                oversamplingCounter = getCurrentOversampling();
            }
        }
    } else {
        prescaleCounter     = 0;
        oversamplingCounter = 0;
    }
}

void sensorChannel::addSample(float theSample) {
    samples[oversamplingCounter] = theSample;
}

float sensorChannel::getOutput() {
    return average(getCurrentOversampling() + 1);
}

float sensorChannel::average(uint32_t nmbrOfSamples) const {
    if (nmbrOfSamples == 0) {
        nmbrOfSamples = 1;
    }
    if (nmbrOfSamples > (maxOversampling + 1)) {
        nmbrOfSamples = (maxOversampling + 1);
    }

    float sum{0.0F};
    for (uint32_t i = 0; i < nmbrOfSamples; i++) {
        sum += samples[i];
    }
    return (sum / nmbrOfSamples);
}

uint32_t sensorChannel::getCurrentPrescaler() const {
    if (power::hasUsbPower()) {
        return prescalerHighPower;
    } else {
        return prescalerLowPower;
    }
}

uint32_t sensorChannel::getCurrentOversampling() const {
    if (power::hasUsbPower()) {
        return oversamplingHighPower;
    } else {
        return oversamplingLowPower;
    }
}

void sensorChannel::limitPrescaleCounter(uint32_t activePrescaler) {
    if (prescaleCounter > activePrescaler) {        // when switching between low power and high power mode, the prescaleCounter could need to be reset in the appropriate range
        prescaleCounter = activePrescaler;
    }
}

void sensorChannel::limitOversamplingCounter(uint32_t activeOversampling) {
    if (oversamplingCounter > activeOversampling) {        // when switching between low power and high power mode, the oversamplingCounter could need to be reset in the appropriate range
        oversamplingCounter = activeOversampling;
    }
}

bool sensorChannel::isActive() const {
    return (getCurrentPrescaler() > 0);
}