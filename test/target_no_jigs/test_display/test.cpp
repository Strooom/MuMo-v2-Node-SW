#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>
#include <display.hpp>
#include <graphics.hpp>
#include <screen.hpp>
#include <gpio.hpp>
#include <spi.hpp>
#include <stdio.h>


extern const font roboto36bold;
extern const font tahoma24bold;
extern const font lucidaConsole12;
extern const bitmap usbIcon;
extern const bitmap muMoLogo;

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

static constexpr bool full{true};
static constexpr bool fast{false};
static constexpr uint32_t testResultLength{256U};
static char testResult[testResultLength]{};
static uint32_t testStartTime;
static uint32_t testEndTime;

void setUp(void) {}
void tearDown(void) {}

void test_displayPresent() {
    TEST_ASSERT_TRUE(display::isPresent());
    TEST_ASSERT_EQUAL(0, display::refreshCounter);
}

void test_erase() {
    display::clearAllPixels();
    testStartTime = HAL_GetTick();
    display::update(true);
    testEndTime = HAL_GetTick();
    snprintf(testResult, testResultLength, "full update %lu ms", testEndTime - testStartTime);
    HAL_Delay(1000U);
    TEST_IGNORE_MESSAGE(testResult);
}

void test_updateFull() {
    screen::showLogo();
    testStartTime = HAL_GetTick();
    display::update(true);
    testEndTime = HAL_GetTick();
    snprintf(testResult, testResultLength, "full update %lu ms", testEndTime - testStartTime);
    HAL_Delay(1000U);
    TEST_IGNORE_MESSAGE(testResult);
}

void test_eraseFast() {
    display::clearAllPixels();
    testStartTime = HAL_GetTick();
    display::update(false);
    testEndTime = HAL_GetTick();
    snprintf(testResult, testResultLength, "partial update %lu ms", testEndTime - testStartTime);
    HAL_Delay(1000U);
    TEST_IGNORE_MESSAGE(testResult);
}

void test_updateFast() {
    screen::showLogo();
    testStartTime = HAL_GetTick();
    display::update(false);
    testEndTime = HAL_GetTick();
    snprintf(testResult, testResultLength, "partial update %lu ms", testEndTime - testStartTime);
    HAL_Delay(1000U);
    TEST_IGNORE_MESSAGE(testResult);
}

int main(int argc, char **argv) {
    HAL_Init();
    SystemClock_Config();

    gpio::initialize();
    uart1::initialize();
    spi::wakeUp();

    HAL_Delay(1000U); // It seems some delay is needed to get all test results properly sent to the PC

    UNITY_BEGIN();
    RUN_TEST(test_displayPresent);
    RUN_TEST(test_erase);
    RUN_TEST(test_updateFull);
    RUN_TEST(test_eraseFast);
    RUN_TEST(test_updateFast);
    UNITY_END();
}
