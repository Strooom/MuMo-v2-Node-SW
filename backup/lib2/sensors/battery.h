// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include "batterytype.h"
#include "sensortype.h"
#include "sensorchannel.h"

struct interpolationPoint {
    float voltage;         // [mV]
    uint8_t charge;        // [%, 0-255]
};

constexpr uint32_t nmbrInterpolationPoints{8};

class battery {
  public:
    static bool isPresent() { return true; };
    static void initalize();        // initial config of the ADC
    static void sample();           // read ADC, translate into mV and SoC

    static float getVoltage();
    static float getChargeLevel();

    static void goSleep(){};        // ADC is auto-power-down, so nothing needed here

    static constexpr uint32_t nmbrChannels{2};
    static sensorChannel channels[nmbrChannels];

#ifndef unitTesting

  private:
#endif
    // ### sensorDevice registers ###
    // ### sensorDevice commands ###
    // ### sensorDevice properties  / magic values ###
    static float batteryVoltage;
    static float batteryChargeLevel;

    // ### sensorDevice internal methods / helper functions ###

    static batteryType type;
    static const interpolationPoint voltageVsCharge[nmbrBatteryTypes][nmbrInterpolationPoints];

    static bool checkVoltageVsCharge(uint32_t batteryTypeIndex);        // this checks that the voltage/charge curve is monotone, ie that the voltage is always increasing when the charge is increasing
    static uint8_t calculateChargeLevel(float voltage);
};