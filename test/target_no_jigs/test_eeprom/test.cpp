#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <gpio.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>
#include <nvs.hpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_nvsPresent() {
    static constexpr uint32_t testDataLength{8};
    static constexpr uint32_t testBaseAddress{0};
    uint8_t actualData[testDataLength];
    nonVolatileStorage::read(testBaseAddress, actualData, testDataLength);

    uint8_t testDataWrite[testDataLength];
    for (uint32_t i = 0; i < testDataLength; i++) {
        testDataWrite[i] = i;
    }
    uint8_t testDataRead[testDataLength]{};
    nonVolatileStorage::write(testBaseAddress, testDataWrite, testDataLength);
    nonVolatileStorage::read(testBaseAddress, testDataRead, testDataLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testDataWrite, testDataRead, testDataLength);

    nonVolatileStorage::write(testBaseAddress, actualData, testDataLength);
    nonVolatileStorage::read(testBaseAddress, testDataRead, testDataLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(actualData, testDataRead, testDataLength);
}

void test_nvsNotPresent() {
    TEST_IGNORE_MESSAGE("No EEPROM detected");
}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(2000);        // required for testing framework to connect
    SystemClock_Config();

    MX_I2C2_Init();
    gpio::enableGpio(gpio::group::vddEnable);
    HAL_GPIO_WritePin(GPIOA, vddEnable_Pin, GPIO_PIN_RESET);

    gpio::enableGpio(gpio::group::i2cEeprom);

    UNITY_BEGIN();
    if (nonVolatileStorage::isPresent()) {
        RUN_TEST(test_nvsPresent);
    } else {
        RUN_TEST(test_nvsNotPresent);
    }
    UNITY_END();
}
