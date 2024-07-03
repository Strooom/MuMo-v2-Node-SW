#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>
#include <nvs.hpp>
#include <settingscollection.hpp>
#include <lorawan.hpp>
#include <hexascii.hpp>
#include <swaplittleandbigendian.hpp>
#include <gpio.hpp>
circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_nvs_pagewrites() {
    uint8_t testData[384];
    for (uint32_t index = 0; index < 384; index++) {
        testData[index] = index % 0xFF;
    }
    nonVolatileStorage::write(nonVolatileStorage::measurementsStartAddress + 64, testData, 384);
    nonVolatileStorage::read(nonVolatileStorage::measurementsStartAddress + 64, testData, 384);
    for (uint32_t index = 0; index < 384; index++) {
        if (testData[index] != index % 0xFF) {
            TEST_FAIL();
        }
    }
}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(2000);        // required for testing framework to connect
    SystemClock_Config();

    MX_I2C2_Init();
    gpio::enableGpio(gpio::group::i2cEeprom);

    UNITY_BEGIN();
    RUN_TEST(test_nvs_pagewrites);
    UNITY_END();
}
