// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include <cstring>
#include <hexascii.hpp>

class deviceAddress {
  public:
    static constexpr uint32_t lengthInBytes{4};
    static constexpr uint32_t lengthAsHexAscii{8};

    void setFromByteArray(const uint8_t bytes[lengthInBytes]);
    void setFromWord(const uint32_t wordIn);
    void setFromHexString(const char* string);

    uint8_t getAsByte(uint32_t index) const { return devAddrAsBytes[index]; }
    uint32_t getAsWord() const { return devAddrAsWord; }
    const char* getAsHexString() const { return devAddrAsHexString; }

  private:
    // These are 3 representations of the same address data, in different formats for easy retrieval and conversion. They are synced when setting the address.
    // Example : "26 0B 17 23" on The Things Network
    // 1. asHexString : "260B1723"
    // 2. as array of bytes : {0x26, 0x0B, 0x17, 0x23}
    //    asByte(0) = 0x26, asByte(1) = 0x0B, asByte(2) = 0x17, asByte(3) = 0x23
    // 3. asWord = 0x23170B26

    uint8_t devAddrAsBytes[lengthInBytes]{};
    uint32_t devAddrAsWord{};
    char devAddrAsHexString[lengthAsHexAscii + 1]{};

    void syncWordFromBytes() {
        (void)memcpy(&devAddrAsWord, devAddrAsBytes, lengthInBytes);
    }
    void syncBytesFromWord() {
        (void)memcpy(devAddrAsBytes, &devAddrAsWord, lengthInBytes);
    }
    void syncHexStringFromBytes() {
        hexAscii::byteArrayToHexStringReversed(devAddrAsHexString, devAddrAsBytes, lengthInBytes);
    }
};
