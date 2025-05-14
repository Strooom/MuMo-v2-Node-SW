#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>
#include <nvs.hpp>
#include <gpio.hpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

static constexpr uint32_t testStartAddress{0};
static constexpr uint32_t cycleCount{16};
HAL_StatusTypeDef result;

void setUp(void) {}
void tearDown(void) {}

//   HAL_OK       = 0x00,
//   HAL_ERROR    = 0x01,
//   HAL_BUSY     = 0x02,
//   HAL_TIMEOUT  = 0x03

void test_hal_timeout_is_present() {
    for (uint32_t cycle = 0; cycle < cycleCount; cycle++) {
        result = HAL_I2C_IsDeviceReady(&hi2c2, static_cast<uint16_t>((nonVolatileStorage::baseI2cAddress) << 1), nonVolatileStorage::halIsPresentNmbrOfTrials, nonVolatileStorage::halIsPresentTimeoutInMs);
        TEST_ASSERT_EQUAL(HAL_OK, result);
        HAL_Delay(nonVolatileStorage::writeCycleBusyTimeInMs);
    }
}

void test_hal_timeout_single_byte() {
    uint8_t testByteToWrite{0xAA};
    nonVolatileStorage::disableWriteProtect();
    for (uint32_t cycle = 0; cycle < cycleCount; cycle++) {
        result = HAL_I2C_Mem_Write(&hi2c2, nonVolatileStorage::bankI2cAddress(testStartAddress) << 1, nonVolatileStorage::bankOffset(testStartAddress), I2C_MEMADD_SIZE_16BIT, &testByteToWrite, 1U, nonVolatileStorage::halByteWriteTimeoutInMs);
        TEST_ASSERT_EQUAL(HAL_OK, result);
        HAL_Delay(nonVolatileStorage::writeCycleBusyTimeInMs);
    }
    nonVolatileStorage::enableWriteProtect();
}

void test_hal_timout_pages() {
    static constexpr uint32_t testDataLength{nonVolatileStorage::pageSize};
    uint8_t testBytesToWrite[testDataLength]{};
    nonVolatileStorage::disableWriteProtect();

    for (uint32_t cycle = 0; cycle < cycleCount; cycle++) {
        result = HAL_I2C_Mem_Write(&hi2c2, nonVolatileStorage::bankI2cAddress(testStartAddress) << 1, nonVolatileStorage::bankOffset(testStartAddress), I2C_MEMADD_SIZE_16BIT, testBytesToWrite, static_cast<uint16_t>(testDataLength), nonVolatileStorage::halPageWriteTimeoutInMs);
        TEST_ASSERT_EQUAL(HAL_OK, result);
        HAL_Delay(nonVolatileStorage::writeCycleBusyTimeInMs);
    }
    nonVolatileStorage::enableWriteProtect();
}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(3000);
    SystemClock_Config();
    gpio::initialize();
    i2c::wakeUp();
    nonVolatileStorage::detectNmbr64KBanks();

    UNITY_BEGIN();
    RUN_TEST(test_hal_timeout_is_present);
    RUN_TEST(test_hal_timeout_single_byte);
    RUN_TEST(test_hal_timout_pages);
    UNITY_END();
}
