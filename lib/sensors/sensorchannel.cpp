#include "sensorchannel.hpp"
#include <stdio.h>           // snprintf
#include <inttypes.h>        // for PRIu32

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

bool sensorChannel::isActive() const {
    return (prescaling > 0);
}
