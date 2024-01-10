// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <sensordevicetype.hpp>
#include <sensorchanneltype.hpp>

class sensorDeviceCollection {
  public:
    sensorDeviceCollection() = delete;        // fully static, cannot instantiate this class
    static void discover();                   // discover all sensor devices that are present by scanning the I2C addresses and checking register-values
    static void tick();                      // service all sensor devices after a RTC tick
    static void run();                        // service all sensor devices until they are back to sleep
    static bool isSleeping();                 // return true if all sensor devices are sleeping, so MCU could go to sleep as well

    static bool hasNewMeasurement(sensorChannelType aChannelType);
    static float getMeasurement(sensorChannelType aChannelType);

#ifndef unitTesting

  private:
#endif

    static bool isPresent[static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices)];        // an array of bools, with true for each known device that is present
    static uint32_t actualNumberOfDevices;
};