#include <unity.h>
#include "main.h"
#include <clock.cpp>
#include <errorhandler.cpp>
#include <uart2.cpp>
#include <gpio.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>

LPTIM_HandleTypeDef hlptim1;
RTC_HandleTypeDef hrtc;
SUBGHZ_HandleTypeDef hsubghz;
circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {}
void tearDown(void) {}

void test_sleepAndWakeUp() {
    // 
    TEST_IGNORE_MESSAGE("Monitor the logging in the SWO. Check current consumption on Power Profiler");
}


int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(2000);        // required for testing framework to connect
    SystemClock_Config();

    UNITY_BEGIN();
    RUN_TEST(test_sleepAndWakeUp);
    UNITY_END();
}

void SysTick_Handler(void) {
    HAL_IncTick();
}

