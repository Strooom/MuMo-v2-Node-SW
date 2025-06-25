/// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include <circularbuffer.hpp>

class uart1 {
  public:
    uart1() = delete;
    static void wakeUp();
    static void initialize();
    static bool isInitialized() { return initalized; }
    static void goSleep();
    static void startOrContinueTx();
    static void txEmpty();
    static void rxNotEmpty();

    static constexpr uint32_t responseBufferLength{2048};
    static void transmit(const char* data);
    static void transmit(const uint8_t* data);
    static void transmit(const uint8_t* data, const uint32_t dataLength);

    static constexpr uint32_t rdrNotEmpty{0b100000};              // bit 5 in USART_ISR
    static constexpr uint32_t transmitComplete{0b1000000};        // bit 6 in USART_ISR
    static constexpr uint32_t tdrEmpty{0b10000000};               // bit 7 in USART_ISR

#ifndef unitTesting

  private:
#endif
    static bool initalized;
    static circularBuffer<uint8_t, responseBufferLength> txBuffer;
#ifdef generic
    static uint8_t mockReceivedChar;
    static uint8_t mockTransmittedChar;
    static bool interruptEnabled;
#endif
};
