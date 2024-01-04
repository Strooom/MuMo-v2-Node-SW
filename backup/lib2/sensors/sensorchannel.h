// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include "sensortype.h"
#include "sensorstate.h"

class sensorChannel {
  public:
    sensorChannel(sensorChannelType type, uint32_t oversamplingLowPower, uint32_t prescalerLowPower, uint32_t oversamplingHighPower, uint32_t prescalerHighPower);
    sensorChannel();

    enum class action : uint32_t {
        none,
        prescale,
        sample,
        output
    };

    action getNextAction();
    void adjustCounters();
    void addSample(float theSample);
    float getOutput();

#ifndef unitTesting

  private:
#endif

    sensorChannelType type;
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

    friend class sensorChannelCollection;
};