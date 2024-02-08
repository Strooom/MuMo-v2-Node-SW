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
    static constexpr uint32_t lengthInBytes{4};
    frameCount();
    explicit frameCount(uint32_t theFrameCount);
    frameCount& operator=(const uint32_t theFrameCount);
    frameCount& operator=(const frameCount& theFrameCount);
    bool operator==(const frameCount& theFrameCount);
    bool operator!=(const frameCount& theFrameCount);
    frameCount& operator++(int);
    void guessFromUint16(uint16_t frameCount16Lsb);
    uint32_t toUint32() const { return asUint32; };
    uint8_t& operator[](int);


#ifndef unitTesting

  private:
#endif
    static constexpr uint32_t maximumGap{256};        // maximum gap between last (valid) received framecount and next received framecount.
    union {
        uint32_t asUint32{0};
        uint8_t asUint8[lengthInBytes];
    };
};
