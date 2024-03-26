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

void test_start_send_packet() {
    LoRaWAN::sendUplink(7, applicationData, applicationDataLength);
    TEST_ASSERT_EQUAL(txRxCycleState::waitForRandomTimeBeforeTransmit, LoRaWAN::getState());
}

void test_send_packet() {
    while (true) {
        mainController::handleEvents();

    }

    TEST_ASSERT_EQUAL(txRxCycleState::waitForRandomTimeBeforeTransmit, LoRaWAN::getState());
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

    UNITY_END();
}