
#include <cstdint>
#include <circularbuffer.hpp>

  static void startTx();
    static void handleRxEvent();
    static void handleTxEvent();

    static void txEmptyInterrupt();
    static void txCompleteInterrupt();
    static void rxNotEmptyInterrupt();

    static constexpr uint32_t commandBufferLength{256};
    static constexpr uint32_t responseBufferLength{256};

    static circularBuffer<uint8_t, commandBufferLength> commandBuffer;
    static circularBuffer<uint8_t, responseBufferLength> responseBuffer;
