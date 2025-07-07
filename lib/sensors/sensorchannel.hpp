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
    void setIndex(uint32_t newOversamplingIndex, uint32_t newPrescalerIndex);
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

    static uint8_t compressOversamplingAndPrescalerIndex(uint32_t oversamplingIndex, uint32_t prescalerIndex) {
        if (oversamplingIndex > maxOversamplingIndex) {
            oversamplingIndex = maxOversamplingIndex;
        }
        if (prescalerIndex > maxPrescalerIndex) {
            prescalerIndex = maxPrescalerIndex;
        }
        return static_cast<uint8_t>((oversamplingIndex << 4) + prescalerIndex);
    };

    static uint32_t extractOversamplingIndex(uint8_t oversamplingAndPrescalerIndex) {
        uint32_t tmpOversamplingIndex = (oversamplingAndPrescalerIndex & 0b0011'0000) >> 4;
        if (tmpOversamplingIndex > maxOversamplingIndex) {
            tmpOversamplingIndex = maxOversamplingIndex;
        }
        return tmpOversamplingIndex;
    };

    static uint32_t extractPrescalerIndex(uint8_t oversamplingAndPrescalerIndex) {
        uint32_t tmpPrescalerIndex = (oversamplingAndPrescalerIndex & 0b000'1111);
        if (tmpPrescalerIndex > maxPrescalerIndex) {
            tmpPrescalerIndex = maxPrescalerIndex;
        }
        return tmpPrescalerIndex;
    };

    static constexpr uint32_t maxPrescalerIndex{13};
    static constexpr uint32_t maxOversamplingIndex{3};
    static constexpr uint32_t prescalerLookup[maxPrescalerIndex + 1]{0, 2, 4, 10, 20, 30, 60, 120, 240, 480, 720, 1440, 2880, 5760};
    static constexpr uint32_t oversamplingLookup[maxOversamplingIndex + 1]{1, 2, 4, 10};


    static uint8_t compressDeviceAndChannelIndex(uint8_t deviceIndex, uint8_t channelIndex) {
        return (((deviceIndex << 3) & 0b11111000) | (channelIndex & 0b00000111));
    };
    static uint8_t extractDeviceIndex(uint8_t deviceAndChannelIndex) {
        return (deviceAndChannelIndex >> 3);
    };
    static uint8_t extractChannelIndex(uint8_t deviceAndChannelIndex) {
        return (deviceAndChannelIndex & 0b00000111);
    };


#ifndef unitTesting

  private:
#endif
    bool initialized{false};

    uint32_t oversampling{0}; // old 
    uint32_t prescaling{0}; // old
    uint32_t oversamplingIndex{0}; // new, using an index in a lookup table to reduce possible values
    uint32_t prescalingIndex{0}; // new, using an index in a lookup table to reduce possible values
    uint32_t oversamplingCounter{0};
    uint32_t prescaleCounter{0};

    static constexpr uint32_t maxPrescaler{5760};        // take a sample every x times of the 30 second RTC tick. 0 means : don't sample this sensor. 5760 = 2 days
    static constexpr uint32_t maxOversampling{9};        // average x+1 samples before storing it in the sample collection
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