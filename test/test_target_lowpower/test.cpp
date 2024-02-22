#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <power.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {}
void tearDown(void) {}

static constexpr uint32_t nmbrTestLoops{3};        // I do a few test runs to make sure the MCU can go to sleep and wake up consistently and not just once

//    MX_USART2_UART_Init();

void test_sleepAndWakeUpSx126x() {
    HAL_NVIC_EnableIRQ(SUBGHZ_Radio_IRQn);
    applicationEventBuffer.initialize();

    // HAL_SUBGHZ_Init(SUBGHZ_HandleTypeDef *hsubghz) will call the 2 lines below
    // LL_EXTI_EnableIT_32_63(LL_EXTI_LINE_44);
    // LL_PWR_SetRadioBusyTrigger(LL_PWR_RADIO_BUSY_TRIGGER_WU_IT); ???

    for (uint32_t testRun = 1; testRun <= nmbrTestLoops; testRun++) {
        TEST_ASSERT_TRUE_MESSAGE(applicationEventBuffer.isEmpty(), toString(applicationEventBuffer.pop()));
        power::goSleep();
        TEST_ASSERT_EQUAL(applicationEvent::sx126xTimeout, applicationEventBuffer.pop());
    }
    HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);
}

void test_rtcInterrupt___mayTakeUpTo30Seconds() {
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
    applicationEventBuffer.initialize();

    for (uint32_t testRun = 1; testRun <= nmbrTestLoops; testRun++) {
        TEST_ASSERT_TRUE_MESSAGE(applicationEventBuffer.isEmpty(), toString(applicationEventBuffer.pop()));
        power::goSleep();
        TEST_ASSERT_EQUAL(applicationEvent::realTimeClockTick, applicationEventBuffer.pop());
    }
    HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);
}

void test_uartInterrupts() {
    HAL_NVIC_EnableIRQ(LPTIM1_IRQn);

    // #define EXTI_IMR1_LPTIM1 (1UL << 29)        // TODO : investigate why this is needed to get the MCU out of STOP2 on LPTIM1 interrupt : https://gist.github.com/jefftenney/02b313fe649a14b4c75237f925872d72#file-lptimtick-c-L291
    //     EXTI->IMR1 |= EXTI_IMR1_LPTIM1;
    __HAL_LPTIM_LPTIM1_EXTI_ENABLE_IT();        // this is needed to get the MCU out of STOP2 on LPTIM1 interrupt

    applicationEventBuffer.initialize();

    for (uint32_t testRun = 1; testRun <= nmbrTestLoops; testRun++) {
        HAL_LPTIM_SetOnce_Start_IT(&hlptim1, 0xFFFF, 128);
        TEST_ASSERT_TRUE_MESSAGE(applicationEventBuffer.isEmpty(), toString(applicationEventBuffer.pop()));
        power::goSleep();
        TEST_ASSERT_EQUAL(applicationEvent::lowPowerTimerExpired, applicationEventBuffer.pop());
        HAL_LPTIM_SetOnce_Stop_IT(&hlptim1);
    }
    __HAL_RCC_LPTIM1_CLK_SLEEP_DISABLE();        // TODO : Why ???
    HAL_NVIC_DisableIRQ(LPTIM1_IRQn);
}

void test_lptim1Interrupt() {
    HAL_NVIC_EnableIRQ(LPTIM1_IRQn);
    applicationEventBuffer.initialize();
    HAL_LPTIM_SetOnce_Start_IT(&hlptim1, 0xFFFF, 2048);        // Start lptim1 timer : 2048 = 1 second @ 2048 Hz
    HAL_Delay(990);
    TEST_ASSERT_TRUE_MESSAGE(applicationEventBuffer.isEmpty(), toString(applicationEventBuffer.pop()));
    HAL_Delay(20);
    TEST_ASSERT_EQUAL(applicationEvent::lowPowerTimerExpired, applicationEventBuffer.pop());
    HAL_LPTIM_SetOnce_Stop_IT(&hlptim1);
    HAL_NVIC_DisableIRQ(LPTIM1_IRQn);
}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(2000);
    SystemClock_Config();
    LL_DBGMCU_EnableDBGStopMode();

    MX_LPTIM1_Init();
    MX_RTC_Init();

    HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);
    HAL_NVIC_DisableIRQ(LPTIM1_IRQn);

    UNITY_BEGIN();
        TEST_MESSAGE("Note : monitor the power consumption with the Power Profiler Kit");

    RUN_TEST(test_lptim1Interrupt);
    RUN_TEST(test_uartInterrupts);
    RUN_TEST(test_rtcInterrupt___mayTakeUpTo30Seconds);
    // RUN_TEST(test_sleepAndWakeUpSx126x);

    UNITY_END();
}
