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
#include <screen.hpp>

extern const font roboto36bold;
extern const font tahoma24bold;
extern const font lucidaConsole12;

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {}
void tearDown(void) {}

void test_screen0() {
    screen::show(screenType::measurements);
    // display::initialize();

    // version::initialize();
    // graphics::drawText(4, 180, lucidaConsole12, version::getIsVersionAsString());
    // graphics::drawText(4, 160, lucidaConsole12, "0080E11505474CAC");
    // graphics::drawText(4, 140, lucidaConsole12, "2024-04-19  16:38");

    // graphics::drawText(4, 40, tahoma24bold, "Tahoma 24 bold");
    // graphics::drawText(4, 4, roboto36bold, "012345");

    // display::update();
}

void test_screen1() {
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

    screen::show(screenType::message);
}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(2000);
    SystemClock_Config();

    MX_SPI2_Init();
    gpio::enableGpio(gpio::group::spiDisplay);

    UNITY_BEGIN();
    if (display::isPresent()) {
        RUN_TEST(test_screen0);
        HAL_Delay(5000);
        RUN_TEST(test_screen1);
    }
    UNITY_END();
}
