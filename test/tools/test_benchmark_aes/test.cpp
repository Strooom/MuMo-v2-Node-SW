#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <aesblock.hpp>
#include <hexascii.hpp>
#include <lorawan.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <settingscollection.hpp>
#include <maccommand.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {
}
void tearDown(void) {}

void benchmark_encryptBlock() {
    uint32_t before;
    uint32_t after;
    static constexpr uint32_t nmbrOfLoops{16 * 1024};
    aesKey testKey;
    aesBlock testBlock;
    before = HAL_GetTick();
    for (uint32_t loopIndex = 0; loopIndex < nmbrOfLoops; loopIndex++) {
        testBlock.encrypt(testKey);
    }
    after = HAL_GetTick();
    TEST_ASSERT_EQUAL_UINT32(0, after - before);
}

void benchmark_encryptMessage() {
    uint32_t before;
    uint32_t after;
    static constexpr uint32_t nmbrOfLoops{2 * 1024};

    LoRaWAN::clearRawMessage();
    LoRaWAN::setOffsetsAndLengthsTx(128, 0);

    before = HAL_GetTick();
    for (uint32_t loopIndex = 0; loopIndex < nmbrOfLoops; loopIndex++) {
        LoRaWAN::encryptDecryptPayload(LoRaWAN::applicationKey, linkDirection::uplink);
    }
    after = HAL_GetTick();
    TEST_ASSERT_EQUAL_UINT32(0, after - before);
}

void benchmark_calculateMic() {
    uint32_t before;
    uint32_t after;
    static constexpr uint32_t nmbrOfLoops{2 * 1024};

    LoRaWAN::clearRawMessage();
    LoRaWAN::setOffsetsAndLengthsTx(128, 0);

    before = HAL_GetTick();
    for (uint32_t loopIndex = 0; loopIndex < nmbrOfLoops; loopIndex++) {
        LoRaWAN::calculateMic();
    }
    after = HAL_GetTick();
    TEST_ASSERT_EQUAL_UINT32(0, after - before);
}


int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(2000);        // required for testing framework to connect
    SystemClock_Config();
    UNITY_BEGIN();
    RUN_TEST(benchmark_encryptBlock);
    RUN_TEST(benchmark_encryptMessage);
    RUN_TEST(benchmark_calculateMic);
    UNITY_END();
}
