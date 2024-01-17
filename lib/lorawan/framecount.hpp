// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

class frameCount {
  public:
    static constexpr uint32_t length{4};                                                     // length of the frameCount in bytes
    frameCount();                                                                            //
    explicit frameCount(uint32_t theFrameCount);                                             //
    void set(uint32_t theFrameCount);                                                        // set the frameCount from a uint32_t
    frameCount& operator=(uint32_t theFrameCount);                                           //
    void set(uint8_t theFrameCount[length]);                                                 // set the frameCount from an array of 4 bytes
    void increment();                                                                        //
    static constexpr uint32_t maximumGap{256};                                               // maximum gap between last (valid) received framecount and next received framecount.
    static uint32_t guessFromUint16(uint32_t frameCount32, uint16_t frameCount16Lsb);        // guess the 32 bit framecount from a 16-bit lsb value

    union {
        uint32_t asUint32{0};
        uint8_t asUint8[length];
    };

  private:
};
