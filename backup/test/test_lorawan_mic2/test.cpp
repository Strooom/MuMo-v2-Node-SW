#include <unity.h>
#include "lorawan.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_calculateMic4() {
    LoRaWAN loraNetwork;

    uint8_t rxTestMessage[0x2B] = {0x60, 0x92, 0x3B, 0x0B, 0x26, 0x00, 0x3B, 0x00, 0x00, 0x11, 0x7C, 0xD9, 0xC4, 0xD5, 0x27, 0xA1, 0x78, 0xBD, 0x07, 0x88, 0x8B, 0x67, 0x52, 0xBC, 0xE4, 0x47, 0x26, 0xF9, 0x2B, 0x62, 0x89, 0xC3, 0x06, 0x2F, 0x69, 0x96, 0x4D, 0xD9, 0x18, 0x22, 0x58, 0x5F, 0x55};
    loraNetwork.payloadLength   = 0x2B;
    for (uint32_t index = 0; index < loraNetwork.payloadLength; index++) {
        loraNetwork.rawMessage[loraNetwork.headerOffset + index] = rxTestMessage[index];
    }
    loraNetwork.payloadLength = loraNetwork.payloadLength - (loraNetwork.headerLength + 4);        // don't count the header and the MIC
    loraNetwork.downlinkFrameCount.set(0x0000'003B);
    loraNetwork.prepareBlockB0(loraNetwork.payloadLength, linkDirection::downlink);

    TEST_ASSERT_TRUE(loraNetwork.calculateAndVerifyMic());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_calculateMic4);
    UNITY_END();
}