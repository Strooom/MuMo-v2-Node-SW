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

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {}
void tearDown(void) {}

void test_displayPresent() {
    display::initialize();
    display::clearAllPixels();
    graphics::drawFilledRectangle(0, 0, 99, 99, graphics::color::black);
    display::update();
    HAL_Delay(2000);
    display::initialize();
    display::clearAllPixels();
    graphics::drawFilledRectangle(0, 99, 99, 199, graphics::color::black);
    display::update();
    HAL_Delay(2000);
    display::initialize();
    display::clearAllPixels();
    graphics::drawFilledRectangle(99, 99, 199, 199, graphics::color::black);
    display::update();
    HAL_Delay(2000);
    display::initialize();
    display::clearAllPixels();
    graphics::drawFilledRectangle(99, 0, 199, 99, graphics::color::black);
    display::update();
}

void test_displayNotPresent() {
    TEST_IGNORE_MESSAGE("No display detected");
}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(2000);        // required for testing framework to connect
    SystemClock_Config();

    MX_SPI2_Init();
    gpio::enableGpio(gpio::group::spiDisplay);

    UNITY_BEGIN();
    if (display::isPresent()) {
        RUN_TEST(test_displayPresent);
    } else {
        RUN_TEST(test_displayNotPresent);
    }
    UNITY_END();
}
