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

    void setFromByteArray(const uint8_t bytes[lengthInBytes]);
    void setFromWord(const uint32_t wordIn);

    uint8_t getAsByte(uint32_t index) const { return frameCountAsBytes[index]; }
    uint32_t getAsWord() const { return frameCountAsWord; }
    void increment();

    frameCount& operator++(int);
    void guessFromUint16(uint16_t frameCount16Lsb);

#ifndef unitTesting

    // private:
#endif
    static constexpr uint32_t maximumGap{256};        // maximum gap between last (valid) received framecount and next received framecount.
    // These are 2 representations of the same frameCount data, in different formats for easy retrieval and conversion. They are synced when setting the frameCount.
    uint8_t frameCountAsBytes[lengthInBytes]{};
    uint32_t frameCountAsWord{};

    void syncWordFromBytes() {
        (void)memcpy(&frameCountAsWord, frameCountAsBytes, lengthInBytes);
    }
    void syncBytesFromWord() {
        (void)memcpy(frameCountAsBytes, &frameCountAsWord, lengthInBytes);
    }
};
