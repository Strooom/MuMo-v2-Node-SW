#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>
#include <display.hpp>
#include <graphics.hpp>
#include <gpio.hpp>
#include <version.hpp>

#include <realtimeclock.hpp>

extern font roboto36bold;
extern font tahoma24bold;
extern font lucidaConsole12;

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {}
void tearDown(void) {}

// UTC	Feb 12, 2024	20:43:08
// Byte Data = 6E 45 F5 52
// GPS = 1391805806
// unix = 1707770588

void test_unixTimeFromGpsTime() {
    TEST_ASSERT_EQUAL(1707770588, static_cast<uint32_t>(realTimeClock::unixTimeFromGpsTime(1391805806)));
}

void test_setRtc() {
    time_t unixTimeIn = 1707770588;
    realTimeClock::set(unixTimeIn);
    time_t unixTimeOut = realTimeClock::get();
    TEST_ASSERT_EQUAL(unixTimeOut, unixTimeIn);

    // TODO : make the test and code more robust by also checking different months and edge cases like years < 2000, ...
}

void test_rtcToDisplay() {
    display::initialize();
    time_t unixTime = realTimeClock::unixTimeFromGpsTime(1397998142U);
    realTimeClock::set(unixTime);

    for (uint32_t loop = 0; loop < 10; loop++) {
        unixTime = realTimeClock::get();
        struct tm *ptm = gmtime(&unixTime);

        display::clearAllPixels();
        graphics::drawText(4, 180, lucidaConsole12, asctime(ptm));
        display::update();
        HAL_Delay(5000);
    }
}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(2000);
    SystemClock_Config();
    MX_RTC_Init();
    MX_SPI2_Init();
    gpio::enableGpio(gpio::group::spiDisplay);

    UNITY_BEGIN();
    RUN_TEST(test_unixTimeFromGpsTime);
    RUN_TEST(test_setRtc);
    RUN_TEST(test_rtcToDisplay);
    UNITY_END();
}
