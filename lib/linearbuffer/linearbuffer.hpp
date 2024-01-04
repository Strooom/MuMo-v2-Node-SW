// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include <string.h>

// template class for a byte buffer.
// In LoRaWAN data is usually binary io null terminated strings, so I use this class to easily pass data around between application, Lorawan and the LoRa modem

template <uint32_t bufferLength>
class linearBuffer {
  public:
    static constexpr uint32_t length = bufferLength;

    linearBuffer(){};
    void initialize() { level = 0; };

    // ### Get the metadata of the buffer ###
    bool isEmpty() const { return level == 0; };
    uint32_t getLevel() const { return level; };
    uint32_t getFree() const { return (length - level); };

    // #################################
    // ### READ data from the buffer ###
    // #################################

    const uint8_t* asUint8Ptr(uint32_t offset = 0) const { return (theBuffer + offset); }        // WARNING : this accepts offsets pointing beyond the meaningful data, even beyond the whole buffer..

    const uint8_t operator[](size_t index) {
        if (index < level) {
            return theBuffer[index];
        } else {
            return {0};
        }
    }

    void consume(uint32_t numberOfBytes) {
        shiftLeft(numberOfBytes);
    }

    void truncate(uint32_t numberOfBytes) {
        if (numberOfBytes < level) {
            level -= numberOfBytes;
        } else {
            level = 0;
        }
    }

    // ################################
    // ### WRITE data to the buffer ###
    // ################################

    // Appending = adding data to the END of the buffer
    // Append a single byte to the buffer
    void append(const uint8_t newData) {
        if ((level + 1) <= bufferLength) {
            theBuffer[level] = newData;
            level++;
        }
    }

    // Append an array of bytes to the buffer
    void append(const uint8_t* newData, uint32_t newDataLength) {
        if ((level + newDataLength) <= bufferLength) {
            (void)memcpy(theBuffer + level, newData, newDataLength);
            level += newDataLength;
        } else {
            (void)memcpy(theBuffer + level, newData, bufferLength - level);
            level = bufferLength;
        }
    }

    // Prefixing = adding data to the BEGINNING of the buffer (and thus shifting the existing data to the right)
    // Prefix a single byte to the buffer
    void prefix(const uint8_t newData) {
        if ((level + 1) <= bufferLength) {
            shiftRight(1U);
            theBuffer[0] = newData;
        }
    }

    // Prefix an array of bytes to the buffer
    void prefix(const uint8_t* newData, uint32_t newDataLength) {
        if ((level + newDataLength) <= bufferLength) {
            (void)memcpy(theBuffer + newDataLength, theBuffer, level);
            (void)memcpy(theBuffer, newData, newDataLength);
            level += newDataLength;
        }
    }

    // #################################
    // ### Internal helper functions ###
    // #################################

#ifndef unitTesting

  private:
#endif
    void shiftLeft(uint32_t shiftAmount) {
        if (shiftAmount < level) {
            (void)memcpy(theBuffer, theBuffer + shiftAmount, level - shiftAmount);
            level -= shiftAmount;
        } else {
            level = 0;
        }
    }

    void shiftRight(uint32_t shiftAmount) {
        if ((level + shiftAmount) <= bufferLength) {
            (void)memcpy(theBuffer + shiftAmount, theBuffer, level);
            level += shiftAmount;
        } else {
            level = bufferLength;
        }
    }

    void set(const uint8_t* newData, uint32_t newDataLength, uint32_t offset = 0) {
        if (offset + newDataLength <= bufferLength) {
            (void)memcpy(theBuffer + offset, newData, newDataLength);
        }
    }

#ifndef unitTesting

  private:
#endif
    uint32_t level{0};                        // how many bytes are in the buffer
    uint8_t theBuffer[bufferLength]{};        // the array where the actual data is stored
};
