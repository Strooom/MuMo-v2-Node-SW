#include <chargefromvoltage.hpp>

const interpolationPoint chargeFromVoltage::voltageVsCharge[nmbrBatteryTypes][nmbrInterpolationPoints] = {
    {
        {2.80F, 0.0f},
        {3.00F, 0.1f},
        {3.10F, 0.2f},
        {3.15F, 0.4f},
        {3.25F, 0.6f},
        {3.30F, 0.8f},
        {3.40F, 0.9f},
        {3.60F, 1.0f},
    },
    {
        {2.80F, 0},
        {3.00F, 25},
        {3.10F, 50},
        {3.15F, 100},
        {3.25F, 150},
        {3.30F, 200},
        {3.40F, 225},
        {3.60F, 255},
    },
    {
        {2.00F, 0},
        {2.10F, 25},
        {2.30F, 50},
        {2.40F, 100},
        {2.50F, 150},
        {2.60F, 200},
        {2.90F, 225},
        {3.00F, 255},
    },
    {
        {2.80F, 0},
        {3.00F, 25},
        {3.10F, 50},
        {3.20F, 100},
        {3.20F, 150},
        {3.30F, 200},
        {3.40F, 225},
        {3.60F, 255},
    }};


bool chargeFromVoltage::checkVoltageVsCharge(uint32_t batteryTypeIndex) {
    for (uint32_t interpolationPointIndex = 0; interpolationPointIndex < nmbrInterpolationPoints - 2; interpolationPointIndex++) {
        if (voltageVsCharge[batteryTypeIndex][interpolationPointIndex].voltage >= voltageVsCharge[batteryTypeIndex][interpolationPointIndex + 1].voltage) {
            return false;
        }
    }
    return true;
}


float chargeFromVoltage::calculateChargeLevel(float voltage, batteryType type) {
    uint32_t typeIndex = static_cast<uint32_t>(type);
    if (typeIndex >= nmbrBatteryTypes) {
        typeIndex = 0;
    }

    if (voltage <= voltageVsCharge[typeIndex][0].voltage) {
        return 0.0f;
    }
    if (voltage >= voltageVsCharge[typeIndex][nmbrInterpolationPoints - 1].voltage) {
        return 1.0f;
    }
    for (uint32_t interpolationPointIndex = 0; interpolationPointIndex < nmbrInterpolationPoints - 1; interpolationPointIndex++) {
        if ((voltage >= voltageVsCharge[typeIndex][interpolationPointIndex].voltage) &&
            (voltage <= voltageVsCharge[typeIndex][interpolationPointIndex + 1].voltage)) {
            float A = voltageVsCharge[typeIndex][interpolationPointIndex].charge;
            float B = voltageVsCharge[typeIndex][interpolationPointIndex + 1].charge - voltageVsCharge[typeIndex][interpolationPointIndex].charge;
            float C   = voltage - voltageVsCharge[typeIndex][interpolationPointIndex].voltage;
            float D   = voltageVsCharge[typeIndex][interpolationPointIndex + 1].voltage - voltageVsCharge[typeIndex][interpolationPointIndex].voltage;

            return (A + B * (C / D));
        }
    }
    return 0.0f;
}

