#include <unity.h>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <maincontroller.hpp>
#include <sensordevicecollection.hpp>
#include <battery.hpp>
#include <power.hpp>
#include <lorawan.hpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_miniAdr() {
    mainController::requestCounter = 0;
    mainController::answerCounter  = 0;
    mainController::miniAdr();
    TEST_ASSERT_EQUAL(5, LoRaWAN::currentDataRateIndex);

    mainController::requestCounter = 4;
    mainController::answerCounter  = 0;
    mainController::miniAdr();
    TEST_ASSERT_EQUAL(3, LoRaWAN::currentDataRateIndex);

    mainController::requestCounter = 6;
    mainController::answerCounter  = 1;
    mainController::miniAdr();
    TEST_ASSERT_EQUAL(3, LoRaWAN::currentDataRateIndex);

    mainController::requestCounter = 100;
    mainController::answerCounter  = 0;
    mainController::miniAdr();
    TEST_ASSERT_EQUAL(0, LoRaWAN::currentDataRateIndex);
}

void test_transitions_boot() {
    // TEST_ASSERT_EQUAL(mainState::boot, mainController::state);
    // mainController::initialize();
    // TEST_ASSERT_EQUAL(mainState::networkCheck, mainController::state);
}

void test_transitions_networkCheck() {
    mainController::state = mainState::idle;
    TEST_IGNORE_MESSAGE("implemented me!");
}

void test_transitions_main() {
    // sensorDeviceCollection::channel(static_cast<uint32_t>(sensorDeviceType::battery), battery::voltage).set(1, 1);
    // TEST_ASSERT_EQUAL(mainState::idle, mainController::state);
    // applicationEventBuffer.push(applicationEvent::realTimeClockTick);
    // mainController::handleEvents();
    // TEST_ASSERT_EQUAL(mainState::measuring, mainController::state);
    // mainController::run();
    // TEST_ASSERT_EQUAL(mainState::logging, mainController::state);
    // mainController::run();
    // TEST_ASSERT_EQUAL(mainState::networking, mainController::state);
    // TEST_ASSERT_EQUAL(txRxCycleState::waitForRandomTimeBeforeTransmit, LoRaWAN::state);
    // applicationEventBuffer.push(applicationEvent::lowPowerTimerExpired);
    // mainController::handleEvents();
    // mainController::run();
    // TEST_ASSERT_EQUAL(txRxCycleState::waitForTxComplete, LoRaWAN::state);
    // applicationEventBuffer.push(applicationEvent::sx126xTxComplete);
    // mainController::handleEvents();
    // mainController::run();
    // TEST_ASSERT_EQUAL(txRxCycleState::waitForRx1Start, LoRaWAN::state);
    // applicationEventBuffer.push(applicationEvent::lowPowerTimerExpired);
    // mainController::handleEvents();
    // mainController::run();
    // TEST_ASSERT_EQUAL(txRxCycleState::waitForRx1CompleteOrTimeout, LoRaWAN::state);
    // applicationEventBuffer.push(applicationEvent::sx126xTimeout);
    // mainController::handleEvents();
    // mainController::run();
    // TEST_ASSERT_EQUAL(txRxCycleState::waitForRx2Start, LoRaWAN::state);
    // applicationEventBuffer.push(applicationEvent::lowPowerTimerExpired);
    // mainController::handleEvents();
    // mainController::run();
    // TEST_ASSERT_EQUAL(txRxCycleState::waitForRx2CompleteOrTimeout, LoRaWAN::state);
    // applicationEventBuffer.push(applicationEvent::sx126xTimeout);
    // mainController::handleEvents();
    // mainController::run();
    // TEST_ASSERT_EQUAL(txRxCycleState::idle, LoRaWAN::state);
    // mainController::run();
    // TEST_ASSERT_EQUAL(mainState::idle, mainController::state);
}

void test_usb_detection() {
    power::mockUsbPower = false;
    mainController::run();
    mainController::handleEvents();
    power::mockUsbPower = true;
    mainController::run();
    mainController::handleEvents();
    power::mockUsbPower = false;
    mainController::run();
    mainController::handleEvents();
}

void test_toString() {
    TEST_ASSERT_EQUAL_STRING("idle", toString(mainState::idle));
    TEST_ASSERT_EQUAL_STRING("measuring", toString(mainState::measuring));
    TEST_ASSERT_EQUAL_STRING("logging", toString(mainState::logging));
    TEST_ASSERT_EQUAL_STRING("networking", toString(mainState::networking));
    TEST_ASSERT_EQUAL_STRING("boot", toString(mainState::boot));
    TEST_ASSERT_EQUAL_STRING("networkCheck", toString(mainState::networkCheck));
    TEST_ASSERT_EQUAL_STRING("fatalError", toString(mainState::fatalError));
    TEST_ASSERT_EQUAL_STRING("test", toString(mainState::test));
    TEST_ASSERT_EQUAL_STRING("unknown", toString(static_cast<mainState>(999U)));

    mainController::showLoRaWanStatus(); // Coverage only -> Use Target unit test to validate
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_transitions_boot);
    RUN_TEST(test_transitions_networkCheck);
    RUN_TEST(test_transitions_main);
    RUN_TEST(test_usb_detection);
    RUN_TEST(test_toString);
    RUN_TEST(test_miniAdr);
    UNITY_END();
}