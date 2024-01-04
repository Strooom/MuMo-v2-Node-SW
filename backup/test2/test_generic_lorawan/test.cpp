#include <unity.h>
#include "lorawan.h"
#include "circularbuffer.h"
#include "lorawanevent.h"
#include "applicationevent.h"

circularBuffer<applicationEvent, 16U> applicationEventBuffer;
circularBuffer<loRaWanEvent, 16U> loraWanEventBuffer;

uint8_t mockSX126xDataBuffer[256];            // unitTesting mock for the LoRa Rx/Tx buffer, inside the SX126x
uint8_t mockSX126xRegisters[0x1000];          // unitTesting mock for the config registers, inside the SX126x
uint8_t mockSX126xCommandData[256][8];        // unitTesting mock for capturing the commands and their parameters to configure the SX126x


void setUp(void) {}
void tearDown(void) {}

void test_initialize() {
    // need to set the mock nonvolatile memory
    // initilize will read from there and restore the correct context
    LoRaWAN::initialize();
    // Assert all individual settings are correct
}

void test_setOffsetsAndLengthsRx() {
    // set a test-vector received message and let it parse
    static constexpr uint32_t loRaPayloadLength{32};
    uint8_t receivedBytes[loRaPayloadLength];
    LoRaWAN::setOffsetsAndLengthsRx(loRaPayloadLength);
    // now check decoding
}

void test_setOffsetsAndLengthsTx() {
    LoRaWAN::setOffsetsAndLengthsTx(24, 5);
    // static uint32_t macPayloadLength;          //
    // static uint32_t framePortLength;           // total length of FPort in [bytes] 1 if present, 0 if not present
    // static uint32_t frameOptionsLength;        // this is not static, it can vary between 0 and 15
    // static uint32_t frameHeaderLength;         // this is not static, it can vary between 7 and 22
    // static uint32_t framePayloadLength;        // length of the application (or MAC layer) payload. excludes header, port and mic
    // static uint32_t loRaPayloadLength;         // length of the data in the radio Tx/Rx buffer
    // static uint32_t framePortOffset;           //
    // static uint32_t framePayloadOffset;        //
    // static uint32_t micOffset;                 //
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_setOffsetsAndLengthsRx);
    RUN_TEST(test_setOffsetsAndLengthsTx);
    UNITY_END();
}