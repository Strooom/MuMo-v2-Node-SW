// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>

class sensorChannel {
  public:
    sensorChannel(uint32_t decimals, const char *name, const char *unit);
    void set(uint32_t oversampling, uint32_t prescaler, float initialSampleValue = 0.0F);
    enum class action : uint32_t {
        none,
        prescale,
        sample,
        sampleAndOutput
    };

    action getNextAction();
    bool needsSampling();
    bool hasOutput();
    void updateCounters();

    void addSample(float theSample);
    float value();

#ifndef unitTesting

  private:
#endif
    bool hasNewValue{false};

    uint32_t oversampling;
    uint32_t prescaling;
    uint32_t oversamplingCounter{0};
    uint32_t prescaleCounter{0};

    static constexpr uint32_t maxPrescaler{4095};         // take a sample every x times of the 30 second RTC tick. 0 means : don't sample this sensor
    static constexpr uint32_t maxOversampling{7};         // average x+1 samples before storing it in the sample collection
    float samples[maxOversampling + 1]{};

    void limitOversamplingAndPrescaler();
    void limitPrescaleCounter(uint32_t prescaler);
    void limitOversamplingCounter(uint32_t oversampling);
    bool isActive() const;

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