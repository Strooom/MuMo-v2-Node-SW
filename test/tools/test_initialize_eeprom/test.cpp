#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>
#include <gpio.hpp>
#include <lorawan.hpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {}
void tearDown(void) {}

void initializeSettings() {
    LoRaWAN::currentDataRateIndex = 5;
    LoRaWAN::saveState();
    LoRaWAN::saveChannels();

    LoRaWAN::restoreConfig();
    LoRaWAN::restoreConfig();
    LoRaWAN::restoreChannels();

    TEST_ASSERT_NOT_EQUAL_UINT32(0xFFFFFFFF, LoRaWAN::DevAddr.asUint32);
    TEST_ASSERT_EQUAL_UINT32(1, LoRaWAN::uplinkFrameCount.asUint32);
    TEST_ASSERT_EQUAL_UINT32(0, LoRaWAN::downlinkFrameCount.asUint32);
}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(2000);
    SystemClock_Config();
    MX_I2C2_Init();
    gpio::enableGpio(gpio::group::i2cEeprom);

    UNITY_BEGIN();
    RUN_TEST(initializeSettings);
    UNITY_END();
}
