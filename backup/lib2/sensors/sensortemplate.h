// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include "sensortype.h"
# include "sensorchannel.h"

class sensorDevice {
  public:
    static bool isPresent();
    static void initalize();
    static void sample(); // TODO : should become startSample() and isSampleReady() so we become non-blocking

    static float getTemperature();
    static float getRelativeHumidity();
    static float getBarometricPressure();

    static void goSleep();

    static constexpr uint32_t nmbrChannels{3};        // eg. temperature, relative humidity, barometric pressure
    static constexpr sensorChannelType channels[nmbrChannels]{
        sensorChannelType::BME680Temperature,
        sensorChannelType::BME680RelativeHumidity,
        sensorChannelType::BME680BarometricPressure,
    };

    static sensorChannel channels[nmbrChannels]; // now call the constructor for each channel

#ifndef unitTesting

  private:
#endif

    // ### sensorDevice registers ###
    enum class registers : uint8_t {
        chipId = 0xD0,        // address of register holding chipId
        reset  = 0xE0,        // address of register to reset the device : Write 0xB6
    };

    // ### sensorDevice commands ###
    enum class commands : uint8_t {
        softReset = 0xB6,        // value to write to resetRegister to reset the BME68X
    };

    // ### sensorDevice properties  / magic values ###
    static constexpr uint8_t chipIdValue{0x61};        // value to expect at the chipIdregister, this allows to discover/recognize the BME68x
};