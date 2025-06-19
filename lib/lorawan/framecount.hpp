// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include <cstring>


class frameCount {
  public:
    static constexpr uint32_t lengthInBytes{4};
    static constexpr uint32_t lengthInWords{1};
// old
    frameCount();
    explicit frameCount(uint32_t theFrameCount);
    frameCount& operator=(const uint32_t theFrameCount);
    frameCount& operator=(const frameCount& theFrameCount);
    //bool operator==(const frameCount& theFrameCount) const;
    bool operator!=(const frameCount& theFrameCount) const;
    bool operator>(const frameCount& theFrameCount) const;
    //frameCount& operator++(int);
    void guessFromUint16(uint16_t frameCount16Lsb);
    uint32_t toUint32() const { return asUint32; };
    uint8_t& operator[](int);

  // new
    void setFromByteArray(const uint8_t bytes[lengthInBytes]);
    void setFromWord(const uint32_t wordIn);

    uint8_t getAsByte(uint32_t index) const { return frameCountAsBytes[index]; }
    uint32_t getAsWord() const { return frameCountAsWord; }
    void increment();



#ifndef unitTesting

    // private:
#endif
    static constexpr uint32_t maximumGap{256};        // maximum gap between last (valid) received framecount and next received framecount.
    // TODO get rid of this punning via union
    union {
        uint32_t asUint32;
        uint8_t asUint8[lengthInBytes];
    };
    // These are 2 representations of the same frameCount data, in different formats for easy retrieval and conversion. They are synced when setting the frameCount.
    // Example : value 100
    // 1. asWord = 0x64
    // 2. as array of bytes : {0x64, 0x00, 0x00, 0x00}
    //    asByte(0) = 0x64, asByte(1) = 0x00, asByte(2) = 0x00, asByte(3) = 0x00

    uint8_t frameCountAsBytes[lengthInBytes]{};
    uint32_t frameCountAsWord{};

    void syncWordFromBytes() {
        (void)memcpy(&frameCountAsWord, frameCountAsBytes, lengthInBytes);
    }
    void syncBytesFromWord() {
        (void)memcpy(frameCountAsBytes, &frameCountAsWord, lengthInBytes);
    }
};
