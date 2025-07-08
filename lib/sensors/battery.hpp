// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once

#include <stdint.h>
#include <batterytype.hpp>
#include <sensordevicestate.hpp>
#include <sensorchannel.hpp>

class battery {
  public:
    static constexpr batteryType defaultBatteryType{batteryType::liFePO4_700mAh};
    static void setType(uint8_t index);
    static bool isValidType(batteryType newBatteryType);
    static void initialize(batteryType newBatteryType);
    static batteryType getType() { return type; };
    static void run();
    static sensorDeviceState getState() { return state; };
    static constexpr uint32_t nmbrChannels{2};
    static constexpr uint32_t voltage{0};
    static constexpr uint32_t stateOfCharge{1};

    static void startSampling();
    static bool samplingIsReady();
    static uint32_t readSample();
    static float voltageFromRaw(uint32_t rawADC);
#ifndef unitTesting

  private:
#endif
    static batteryType type;
    static sensorDeviceState state;
#ifdef generic
    static uint32_t mockBatteryRawADC;
    static float mockBatteryVoltage;
#endif
    static sensorChannel channels[nmbrChannels];

    friend class sensorDeviceCollection;
};
