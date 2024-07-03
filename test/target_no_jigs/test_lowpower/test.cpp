#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <power.hpp>
#include <sx126x.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void goSleep() {
    power::goSleep();
    MX_USART1_UART_Init();        // Need to reinitialize the UART after the MCU wakes up from STOP2 mode
}

static constexpr uint32_t nmbrTestLoops{2};        // I do a few test runs to make sure the MCU can go to sleep and wake up consistently and not just once

void monitorPowerConsumptionToVerifyLowPowerMode() {
    TEST_IGNORE();
}

void test_wakeUpFromLptim1Interrupt() {
    MX_LPTIM1_Init();
    __HAL_LPTIM_LPTIM1_EXTI_ENABLE_IT();        // this is needed to get the MCU out of STOP2 on LPTIM1 interrupt
    HAL_NVIC_EnableIRQ(LPTIM1_IRQn);
    applicationEventBuffer.initialize();

    for (uint32_t testRun = 1; testRun <= nmbrTestLoops; testRun++) {
        HAL_LPTIM_SetOnce_Start_IT(&hlptim1, 0xFFFF, 4096);        // Start lptim1 timer : 4096 = 1 second @ 4096 Hz
        goSleep();
        MX_USART1_UART_Init();        // Need to reinitialize the UART after the MCU wakes up from STOP2 mode
        HAL_LPTIM_SetOnce_Stop_IT(&hlptim1);
        TEST_ASSERT_EQUAL(applicationEvent::lowPowerTimerExpired, applicationEventBuffer.pop());
    }
    HAL_NVIC_DisableIRQ(LPTIM1_IRQn);
}

void nextTestMaySleepFor60Seconds() {
    TEST_IGNORE();
}

void test_wakeUpFromRtc() {
    MX_RTC_Init();
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
    applicationEventBuffer.initialize();

    for (uint32_t testRun = 1; testRun <= nmbrTestLoops; testRun++) {
        TEST_ASSERT_TRUE_MESSAGE(applicationEventBuffer.isEmpty(), toString(applicationEventBuffer.pop()));
        goSleep();
        MX_USART1_UART_Init();        // Need to reinitialize the UART after the MCU wakes up from STOP2 mode
        TEST_ASSERT_EQUAL(applicationEvent::realTimeClockTick, applicationEventBuffer.pop());
    }
    HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);
}

void test_sleepAndWakeUpFromSx126x() {
    MX_SUBGHZ_Init();        // this will do LL_EXTI_EnableIT_32_63(LL_EXTI_LINE_44) so wakeup from Stop2 should work
    applicationEventBuffer.initialize();

    // LL_PWR_SetRadioBusyTrigger(LL_PWR_RADIO_BUSY_TRIGGER_WU_IT); ???

    sx126x::initializeRadio();
    sx126x::configForReceive(spreadingFactor::SF7, 868100000U);
    sx126x::startReceive(10U);
    goSleep();
    MX_USART1_UART_Init();        // Need to reinitialize the UART after the MCU wakes up from STOP2 mode
    TEST_ASSERT_EQUAL(applicationEvent::sx126xTimeout, applicationEventBuffer.pop());
}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(5000);
    SystemClock_Config();

    UNITY_BEGIN();
    MX_USART1_UART_Init();
    RUN_TEST(monitorPowerConsumptionToVerifyLowPowerMode);
    RUN_TEST(test_wakeUpFromLptim1Interrupt);
    RUN_TEST(nextTestMaySleepFor60Seconds);
    RUN_TEST(test_wakeUpFromRtc);
    RUN_TEST(test_sleepAndWakeUpFromSx126x);

    UNITY_END();
}
