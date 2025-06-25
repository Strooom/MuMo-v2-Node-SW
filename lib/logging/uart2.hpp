
#pragma once
#include <stdint.h>
#include <circularbuffer.hpp>
#include <clicommand.hpp>

class uart2 {
  public:
    uart2() = delete;
    static void initialize();
    static void rxNotEmpty();
    static void startOrContinueTx();
    static void txEmpty();
    static constexpr uint32_t commandBufferLength{cliCommand::maxCommandLineLength};
    static constexpr uint32_t responseBufferLength{2048};
    static constexpr uint8_t commandTerminator{'\n'};
    static uint32_t amountFreeInTxBuffer();

    static void transmit(const char* data);
    static void transmit(const uint8_t* data);
    static void transmit(const uint8_t* data, const uint32_t length);
    static void transmit(const char* data, const uint32_t length);
    static uint32_t commandCount();
    static void receive(char* data);

    static constexpr uint32_t rdrNotEmpty{0b100000};              // bit 5 in USART_ISR
    static constexpr uint32_t transmitComplete{0b1000000};        // bit 6 in USART_ISR
    static constexpr uint32_t tdrEmpty{0b10000000};               // bit 7 in USART_ISR

#ifndef unitTesting

  private:
#endif

    static bool initalized;
    static circularBuffer<uint8_t, commandBufferLength> rxBuffer;
    static circularBuffer<uint8_t, responseBufferLength> txBuffer;
    static uint32_t commandCounter;
#ifdef generic
    static uint8_t mockReceivedChar;
    static uint8_t mockTransmittedChar;
    static bool interruptEnabled;
#endif
    friend class cli;
};
