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

extern font roboto36bold;
extern font tahoma24bold;
extern font lucidaConsole12;
extern bitmap usbIcon;

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {}
void tearDown(void) {}

void test_displayFonts() {
    display::clearAllPixels();
    graphics::drawText(4, 180, lucidaConsole12, version::getIsVersionAsString());
    graphics::drawText(4, 160, lucidaConsole12, "0080E11505474CAC");
    graphics::drawText(4, 140, lucidaConsole12, "2024-04-19  16:38");
    graphics::drawText(4, 40, tahoma24bold, "Tahoma 24 bold");
    graphics::drawText(4, 4, roboto36bold, "012345");
    display::update();
}

void test_displayBitmap() {
    display::clearAllPixels();
    graphics::drawBitMap(0, 0, usbIcon);
    display::update();
}

void test_displayNotPresent() {
    TEST_IGNORE_MESSAGE("No display detected");
}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(2000);
    SystemClock_Config();

    MX_SPI2_Init();
    gpio::enableGpio(gpio::group::spiDisplay);

    UNITY_BEGIN();
    if (display::isPresent()) {
        RUN_TEST(test_displayBitmap);
        // RUN_TEST(test_displayFonts);
    } else {
        RUN_TEST(test_displayNotPresent);
    }
    UNITY_END();
}
