#include "sensorchannel.hpp"
#include <stdio.h>           // snprintf
#include <inttypes.h>        // for PRIu32

sensorChannel::sensorChannel(uint32_t decimals, const char* name, const char* unit) : oversampling{0}, prescaling{0}, decimals{decimals}, name{name}, unit{unit} {
    limitOversamplingAndPrescaler();
}

void sensorChannel::set(uint32_t newOversampling, uint32_t newPrescaler, float initialSampleValue) {
    oversampling = newOversampling;
    prescaling   = newPrescaler;
    limitOversamplingAndPrescaler();
    oversamplingCounter = 0;
    prescaleCounter     = 0;
    for (uint32_t i = 0; i < (maxOversampling + 1); i++) {
        samples[i] = initialSampleValue;
    }
}

bool sensorChannel::needsSampling() {
    action anAction = getNextAction();
    return ((anAction == action::sample) || (anAction == action::sampleAndOutput));
};

bool sensorChannel::hasOutput() {
    return (getNextAction() == action::sampleAndOutput);
};

void sensorChannel::limitOversamplingAndPrescaler() {
    if (oversampling > maxOversampling) {
        oversampling = maxOversampling;
    }
    if (prescaling > maxPrescaler) {
        prescaling = maxPrescaler;
    }
}

sensorChannel::action sensorChannel::getNextAction() {
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
    samples[oversamplingCounter] = theSample;
}

float sensorChannel::value() {
    uint32_t nmbrOfSamples = oversampling + 1;
    float sum{0.0F};
    for (uint32_t i = 0; i < nmbrOfSamples; i++) {
        sum += samples[i];
    }
    return (sum / nmbrOfSamples);
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
    return (prescaling > 0);
}

void sensorChannel::buildBigTextString(char* destination, uint32_t maxLength) {
    snprintf(destination, maxLength, "%" PRId32, value());
}

void sensorChannel::buildSmallTextString(char* destination, uint32_t maxLength) {
    if (decimals > 0) {
        snprintf(destination, maxLength, "%" PRIu32 " %s", value(), unit);
    } else {
        snprintf(destination, maxLength, "%s", unit);
    }
}
