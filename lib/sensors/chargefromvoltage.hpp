// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <batterytype.hpp>

struct interpolationPoint {
    float voltage;         // [mV]
    float charge;        // [0..1]
};

class chargeFromVoltage {
  public:
    static constexpr uint32_t nmbrInterpolationPoints{8};
    static const interpolationPoint voltageVsCharge[nmbrBatteryTypes][nmbrInterpolationPoints];
    static bool checkVoltageVsCharge(uint32_t batteryTypeIndex);        // this checks that the voltage/charge curve is monotone, ie that the voltage is always increasing when the charge is increasing
    static float calculateChargeLevel(float voltage, batteryType type);
};

