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
#include <spi.hpp>
#include <version.hpp>
#include <screen.hpp>

extern const font roboto36bold;
extern const font tahoma24bold;
extern const font lucidaConsole12;

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {}
void tearDown(void) {}

void test_displayPresent() {
    TEST_ASSERT_TRUE(display::isPresent());
}

void test_showLogo() {
    screen::setType(screenType::logo);
    screen::update();
}

void test_showConsole() {
    screen::setType(screenType::version);
    screen::setText(0, "Hello, world!");
    screen::setText(1, "Line 2");
    screen::setText(2, "Line 3");
    screen::setText(3, "Line 4");
    screen::setText(4, "Line 5");
    screen::setText(5, "Line 6");
    screen::setText(6, "Line 7");
    screen::setText(7, "Line 8");
    screen::setText(8, "Line 9");
    screen::setText(9, "Line 10");
    screen::update();
}

void test_showUid() {
    screen::setType(screenType::uid);
    screen::update();
}

void test_showMeasurements() {
    screen::setType(screenType::measurements);
    screen::update();
}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(2000);
    SystemClock_Config();
    spi::wakeUp();

    UNITY_BEGIN();
    RUN_TEST(test_displayPresent);
    RUN_TEST(test_showLogo);
    HAL_Delay(3000);
    RUN_TEST(test_showConsole);
    HAL_Delay(3000);
    RUN_TEST(test_showUid);
    HAL_Delay(3000);
    RUN_TEST(test_showMeasurements);

    UNITY_END();
}
