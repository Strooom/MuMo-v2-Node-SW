#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {}
void tearDown(void) {}

void test_waitForLptim1Interrupt() {
    applicationEventBuffer.initialize();
    HAL_LPTIM_SetOnce_Start_IT(&hlptim1, 0xFFFF, 2048);        // Start lptim1 timer : 2048 = 1 second @ 2048 Hz
    HAL_Delay(990);
    TEST_ASSERT_TRUE_MESSAGE(applicationEventBuffer.isEmpty(), toString(applicationEventBuffer.pop()));
    HAL_Delay(20);
    TEST_ASSERT_EQUAL(applicationEvent::lowPowerTimerExpired, applicationEventBuffer.pop());
    HAL_LPTIM_SetOnce_Stop_IT(&hlptim1);

    // HAL_NVIC_SetPriority(LPTIM1_IRQn, 8, 0);
    // HAL_NVIC_EnableIRQ(LPTIM1_IRQn);
    // HAL_NVIC_DisableIRQ(LPTIM1_IRQn);
}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(2000);        // required for testing framework to connect
    SystemClock_Config();
    MX_LPTIM1_Init();

    UNITY_BEGIN();
    RUN_TEST(test_waitForLptim1Interrupt);
    UNITY_END();
}
