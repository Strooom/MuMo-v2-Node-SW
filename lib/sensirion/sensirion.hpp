// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>

class sensirion {
  public:
    static uint8_t crc(const uint8_t byte0, const uint8_t byte1);        // calculate the CRC of two bytes according to the Sensirion CRC-8 algorithm.
    static bool checkCrc(const uint8_t* data, uint32_t count);           // check all CRCs in a buffer of data. Every third byte is a CRC of the previous two bytes.
    static void insertCrc(uint8_t* data, uint32_t count);                // insert CRCs in a buffer of data. Every third byte is a CRC of the previous two bytes.

    static constexpr uint8_t crcPolynome{0x31};
    static constexpr uint8_t crcInit{0xFF};

#ifndef unitTesting

  private:
#endif
    static void div(uint8_t& aByte);
};
