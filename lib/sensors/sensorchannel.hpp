// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>

class sensorChannel {
  public:
    sensorChannel();
    void set(uint32_t oversamplingLowPower, uint32_t prescalerLowPower, uint32_t oversamplingHighPower, uint32_t prescalerHighPower);
    enum class action : uint32_t {
        none,
        prescale,
        sample,
        sampleAndOutput
    };

    action getNextAction();
    bool needsSampling();
    bool hasOutput();
    void adjustCounters();

    void addSample(float theSample);
    float getOutput();
    

#ifndef unitTesting

  private:
#endif
    float lastValue{0.0F};
    bool hasNewValue{false};

    uint32_t oversamplingLowPower;        // low power = on battery
    uint32_t prescalerLowPower;
    uint32_t oversamplingHighPower;        // high power =  on USB power
    uint32_t prescalerHighPower;
    uint32_t oversamplingCounter{0};
    uint32_t prescaleCounter{0};

    static constexpr uint32_t maxPrescaler{4095};         // take a sample every x times of the 30 second RTC tick. 0 means : don't sample this sensor
    static constexpr uint32_t maxOversampling{15};        // average x+1 samples before storing it in the sample collection
    float samples[maxOversampling + 1]{};

    float average(uint32_t nmbrOfSamples) const;
    void limitOversamplingAndPrescaler();
    uint32_t getCurrentPrescaler() const;
    uint32_t getCurrentOversampling() const;
    void limitPrescaleCounter(uint32_t prescaler);
    void limitOversamplingCounter(uint32_t oversampling);
    bool isActive() const;

    friend class battery;
    friend class bme680;
    friend class tsl2591;
};