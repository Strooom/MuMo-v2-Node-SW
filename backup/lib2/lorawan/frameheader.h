// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

#include "deviceaddress.h"
#include "framecontrol.h"
#include "framecount.h"
#include "frameoptions.h"

class frameHeader {
  public:
    frameHeader(deviceAddress theDeviceAddress);                                                                // construct a frameHeader with the given deviceAddress
    frameHeader(deviceAddress theDeviceAddress, frameControl theFrameControl, frameCount theFrameCount);        //
    uint8_t getLength() const;                                                                                  // return the length of the frameHeader in bytes
    uint8_t asByte(uint8_t index) const;                                                                        // return the index-th frameHeader byte
    void fromBytes(uint8_t *buffer);                                                                            // decode the frameHeader from a buffer of received bytes
    uint8_t length();                                                                                           // return total length in [bytes]
  private:
    deviceAddress theDeviceAddress;
    frameControl theFrameControl;
    frameCount theFrameCount;
    frameOptions theFrameOptions;
};
