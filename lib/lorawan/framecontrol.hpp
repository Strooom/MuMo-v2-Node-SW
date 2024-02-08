// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

#include <linkdirection.hpp>

class frameControl {
  public:
    static constexpr uint32_t lengthInBytes{1};
    explicit frameControl();
    explicit frameControl(linkDirection theLinkDirection);
    void set(bool ADR, bool ADRACKReq, bool ACK, bool ClassB, uint8_t FOptsLen);
    void set(uint8_t theByte);
    uint8_t asByte() const;

#ifndef unitTesting

  private:
#endif
    linkDirection theLinkDirection{linkDirection::uplink};        //
    bool ADR{false};                                              // bit [7]
    bool ADRACKReq{false};                                        // bit [6]
    bool ACK{false};                                              // bit [5]
    bool ClassB{false};                                           // bit [4]
    bool FPending{false};                                         // bit [4]
    uint8_t FOptsLen{0};                                          // bits [3:0]
};
