// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include <circularbuffer.hpp>

class cli {
  public:
    static void startTx();
    static void handleRxEvent();
    static void handleTxEvent();

    static void txEmptyInterrupt();
    static void txCompleteInterrupt();
    static void rxNotEmptyInterrupt();

  private:
    static void handleEvents();
    static void jumpToBootLoader();

    static constexpr uint32_t commandBufferLength{256};
    static constexpr uint32_t responseBufferLength{256};

    static circularBuffer<uint8_t, commandBufferLength> commandBuffer;
    static circularBuffer<uint8_t, responseBufferLength> responseBuffer;

    static constexpr uint8_t bootLoaderMagicValue{0x7F};
};
