// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>

class sensirion {
  public:
    static uint8_t crc(const uint8_t byte0, const uint8_t byte1);        // calculate the CRC of two bytes according to the Sensirion CRC-8 algorithm.
    static uint8_t generateCRCGeneric(const uint8_t* data, uint8_t count, uint8_t init, uint8_t polynomial); // code from Sensirion arduino library
    static bool checkCrc(const uint8_t* data, uint32_t dataLength);        // check all CRCs in a buffer of data. Every third byte is a CRC of the previous two bytes.
    static void insertCrc(uint8_t* data, uint32_t dataLength);             // insert CRCs in a buffer of data. Every third byte is a CRC of the previous two bytes.
    static float asFloat(const uint8_t* bytes);                       // convert a sensirion 4-byte array to a float
    static uint32_t asUint32(const uint8_t* bytes);                   // convert a sensirion 4-byte array to a uint32_t
    static uint16_t asUint16(const uint8_t* bytes);                   // convert a sensirion 2-byte array to a uint16_t
    static constexpr uint8_t crcPolynome{0x31};
    static constexpr uint8_t crcInit{0xFF};

#ifndef unitTesting

  private:
#endif
    static const uint8_t substitute[256];
};
