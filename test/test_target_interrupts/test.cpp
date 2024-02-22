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

static constexpr uint32_t nmbrTestLoops{3};        // I do a few test runs to make sure the MCU can go to sleep and wake up consistently and not just once

void test_lptim1Interrupt() {
    MX_LPTIM1_Init();

    applicationEventBuffer.initialize();
    HAL_LPTIM_SetOnce_Start_IT(&hlptim1, 0xFFFF, 4096);        // Start lptim1 timer : 4096 = 1 second @ 4096 Hz

    uint32_t now = HAL_GetTick();
    while (HAL_GetTick() - now < 2000) {
        if (!applicationEventBuffer.isEmpty()) {
            break;
        }
    }
    TEST_ASSERT_EQUAL(applicationEvent::lowPowerTimerExpired, applicationEventBuffer.pop());
    HAL_LPTIM_SetOnce_Stop_IT(&hlptim1);
    HAL_NVIC_DisableIRQ(LPTIM1_IRQn);
}



void test_rtcInterrupt___mayTakeUpTo30Seconds() {
    TEST_MESSAGE("Warning : this test can take up to 30 seconds");
    MX_RTC_Init();
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
    applicationEventBuffer.initialize();
    uint32_t now = HAL_GetTick();
    while (HAL_GetTick() - now < 31000) {
        if (!applicationEventBuffer.isEmpty()) {
            break;
        }
    }
    TEST_ASSERT_EQUAL(applicationEvent::realTimeClockTick, applicationEventBuffer.pop());
    HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);
}

// void test_uartInterrupts() {
//     TEST_MESSAGE("This test requires a loopback on the UART1 RX - TX");
//     HAL_NVIC_EnableIRQ(LPTIM1_IRQn);

//     // #define EXTI_IMR1_LPTIM1 (1UL << 29)        // TODO : investigate why this is needed to get the MCU out of STOP2 on LPTIM1 interrupt : https://gist.github.com/jefftenney/02b313fe649a14b4c75237f925872d72#file-lptimtick-c-L291
//     //     EXTI->IMR1 |= EXTI_IMR1_LPTIM1;
//     __HAL_LPTIM_LPTIM1_EXTI_ENABLE_IT();        // this is needed to get the MCU out of STOP2 on LPTIM1 interrupt

//     applicationEventBuffer.initialize();

//     for (uint32_t testRun = 1; testRun <= nmbrTestLoops; testRun++) {
//         HAL_LPTIM_SetOnce_Start_IT(&hlptim1, 0xFFFF, 128);
//         TEST_ASSERT_TRUE_MESSAGE(applicationEventBuffer.isEmpty(), toString(applicationEventBuffer.pop()));
//         goStop2();
//         TEST_ASSERT_EQUAL(applicationEvent::lowPowerTimerExpired, applicationEventBuffer.pop());
//         HAL_LPTIM_SetOnce_Stop_IT(&hlptim1);
//     }
//     __HAL_RCC_LPTIM1_CLK_SLEEP_DISABLE();        // TODO : Why ???
//     HAL_NVIC_DisableIRQ(LPTIM1_IRQn);
// }

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(2000);
    SystemClock_Config();

    UNITY_BEGIN();
    RUN_TEST(test_lptim1Interrupt);
    //RUN_TEST(test_rtcInterrupt___mayTakeUpTo30Seconds);
    // RUN_TEST(test_uartInterrupts);

    UNITY_END();
}
