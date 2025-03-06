#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <i2c.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>
#include <nvs.hpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {
}
void tearDown(void) {
}

void test_nvsPresent() {
    static constexpr uint32_t testDataLength{8};
    static constexpr uint32_t testBaseAddress{0};
    uint8_t actualData[testDataLength];
    nonVolatileStorage::read(testBaseAddress, actualData, testDataLength);

    uint8_t testDataWrite[testDataLength];
    for (uint32_t i = 0; i < testDataLength; i++) {
        testDataWrite[i] = static_cast<uint8_t>(i);
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
    HAL_Delay(3000);
    SystemClock_Config();
    i2c::wakeUp();

    UNITY_BEGIN();
    if (nonVolatileStorage::isPresent()) {
        RUN_TEST(test_nvsPresent);
    } else {
        RUN_TEST(test_nvsNotPresent);
    }
    UNITY_END();
}
