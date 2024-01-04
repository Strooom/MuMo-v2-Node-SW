// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include "sensortype.h"

const char* toString(sensorChannelType type) {
    switch (type) {
        case sensorChannelType::batteryChargeLevel:
            return "batteryChargeLevel";
            break;

        case sensorChannelType::batteryVoltage:
            return "batteryChargeLevel";
            break;

        case sensorChannelType::BME680Temperature:
            return "BME680 - Temperature";
            break;

        case sensorChannelType::BME680RelativeHumidity:
            return "BME680 - Relative Humidity";
            break;

        case sensorChannelType::BME680BarometricPressure:
            return "BME680 - Barometer";
            break;

        case sensorChannelType::TSL25911VisibleLight:
            return "TSL25911 - Visible Light";
            break;

        case sensorChannelType::TSL25911Infrared:
            return "TSL25911 - Infrared Light";
            break;

        default:
            return "unknown infoChannel";
            break;
    }
}