// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>

class sensorChannel {
  public:
    sensorChannel(uint32_t decimals, const char* name, const char* unit);
    void set(uint32_t oversampling, uint32_t prescaler);
    enum class action : uint32_t {
        none,
        prescale,
        sample,
        sampleAndOutput
    };

    action getNextAction() const;
    bool needsSampling() const;
    bool hasOutput() const;
    void updateCounters();

    void addSample(float theSample);
    float value() const;

    uint32_t getOversampling() const {
        return oversampling;
    }
    uint32_t getPrescaler() const {
        return prescaling;
    }
    uint32_t getNumberOfSamplesToAverage() const {
        return oversampling + 1;
    }
    uint32_t getMinutesBetweenOutput() const {
        if (prescaling == 0) {
            return 0;
        }
        return ((oversampling + 1) * prescaling) / 2;
    }

    bool isActive() const { return (prescaling > 0); };

    static uint32_t calculateOversampling(uint32_t numberOfSamplesToAverage);
    static uint32_t calculatePrescaler(uint32_t minutesBetweenOutput, uint32_t numberOfSamplesToAverage);

    static uint16_t compressOversamplingAndPrescaler(uint32_t oversampling, uint32_t prescaler) {
        if (oversampling > maxOversampling) {
            oversampling = maxOversampling;
        }
        if (prescaler > maxPrescaler) {
            prescaler = maxPrescaler;
        }
        return static_cast<uint16_t>((oversampling << 13) + prescaler);
    };

    static uint32_t extractOversampling(uint16_t oversamplingAndPrescaler) {
        return (oversamplingAndPrescaler & 0b1110'0000'0000'0000) >> 13;
    };

    static uint32_t extractPrescaler(uint16_t oversamplingAndPrescaler) {
        return (oversamplingAndPrescaler & 0b0001'1111'1111'1111);
    };

#ifndef unitTesting

  private:
#endif
    bool initialized{false};

    uint32_t oversampling{0};
    uint32_t prescaling{0};
    uint32_t oversamplingCounter{0};
    uint32_t prescaleCounter{0};

    static constexpr uint32_t maxPrescaler{8191};        // take a sample every x times of the 30 second RTC tick. 0 means : don't sample this sensor
    static constexpr uint32_t maxOversampling{7};        // average x+1 samples before storing it in the sample collection
    float samples[maxOversampling + 1]{};

    void limitOversamplingAndPrescaler();

    const uint32_t decimals;
    const char* name;
    const char* unit;

    friend class battery;
    friend class bme680;
    friend class tsl2591;
    friend class sht40;
    friend class sps30;
    friend class sensorDeviceCollection;
};