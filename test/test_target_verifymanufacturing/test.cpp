#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {}
void tearDown(void) {}

void test_sleepAndWakeUp() {
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

