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

extern const font roboto36bold;
extern const font tahoma24bold;
extern const font lucidaConsole12;

circularBuffer<applicationEvent, 16U> applicationEventBuffer;
time_t testUnixTime  = 1707770588;
uint32_t testGpsTime = 1391805806;
// UTC	Feb 12, 2024	20:43:08
// Byte Data = 6E 45 F5 52
// GPS = 1391805806
// unix = 1707770588

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_unixTimeFromGpsTime() {
    TEST_ASSERT_EQUAL(testUnixTime, static_cast<uint32_t>(realTimeClock::unixTimeFromGpsTime(testGpsTime)));
}

void test_setRtc() {
    realTimeClock::set(testUnixTime);
    time_t unixTimeOut = realTimeClock::get();
    TEST_ASSERT_EQUAL(unixTimeOut, testUnixTime);
}

void test_rtcIsRunning() {
    HAL_Delay(3000);
    time_t unixTimeExpected = testUnixTime + 3;
    time_t unixTimeOut      = realTimeClock::get();
    TEST_ASSERT_EQUAL(unixTimeExpected, unixTimeOut);
}

void test_time_t() {
    TEST_ASSERT_EQUAL(8, sizeof(time_t));
    uint32_t testTimeUint32 = static_cast<uint32_t>(testUnixTime);
    TEST_ASSERT_EQUAL(1707770588, testTimeUint32);


    time_t testTimeBeyond2038 = 2537551506;
    uint32_t testTimeUint32b = static_cast<uint32_t>(testTimeBeyond2038);
    TEST_ASSERT_EQUAL(2537551506, testTimeUint32b);

}

void test_rtcToDisplay() {
    display::initialize();
    time_t unixTime = realTimeClock::unixTimeFromGpsTime(1397998142U);
    realTimeClock::set(unixTime);

    for (uint32_t loop = 0; loop < 2; loop++) {
        unixTime       = realTimeClock::get();
        struct tm *ptm = gmtime(&unixTime);

        display::clearAllPixels();
        graphics::drawText(4, 180, lucidaConsole12, asctime(ptm));
        display::update();
        HAL_Delay(10000);
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
    RUN_TEST(test_rtcIsRunning);
    RUN_TEST(test_time_t);
    RUN_TEST(test_rtcToDisplay);
    UNITY_END();
}
