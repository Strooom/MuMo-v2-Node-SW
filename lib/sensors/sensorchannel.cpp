#include "sensorchannel.hpp"
// #include <stdio.h>           // snprintf
// #include <inttypes.h>        // for PRIu32

sensorChannel::sensorChannel(uint32_t decimals, const char* name, const char* unit) : decimals{decimals}, name{name}, unit{unit} {
    limitOversamplingAndPrescaler();
}

void sensorChannel::set(uint32_t newOversampling, uint32_t newPrescaler) {
    oversampling = newOversampling;
    prescaling   = newPrescaler;
    limitOversamplingAndPrescaler();
    oversamplingCounter = 0;
    prescaleCounter     = 0;
}

void sensorChannel::setIndex(uint32_t newOversamplingIndex, uint32_t newPrescalerIndex) {
    if (newOversamplingIndex <= maxOversamplingIndex) {
        oversamplingIndex = newOversamplingIndex;
    } else {
        oversamplingIndex = maxOversamplingIndex;
    }
    oversampling = oversamplingLookup[oversamplingIndex] - 1;

    if (newPrescalerIndex <= maxPrescalerIndex) {
        prescalingIndex = newPrescalerIndex;
    } else {
        prescalingIndex = maxPrescalerIndex;
    }
    prescaling = prescalerLookup[prescalingIndex] / (oversampling + 1);

    oversamplingCounter = 0;        // resetting the counters here ensures that when choosing a smaller oversampling or prescaler, the counters will always be lower than the maximum values
    prescaleCounter     = 0;
}

bool sensorChannel::needsSampling() const {
    return (isActive() && prescaleCounter == 0);
}

bool sensorChannel::hasOutput() const {
    return (isActive() && (prescaleCounter == 0) && (oversamplingCounter == 0));
}

void sensorChannel::limitOversamplingAndPrescaler() {
    if (oversampling > maxOversampling) {
        oversampling = maxOversampling;
    }
    if (prescaling > maxPrescaler) {
        prescaling = maxPrescaler;
    }
}

sensorChannel::action sensorChannel::getNextAction() const {
    if (isActive()) {
        if (prescaleCounter > 0) {
            return action::prescale;
        } else {
            if (oversamplingCounter > 0) {
                return action::sample;
            } else {
                return action::sampleAndOutput;
            }
        }
    } else {
        return action::none;
    }
}

void sensorChannel::updateCounters() {
    if (isActive()) {
        if (prescaleCounter > 0) {
            prescaleCounter--;
        } else {
            prescaleCounter = prescaling - 1;
            if (oversamplingCounter > 0) {
                oversamplingCounter--;
            } else {
                oversamplingCounter = oversampling;
            }
        }
    } else {
        prescaleCounter     = 0;
        oversamplingCounter = 0;
    }
}

void sensorChannel::addSample(float theSample) {
    if (initialized) {
        samples[oversamplingCounter] = theSample;
    } else {
        for (uint32_t i = 0; i < (maxOversampling + 1); i++) {
            samples[i] = theSample;
        }
        initialized = true;
    }
}

float sensorChannel::value() const {
    uint32_t nmbrOfSamples = oversampling + 1;
    float sum{0.0F};
    for (uint32_t i = 0; i < nmbrOfSamples; i++) {
        sum += samples[i];
    }
    return (sum / static_cast<float>(nmbrOfSamples));
}

uint32_t sensorChannel::calculateOversampling(uint32_t numberOfSamplesToAverage) {
    if (numberOfSamplesToAverage <= 1) {
        return 0;
    }
    if (numberOfSamplesToAverage >= (maxOversampling + 1)) {
        return maxOversampling;
    }
    return (numberOfSamplesToAverage - 1);
}

uint32_t sensorChannel::calculatePrescaler(uint32_t minutesBetweenOutput, uint32_t numberOfSamplesToAverage) {
    if (minutesBetweenOutput <= 1) {
        minutesBetweenOutput = 1;
    }
    if (numberOfSamplesToAverage == 0) {
        numberOfSamplesToAverage = 1;
    }
    if (numberOfSamplesToAverage >= (maxOversampling + 1)) {
        numberOfSamplesToAverage = maxOversampling + 1;
    }
    uint32_t ticksPerOutput = (minutesBetweenOutput * 2);
    if (ticksPerOutput <= numberOfSamplesToAverage) {
        return 1;
    }
    return (ticksPerOutput / numberOfSamplesToAverage);
}
