#include <unity.h>
#include "main.h"

#include <cube.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {}
void tearDown(void) {}

void test_transmit() {
}

void test_receive() {
}

void test_loopback() {

}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(2000);
    SystemClock_Config();


uart2::initialize();  // initialize UART2 peripheral

    HAL_Delay(1000);
// initialize CLI and related peripherals

    UNITY_BEGIN();
    RUN_TEST(test_transmit);
    RUN_TEST(test_receive);
    RUN_TEST(test_loopback);
    UNITY_END();
}
