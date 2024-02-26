#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

#ifdef __cplusplus
extern "C"
{
#endif
void initialise_monitor_handles(void);
#ifdef __cplusplus
}
#endif /* extern "C" */

void setUp(void) {}
void tearDown(void) {}

void test_sleepAndWakeUp() {
}

int main(int argc, char **argv) {
    initialise_monitor_handles();
    HAL_Init();
    HAL_Delay(2000);
    SystemClock_Config();

    UNITY_BEGIN();
    RUN_TEST(test_sleepAndWakeUp);
    UNITY_END();
}


