#include <unity.h>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
// #include "lorawanevent.h"
#include <maincontroller.hpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;
// circularBuffer<loRaWanEvent, 16U> loraWanEventBuffer;

uint8_t mockSX126xDataBuffer[256];            // unitTesting mock for the LoRa Rx/Tx buffer, inside the SX126x
uint8_t mockSX126xRegisters[0x1000];          // unitTesting mock for the config registers, inside the SX126x
uint8_t mockSX126xCommandData[256][8];        // unitTesting mock for capturing the commands and their parameters to configure the SX126x
uint8_t mockBME680Registers[256]{};           // unitTesting mock for the config registers, inside the BME680
uint8_t mockTSL2591Registers[256]{};          // unitTesting mock for the config registers, inside the TSL2591

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_initialize() {
    TEST_ASSERT_EQUAL(mainState::boot, mainController::state);
    mainController::initialize();
    TEST_ASSERT_EQUAL(mainState::idle, mainController::state);
}

void test_handleEvents() {
    // Not really testing a lot yet... Need to check state transitions
    mainController::handleEvents();
    applicationEventBuffer.push(applicationEvent::usbConnected);
    mainController::handleEvents();
    applicationEventBuffer.push(applicationEvent::usbRemoved);
    mainController::handleEvents();
    applicationEventBuffer.push(applicationEvent::realTimeClockTick);
    mainController::handleEvents();
    applicationEventBuffer.push(applicationEvent::lowPowerTimerExpired);
    mainController::handleEvents();
    applicationEventBuffer.push(applicationEvent::downlinkApplicationPayloadReceived);
    mainController::handleEvents();
    applicationEventBuffer.push(applicationEvent::downlinkMacCommandReceived);
    mainController::handleEvents();
}

void test_toString() {
    // for test coverage only
    TEST_ASSERT_EQUAL_STRING("boot", toString(mainState::boot));
    TEST_ASSERT_EQUAL_STRING("idle", toString(mainState::idle));
    TEST_ASSERT_EQUAL_STRING("measuring", toString(mainState::measuring));
    TEST_ASSERT_EQUAL_STRING("logging", toString(mainState::logging));
    TEST_ASSERT_EQUAL_STRING("storing", toString(mainState::storing));
    TEST_ASSERT_EQUAL_STRING("displaying", toString(mainState::displaying));
    TEST_ASSERT_EQUAL_STRING("networking", toString(mainState::networking));
    TEST_ASSERT_EQUAL_STRING("sleeping", toString(mainState::sleeping));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_toString);
    UNITY_END();
}