// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include "sensor.h"

// Represents a Bosch BME680 sensor

class bme680 : public sensor {
  public:
    static bool isPresent();        // detect if there is an BME680 on the I2C bus
    const char* getName();
    static bool reset();         // soft-reset
    bool initialize();           //
    bool doMeasurement();        //

    static constexpr uint8_t i2cAddress{0x76};        // default I2C address for this sensor, DSO tied to GND on our hardware

    // Registers
    static constexpr uint8_t chipIdRegister{0xD0};        // address of register holding chipId

    // Commands
    static constexpr uint8_t softResetCommand{0xB6};

    // Other
    static constexpr uint8_t chipIdValue{0x61};        // value to expect at the chipIdregister, this allows to discover/recognize the BME68x

  private:
    bool readRegisters() const;                // TODO add parameters
    bool writeRegisters();                     // TODO add parameters
    void calculateTemperature();               //
    void calculateBarometricPressure();        //
    void calculateRelativeHumidity();          //
};