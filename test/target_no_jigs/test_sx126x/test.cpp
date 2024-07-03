#include <unity.h>
#include "main.h"
#include <cube.hpp>

#include <sx126x.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_signature() {
    uint8_t isSignature[6];
    uint8_t toBeSignature[6] = {0x53, 0x58, 0x31, 0x32, 0x36, 0};        // "SX126"
    sx126x::readRegisters(sx126x::registerAddress::deviceSignature, isSignature, 5);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(toBeSignature, isSignature, 5);

    // sx126x::initializeInterface();
    //    sx126x::initializeRadio();            // writes all config registers.. should not be needed if config is retained
}

void test_readWriteTxRxBuffer() {
    uint8_t dataIn[sx126x::rxTxBufferLength];
    uint8_t dataOut[sx126x::rxTxBufferLength];
    for (uint32_t i = 0; i < sx126x::rxTxBufferLength; i++) {
        dataOut[i] = i;
        dataIn[i]  = 0;
    }
    sx126x::writeBuffer(dataOut, sx126x::rxTxBufferLength);
    sx126x::readBuffer(dataIn, sx126x::rxTxBufferLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(dataIn, dataOut, sx126x::rxTxBufferLength);
}

void test_setMode() {
    uint8_t currentMode;
    currentMode = (sx126x::getStatus() & 0b01110000) >> 4;
    TEST_ASSERT_EQUAL(2, currentMode);

    // TODO : test if we can get the radio into the other modes as well
    // first I need to know what prior config is required for this to work
}

int main(int argc, char** argv) {
    HAL_Init();
    HAL_Delay(2000);
    SystemClock_Config();

    MX_SUBGHZ_Init();
    MX_USART1_UART_Init();

    UNITY_BEGIN();
    RUN_TEST(test_signature);
    RUN_TEST(test_readWriteTxRxBuffer);
    RUN_TEST(test_setMode);
    UNITY_END();
}