#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>
#include <sx126x.hpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

static constexpr uint32_t nmbrTestLoops{3};        // I do a few test runs to make sure the MCU can go to sleep and wake up consistently and not just once

void test_lptim1Interrupt() {
    MX_LPTIM1_Init();

    applicationEventBuffer.initialize();
    HAL_LPTIM_SetOnce_Start_IT(&hlptim1, 0xFFFF, 4096);        // Start lptim1 timer : 4096 = 1 second @ 4096 Hz

    HAL_Delay(990);
    TEST_ASSERT_TRUE_MESSAGE(applicationEventBuffer.isEmpty(), toString(applicationEventBuffer.pop()));
    HAL_Delay(20);
    TEST_ASSERT_EQUAL(applicationEvent::lowPowerTimerExpired, applicationEventBuffer.pop());

    HAL_LPTIM_SetOnce_Stop_IT(&hlptim1);
    HAL_NVIC_DisableIRQ(LPTIM1_IRQn);
}

void nextTestMayTake30Seconds() {
    TEST_IGNORE();
}

void test_rtcInterrupt() {
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

void nextTestRequiresUart1Loopback() {
    TEST_IGNORE();
}

void test_uartInterrupts() {
    MX_USART1_UART_Init();
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    static constexpr uint32_t testDataLength{16};
    uint8_t txData[testDataLength] = "Hello, World!";
    uint8_t rxData[testDataLength];
    HAL_UART_Transmit_IT(&huart1, txData, testDataLength);
    HAL_UART_Receive_IT(&huart1, rxData, testDataLength);
    HAL_Delay(10);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(txData, rxData, testDataLength);
    HAL_NVIC_DisableIRQ(USART1_IRQn);
}

void test_sx126xInterrupts() {
    MX_SUBGHZ_Init();
    applicationEventBuffer.initialize();

    //     // LL_EXTI_EnableIT_32_63(LL_EXTI_LINE_44);
    //     // LL_PWR_SetRadioBusyTrigger(LL_PWR_RADIO_BUSY_TRIGGER_WU_IT); ???
    //     HAL_NVIC_EnableIRQ(SUBGHZ_Radio_IRQn);
    //     applicationEventBuffer.initialize();

    sx126x::initializeRadio();
    sx126x::configForReceive(spreadingFactor::SF7, 868100000U);
    sx126x::startReceive(10U);
    HAL_Delay(500);        // TODO : why does this need 500 ms ? There is some startup time for the radio needed, and then 10 clocks @ 64 KHz
    TEST_ASSERT_EQUAL(applicationEvent::sx126xTimeout, applicationEventBuffer.pop());
}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(2000);
    SystemClock_Config();

    UNITY_BEGIN();
    RUN_TEST(test_lptim1Interrupt);
    RUN_TEST(nextTestRequiresUart1Loopback);
    RUN_TEST(test_uartInterrupts);
    RUN_TEST(test_sx126xInterrupts);
    RUN_TEST(nextTestMayTake30Seconds);
    RUN_TEST(test_rtcInterrupt);
    UNITY_END();
}
