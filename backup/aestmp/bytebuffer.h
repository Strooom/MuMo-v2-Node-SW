// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

// In LoRaWAN data is usually binary io null terminated strings, so I use this class to easily pass data around between application, Lorawan and the LoRa modem

class byteBuffer {
  public:
    void clear();
    void set(const uint8_t* newData, uint32_t newDataLength);
    void set(const char* newData);
    void setFromHexAscii(const char* asciiKey);

    void dump();        // dump the contents to the logging output for testing purposes

#ifndef unitTesting
//  private:
#endif
    static constexpr uint32_t maxLength{256};
    uint8_t buffer[maxLength]{};
    uint32_t length{0};

    friend class LoRaWAN;
};
