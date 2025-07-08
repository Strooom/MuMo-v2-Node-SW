#include <unity.h>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <maincontroller.hpp>
#include <sensordevicecollection.hpp>
#include <battery.hpp>
#include <power.hpp>
#include <lorawan.hpp>
#include <uart2.hpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {}
void tearDown(void) {}

void test_initialize() {
    TEST_ASSERT_EQUAL(mainState::boot, mainController::state);
    TEST_ASSERT_EQUAL(0, mainController::requestCounter);
    TEST_ASSERT_EQUAL(0, mainController::answerCounter);
    // mainController::initialize();
    TEST_IGNORE_MESSAGE("implement me!");
}

#pragma region stateMachine

#pragma endregion
#pragma region CLI
void test_setDeviceAddress() {
    cliCommand testCommand;
    // set command properties
    // LoRaWAN::DevAddr.setFromHexString("12345678");
    mainController::setDeviceAddress(testCommand);
    TEST_IGNORE_MESSAGE("implement me!");
}

void test_setKeys() {
    cliCommand testCommand;
    // set command properties
    // mainController::setName(testCommand);
    TEST_IGNORE_MESSAGE("implement me!");
}

void test_setName() {
    cliCommand testCommand;
    // set command properties
    // mainController::setName(testCommand);
    TEST_IGNORE_MESSAGE("implement me!");
}

void test_setRadio() {
    cliCommand testCommand;
    // set command properties
    // mainController::setRadio(testCommand);
    TEST_IGNORE_MESSAGE("implement me!");
}

void test_setBattery() {
    cliCommand testCommand;
    // set command properties
    // mainController::setBattery(testCommand);
    TEST_IGNORE_MESSAGE("implement me!");
}

void test_setSensor() {
    cliCommand testCommand;
    // set command properties
    // mainController::setSensor(testCommand);
    TEST_IGNORE_MESSAGE("implement me!");
}
#pragma endregion
#pragma region other
void test_toString() {
    TEST_ASSERT_EQUAL_STRING("idle", toString(mainState::idle));
    TEST_ASSERT_EQUAL_STRING("sampling", toString(mainState::sampling));
    TEST_ASSERT_EQUAL_STRING("networking", toString(mainState::networking));
    TEST_ASSERT_EQUAL_STRING("boot", toString(mainState::boot));
    TEST_ASSERT_EQUAL_STRING("networkCheck", toString(mainState::networkCheck));
    TEST_ASSERT_EQUAL_STRING("fatalError", toString(mainState::fatalError));
    TEST_ASSERT_EQUAL_STRING("test", toString(mainState::test));
    TEST_ASSERT_EQUAL_STRING("unknown", toString(static_cast<mainState>(999U)));
}

void test_forCoverageOnly() {
    mainController::showHelp();
    uart2::initialize();        // clears output buffer, could get full otherwise, stalling the tests
    mainController::showPrompt();
    uart2::initialize();
    mainController::showDeviceStatus();
    uart2::initialize();
    mainController::showNetworkStatus();
    uart2::initialize();
    mainController::showMeasurementsStatus();
    uart2::initialize();
    TEST_IGNORE_MESSAGE("for coverage only");
}
#pragma endregion

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
    TEST_ASSERT_EQUAL(mainState::boot, mainController::state);
    mainController::initialize();
    TEST_ASSERT_EQUAL(mainState::networkCheck, mainController::state);
}

void test_transitions_networkCheck() {
    mainController::state = mainState::idle;
    TEST_IGNORE_MESSAGE("implemented me!");
}

void test_usb_detection() {
    power::mockUsbPower = false;
    mainController::runUsbPowerDetection();
    mainController::handleEvents();
    power::mockUsbPower = true;
    mainController::runUsbPowerDetection();
    mainController::handleEvents();
    power::mockUsbPower = false;
    mainController::runUsbPowerDetection();
    mainController::handleEvents();
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    // stateMachine tests
    RUN_TEST(test_transitions_boot);
    RUN_TEST(test_transitions_networkCheck);
    RUN_TEST(test_usb_detection);
    // cli tests
    RUN_TEST(test_setDeviceAddress);
    RUN_TEST(test_setKeys);
    RUN_TEST(test_setName);
    RUN_TEST(test_setRadio);
    RUN_TEST(test_setBattery);
    RUN_TEST(test_setSensor);
    // other tests
    RUN_TEST(test_toString);
    RUN_TEST(test_miniAdr);
    RUN_TEST(test_forCoverageOnly);
    UNITY_END();
}