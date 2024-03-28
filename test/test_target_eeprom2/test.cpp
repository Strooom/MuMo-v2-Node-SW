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

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {}
void tearDown(void) {}

void test_nvs() {
    LoRaWAN::restoreConfig();

    //TEST_ASSERT_EQUAL(0x05, LoRaWAN::rx1DelayInSeconds);
    TEST_ASSERT_EQUAL(0x1BC70B26, LoRaWAN::DevAddr.asUint32);
    char tmpKeyAsHexString[33];
    hexAscii::byteArrayToHexString(tmpKeyAsHexString, LoRaWAN::applicationKey.asBytes(), 16);
    TEST_ASSERT_EQUAL_STRING("ECF61A5B18BFBF81EF4FA7DBA764CE8B", tmpKeyAsHexString);
    hexAscii::byteArrayToHexString(tmpKeyAsHexString, LoRaWAN::networkKey.asBytes(), 16);
    TEST_ASSERT_EQUAL_STRING("34CE07A8DDE81F4C29A0AED7B4F1D7BB", tmpKeyAsHexString);
}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(2000);        // required for testing framework to connect
    SystemClock_Config();

    MX_I2C2_Init();
    gpio::enableGpio(gpio::group::i2cEeprom);

    UNITY_BEGIN();
    RUN_TEST(test_nvs);
    UNITY_END();
}
