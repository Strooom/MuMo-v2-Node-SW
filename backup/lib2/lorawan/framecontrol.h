// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

#include "linkdirection.h"

class frameControl {
  public:
    explicit frameControl();                                      // default constructor
    explicit frameControl(linkDirection theLinkDirection);        // slighly smarter constructor, creates one based on the link direction
    void set(bool ADR, bool ADRACKReq, bool ACK, bool ClassB, uint8_t FOptsLen);
    uint8_t asByte() const;                     // return the frameControl as a byte - used in uplink / transmit direction
    void fromByte(uint8_t theByte);             // decode the frameControl from a byte - used in the downlink / receive direction
    static constexpr uint32_t length{1};        // [bytes]
#ifndef unitTesting

  private:
#endif
    linkDirection theLinkDirection{linkDirection::uplink};
    bool ADR{false};              // bit [7]
    bool ADRACKReq{false};        // bit [6]
    bool ACK{false};              // bit [5]
    bool ClassB{false};           // bit [4]
    bool FPending{false};         // bit [4]
    uint8_t FOptsLen{0};          // bits [3:0]
};
