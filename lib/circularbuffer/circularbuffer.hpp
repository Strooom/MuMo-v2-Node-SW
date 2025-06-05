// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#ifndef generic
#include "main.h"        // required for suspending interrupts to make it thread safe
#endif

// This is a circular buffer template, so it works FIFO
// push() on an already full buffer, overwrites the oldest event.
// pop() on an empty buffer, returns 0x00

template <typename itemType, uint32_t toBeLength>
class circularBuffer {
  public:
    static constexpr uint32_t length = toBeLength;

    circularBuffer() {
        initialize();
    };

    void initialize() {
        head  = 0;
        level = 0;
    };

    void push(itemType newItem) {
#ifndef generic
        bool interrupts_enabled = (__get_PRIMASK() == 0);
        __disable_irq();
#endif
        buffer[(head + level) % length] = newItem;
        if (level < length) {
            level++;
        } else {
            head = (head + 1) % length;
        }
#ifndef generic
        if (interrupts_enabled) {
            __enable_irq();
        }
#endif
    };

    itemType pop() {
#ifndef generic
        bool interrupts_enabled = (__get_PRIMASK() == 0);
        __disable_irq();
#endif
        itemType result{};        // default should be all zeroes, which is returned when popping from empty buffer
        if (level > 0) {
            result = buffer[head];
            head   = (head + 1) % length;
            level--;
        }
#ifndef generic
        if (interrupts_enabled) {
            __enable_irq();
        }
#endif
        return result;
    };

    bool isEmpty() const {
        return level == 0;
    };

    uint32_t getLevel() const {
        return level;
    };

#ifndef unitTesting

  private:
#endif
    uint32_t head{0};
    uint32_t level{0};
    itemType buffer[length]{};
};
