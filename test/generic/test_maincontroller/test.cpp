#include <unity.h>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <maincontroller.hpp>
#include <sensordevicecollection.hpp>
#include <battery.hpp>
#include <power.hpp>
#include <lorawan.hpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

extern bool mockUsbPower;

void setUp(void) {}
void tearDown(void) {}

void test_transitions_boot() {
    TEST_ASSERT_EQUAL(mainState::boot, mainController::state);
    mainController::handleEvents();
    mainController::run();
    TEST_ASSERT_EQUAL(mainState::boot, mainController::state);

    mainController::initialize();
    TEST_ASSERT_EQUAL(mainState::waitForBootScreen, mainController::state);
    applicationEventBuffer.push(applicationEvent::realTimeClockTick);
    mainController::handleEvents();
    TEST_ASSERT_EQUAL(mainState::waitForNetworkResponse, mainController::state);
    applicationEventBuffer.push(applicationEvent::realTimeClockTick);
    mainController::handleEvents();
    applicationEventBuffer.push(applicationEvent::downlinkMacCommandReceived);
    mainController::handleEvents();
    TEST_ASSERT_EQUAL(mainState::idle, mainController::state);
    mainController::run();
    TEST_ASSERT_EQUAL(mainState::idle, mainController::state);
    mainController::run();
    TEST_ASSERT_EQUAL(mainState::idle, mainController::state);
}

void test_transitions_tick() {
    sensorDeviceCollection::channel(static_cast<uint32_t>(sensorDeviceType::battery), static_cast<uint32_t>(battery::voltage)).set(1, 1, 1, 1, 3.2F);
    TEST_ASSERT_EQUAL(mainState::idle, mainController::state);
    applicationEventBuffer.push(applicationEvent::realTimeClockTick);
    mainController::handleEvents();
    TEST_ASSERT_EQUAL(mainState::measuring, mainController::state);
    mainController::run();
    TEST_ASSERT_EQUAL(mainState::logging, mainController::state);
    mainController::run();
    TEST_ASSERT_EQUAL(mainState::networking, mainController::state);
    TEST_ASSERT_EQUAL(txRxCycleState::waitForRandomTimeBeforeTransmit, LoRaWAN::state);
    applicationEventBuffer.push(applicationEvent::lowPowerTimerExpired);
    mainController::handleEvents();
    mainController::run();
    TEST_ASSERT_EQUAL(txRxCycleState::waitForTxComplete, LoRaWAN::state);
    applicationEventBuffer.push(applicationEvent::sx126xTxComplete);
    mainController::handleEvents();
    mainController::run();
    TEST_ASSERT_EQUAL(txRxCycleState::waitForRx1Start, LoRaWAN::state);
    applicationEventBuffer.push(applicationEvent::lowPowerTimerExpired);
    mainController::handleEvents();
    mainController::run();
    TEST_ASSERT_EQUAL(txRxCycleState::waitForRx1CompleteOrTimeout, LoRaWAN::state);
    applicationEventBuffer.push(applicationEvent::sx126xTimeout);
    mainController::handleEvents();
    mainController::run();
    TEST_ASSERT_EQUAL(txRxCycleState::waitForRx2Start, LoRaWAN::state);
    applicationEventBuffer.push(applicationEvent::lowPowerTimerExpired);
    mainController::handleEvents();
    mainController::run();
    TEST_ASSERT_EQUAL(txRxCycleState::waitForRx2CompleteOrTimeout, LoRaWAN::state);
    applicationEventBuffer.push(applicationEvent::sx126xTimeout);
    mainController::handleEvents();
    mainController::run();
    TEST_ASSERT_EQUAL(txRxCycleState::idle, LoRaWAN::state);
    mainController::run();
    TEST_ASSERT_EQUAL(mainState::idle, mainController::state);
}

void test_usb_detection() {
    mockUsbPower = false;
    mainController::run();
    mainController::handleEvents();
    mockUsbPower = true;
    mainController::run();
    mainController::handleEvents();
    mockUsbPower = false;
    mainController::run();
    mainController::handleEvents();
}

void test_toString() {
    // for test coverage only
    TEST_ASSERT_EQUAL_STRING("boot", toString(mainState::boot));
    TEST_ASSERT_EQUAL_STRING("idle", toString(mainState::idle));
    TEST_ASSERT_EQUAL_STRING("measuring", toString(mainState::measuring));
    TEST_ASSERT_EQUAL_STRING("logging", toString(mainState::logging));
    TEST_ASSERT_EQUAL_STRING("displaying", toString(mainState::displaying));
    TEST_ASSERT_EQUAL_STRING("networking", toString(mainState::networking));
    TEST_ASSERT_EQUAL_STRING("sleeping", toString(mainState::sleeping));
    TEST_ASSERT_EQUAL_STRING("waitForNetworkRequest", toString(mainState::waitForNetworkRequest));
    TEST_ASSERT_EQUAL_STRING("waitForNetworkResponse", toString(mainState::waitForNetworkResponse));
    TEST_ASSERT_EQUAL_STRING("waitForBootScreen", toString(mainState::waitForBootScreen));
    TEST_ASSERT_EQUAL_STRING("test", toString(mainState::test));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_transitions_boot);
    RUN_TEST(test_transitions_tick);
    RUN_TEST(test_usb_detection);
    RUN_TEST(test_toString);
    UNITY_END();
}