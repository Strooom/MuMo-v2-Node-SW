// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include "sensordevice.h"
#include "sensordevicetype.h"

class sensorDeviceCollection {
  public:
    sensorDeviceCollection() = delete;
    static void discover();        // discover all sensor devices that are present by scanning the I2C addresses and checking register-values
    static void run();             // service all sensor devices that are present

#ifndef unitTesting

  private:
#endif

    static bool isPresent[static_cast<uint32_t>(sensorDeviceType::nmbrOfKnownDevices)];        // an array of bools, with true for each known device that is present
    static uint32_t actualNumberOfDevices;
};