#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <maincontroller.hpp>
#include <lorawan.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <settingscollection.hpp>
#include <maccommand.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;
static constexpr uint32_t applicationDataLength{4};
const uint8_t applicationData[applicationDataLength]{};

void setUp(void) {
}
void tearDown(void) {}

void waitForStateOrTimeout(txRxCycleState toBeState, uint32_t timeout) {
    uint32_t start = HAL_GetTick();
    while (LoRaWAN::getState() != toBeState) {
        mainController::handleEvents();
        if (HAL_GetTick() - start > timeout) {
            break;
        }
    }
}

void test_start_send_packet() {
    LoRaWAN::sendUplink(7, applicationData, applicationDataLength);
    TEST_ASSERT_EQUAL(txRxCycleState::waitForRandomTimeBeforeTransmit, LoRaWAN::getState());
}

void test_send_packet() {
    waitForStateOrTimeout(txRxCycleState::waitForTxComplete, 2000U);
    TEST_ASSERT_EQUAL(txRxCycleState::waitForTxComplete, LoRaWAN::getState());
}

void test_wait_rx1() {
    waitForStateOrTimeout(txRxCycleState::waitForRx1Start, 2000U);
    TEST_ASSERT_EQUAL(txRxCycleState::waitForRx1Start, LoRaWAN::getState());
}

void test_rx1() {
    waitForStateOrTimeout(txRxCycleState::waitForRx1CompleteOrTimeout, 2000U);
    TEST_ASSERT_EQUAL(txRxCycleState::waitForRx1CompleteOrTimeout, LoRaWAN::getState());
}

void test_wait_rx2() {
    waitForStateOrTimeout(txRxCycleState::waitForRx2Start, 2000U);
    TEST_ASSERT_EQUAL(txRxCycleState::waitForRx2Start, LoRaWAN::getState());
}

void test_rx2() {
    waitForStateOrTimeout(txRxCycleState::waitForRx2CompleteOrTimeout, 2000U);
    TEST_ASSERT_EQUAL(txRxCycleState::waitForRx2CompleteOrTimeout, LoRaWAN::getState());
}

void test_back_to_idle() {
    waitForStateOrTimeout(txRxCycleState::idle, 2000U);
    TEST_ASSERT_EQUAL(txRxCycleState::idle, LoRaWAN::getState());
}


int main(int argc, char** argv) {
    HAL_Init();
    HAL_Delay(2000);
    SystemClock_Config();
    MX_USART1_UART_Init();
    MX_LPTIM1_Init();
    MX_SUBGHZ_Init();
    LoRaWAN::initialize();

    UNITY_BEGIN();
    RUN_TEST(test_start_send_packet);
    RUN_TEST(test_send_packet);
    RUN_TEST(test_wait_rx1);
    RUN_TEST(test_rx1);
    RUN_TEST(test_wait_rx2);
    RUN_TEST(test_rx2);
    RUN_TEST(test_back_to_idle);
    UNITY_END();
}