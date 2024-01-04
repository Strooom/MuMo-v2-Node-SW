// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "macheader.h"

#include "frametype.h"

   macHeader::macHeader(){};

macHeader::macHeader(frameType theFrameType) : theFrameType{theFrameType} {};

void macHeader::set(frameType newFrameType) {
    theFrameType = newFrameType;
}

uint8_t macHeader::asUint8() const {
    return static_cast<uint8_t>(theFrameType) << 5;
}
