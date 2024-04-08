// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include <string.h>

// template class for a circular byte buffer.

template <uint32_t bufferLength>
class byteBuffer3 {
  public:
    static constexpr uint32_t length = bufferLength;

    byteBuffer3(){};

    void initialize() {
        level     = 0;
        headIndex = 0;
    };

    // ### Get the metadata of the buffer ###
    bool isEmpty() const { return level == 0; };
    uint32_t getLevel() const { return level; };
    uint32_t getFree() const { return (length - level); };

    // #################################
    // ### READ data from the buffer ###
    // #################################

    uint8_t pop() {
        if (level > 0) {
            uint8_t result = theBuffer[headIndex];
            headIndex             = (headIndex + 1) % bufferLength;
            level--;
            return result;
        } else {
            return 0x00;
        }
    };


    // ################################
    // ### WRITE data to the buffer ###
    // ################################

    void push(uint8_t newData) {
        theBuffer[(headIndex + level) % bufferLength] = newData;
        if (level < bufferLength) {
            level++;
        } else {
            headIndex = (headIndex + 1) % bufferLength;
        }
    };

    // #################################
    // ### Internal helper functions ###
    // #################################

#ifndef unitTesting
//  private:
#endif

    //  private:
    uint32_t level{0};                        // how many bytes are in the buffer
    uint32_t headIndex{0};                    // index to head = first to read position
    uint8_t theBuffer[bufferLength]{};        // the array where the actual data is stored
};
