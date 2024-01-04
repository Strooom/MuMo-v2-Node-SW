// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once

#include <stdint.h>
#include <sensordevicestate.hpp>
#include <batterytype.hpp>
#include <interpolation.hpp>

// #include "sensortype.hpp"
// #include "sensorchannel.hpp"

class battery {
  public:
    static bool isPresent() { return true; };
    static sensorDeviceState getState() { return state; };
    static float getLastChannelValue(uint32_t index);
    static void tick();
    static void run();

    static constexpr uint32_t nmbrChannels{2};
    static sensorChannel channels[nmbrChannels];

#ifndef unitTesting

  private:
#endif
    static sensorDeviceState state;

    // ### sensorDevice internal methods / helper functions ###

    static batteryType type;
    static constexpr uint32_t nmbrInterpolationPoints{8};
    static const interpolationPoint voltageVsCharge[nmbrBatteryTypes][nmbrInterpolationPoints];

    static void initalize();        // initial config of the ADC
    static void sample();           // read ADC, translate into mV and SoC

    static float getVoltage();
    static float getChargeLevel();

    static void goSleep(){};        // ADC is auto-power-down, so nothing needed here

    static bool checkVoltageVsCharge(uint32_t batteryTypeIndex);        // this checks that the voltage/charge curve is monotone, ie that the voltage is always increasing when the charge is increasing
    static uint8_t calculateChargeLevel(float voltage);
};