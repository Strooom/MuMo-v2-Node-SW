// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include <cstring>

// template class for a byte buffer.
// In LoRaWAN data is usually binary io null terminated strings, so I use this class to easily pass data around between application, Lorawan and the LoRa modem

template <uint32_t toBeLength>
class linearBuffer {
  public:
    static constexpr uint32_t length = toBeLength;

    linearBuffer() = default;
    void initialize() { level = 0; };

    // ### Get the metadata of the buffer ###
    bool isEmpty() const { return level == 0; };
    uint32_t getLevel() const { return level; };
    uint32_t getFree() const { return (length - level); };

    // #################################
    // ### READ data from the buffer ###
    // #################################

    const uint8_t* asUint8Ptr(uint32_t offset = 0) const { return (buffer + offset); }        // WARNING : this accepts offsets pointing beyond the meaningful data, even beyond the whole buffer..

    uint8_t operator[](size_t index) {
        if (index < level) {
            return buffer[index];
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
        if ((level + 1) <= length) {
            buffer[level] = newData;
            level++;
        }
    }

    // Append an array of bytes to the buffer
    void append(const uint8_t* newData, uint32_t newDataLength) {
        if ((level + newDataLength) <= length) {
            (void)memcpy(buffer + level, newData, newDataLength);
            level += newDataLength;
        } else {
            (void)memcpy(buffer + level, newData, length - level);
            level = length;
        }
    }

    // Prefixing = adding data to the BEGINNING of the buffer (and thus shifting the existing data to the right)
    // Prefix a single byte to the buffer
    void prefix(const uint8_t newData) {
        if ((level + 1) <= length) {
            shiftRight(1U);
            buffer[0] = newData;
        }
    }

    // Prefix an array of bytes to the buffer
    void prefix(const uint8_t* newData, uint32_t newDataLength) {
        if ((level + newDataLength) <= length) {
            (void)memmove(buffer + newDataLength, buffer, level);
            (void)memcpy(buffer, newData, newDataLength);
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
            (void)memmove(buffer, buffer + shiftAmount, level - shiftAmount);
            level -= shiftAmount;
        } else {
            level = 0;
        }
    }

    void shiftRight(uint32_t shiftAmount) {
        if ((level + shiftAmount) <= length) {
            (void)memmove(buffer + shiftAmount, buffer, level);
            level += shiftAmount;
        } else {
            level = length;
        }
    }

    void set(const uint8_t* newData, uint32_t newDataLength, uint32_t offset = 0) {
        if (offset + newDataLength <= length) {
            (void)memcpy(buffer + offset, newData, newDataLength);
        }
    }

#ifndef unitTesting

  private:
#endif
    uint32_t level{0};               // how many bytes are in the buffer
    uint8_t buffer[length]{};        // the array where the actual data is stored
};
