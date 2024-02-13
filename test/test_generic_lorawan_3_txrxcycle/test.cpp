#include <unity.h>
#include <aeskey.hpp>
#include <lorawan.hpp>
#include <circularbuffer.hpp>
#include <lorawanevent.hpp>
#include <applicationevent.hpp>
#include <settingscollection.hpp>
#include <maccommand.hpp>
#include <hexascii.hpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;
circularBuffer<loRaWanEvent, 16U> loraWanEventBuffer;

uint8_t mockSX126xDataBuffer[256];            // unitTesting mock for the LoRa Rx/Tx buffer, inside the SX126x
uint8_t mockSX126xRegisters[0x1000];          // unitTesting mock for the config registers, inside the SX126x
uint8_t mockSX126xCommandData[256][8];        // unitTesting mock for capturing the commands and their parameters to configure the SX126x

uint8_t allZeroes[LoRaWAN::rawMessageLength];

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_messageTypeToString() {
    TEST_ASSERT_EQUAL_STRING("application", toString(messageType ::application));
    TEST_ASSERT_EQUAL_STRING("lorawanMac", toString(messageType ::lorawanMac));
    TEST_ASSERT_EQUAL_STRING("invalid / unknown", toString(static_cast<messageType>(0xFF)));
}

void test_lorawanEventToString() {
    TEST_ASSERT_EQUAL_STRING("none", toString(loRaWanEvent ::none));
    TEST_ASSERT_EQUAL_STRING("sx126x Cad End", toString(loRaWanEvent ::sx126xCadEnd));
    TEST_ASSERT_EQUAL_STRING("sx126x Tx Complete", toString(loRaWanEvent ::sx126xTxComplete));
    TEST_ASSERT_EQUAL_STRING("sx126x Rx Complete", toString(loRaWanEvent ::sx126xRxComplete));
    TEST_ASSERT_EQUAL_STRING("sx126x Timeout", toString(loRaWanEvent ::sx126xTimeout));
    TEST_ASSERT_EQUAL_STRING("timeOut", toString(loRaWanEvent ::timeOut));
    TEST_ASSERT_EQUAL_STRING("unknown event", toString(static_cast<loRaWanEvent>(0xFF)));
}

void test_txRxCycleStateToString() {
    TEST_ASSERT_EQUAL_STRING("idle", toString(txRxCycleState::idle));
    TEST_ASSERT_EQUAL_STRING("waitForCadEnd", toString(txRxCycleState::waitForCadEnd));
    TEST_ASSERT_EQUAL_STRING("waitForRandomTimeBeforeTransmit", toString(txRxCycleState::waitForRandomTimeBeforeTransmit));
    TEST_ASSERT_EQUAL_STRING("waitForTxComplete", toString(txRxCycleState::waitForTxComplete));
    TEST_ASSERT_EQUAL_STRING("waitForRx1Start", toString(txRxCycleState::waitForRx1Start));
    TEST_ASSERT_EQUAL_STRING("waitForRx1CompleteOrTimeout", toString(txRxCycleState::waitForRx1CompleteOrTimeout));
    TEST_ASSERT_EQUAL_STRING("waitForRx2Start", toString(txRxCycleState::waitForRx2Start));
    TEST_ASSERT_EQUAL_STRING("waitForRx2CompleteOrTimeout", toString(txRxCycleState::waitForRx2CompleteOrTimeout));
    TEST_ASSERT_EQUAL_STRING("waitForRxMessageReadout", toString(txRxCycleState::waitForRxMessageReadout));
    TEST_ASSERT_EQUAL_STRING("unknown state", toString(static_cast<txRxCycleState>(99)));
}

void test_initialize() {
    // Putting some dirty data in the context
    LoRaWAN::theTxRxCycleState = txRxCycleState::waitForRxMessageReadout;
    LoRaWAN::macIn.append(0xFF);
    LoRaWAN::macOut.append(0xFF);
    LoRaWAN::rawMessage[0] = 0xFF;

    LoRaWAN::initialize();
    TEST_ASSERT_EQUAL(txRxCycleState::idle, LoRaWAN::theTxRxCycleState);
    TEST_ASSERT_TRUE(LoRaWAN::isIdle());
    TEST_ASSERT_TRUE(LoRaWAN::macIn.isEmpty());
    TEST_ASSERT_TRUE(LoRaWAN::macOut.isEmpty());
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage, LoRaWAN::rawMessageLength);
}

void test_getState() {
    LoRaWAN::theTxRxCycleState = txRxCycleState::idle;
    TEST_ASSERT_EQUAL(txRxCycleState::idle, LoRaWAN::getState());
    LoRaWAN::theTxRxCycleState = txRxCycleState::waitForCadEnd;
    TEST_ASSERT_EQUAL(txRxCycleState::waitForCadEnd, LoRaWAN::getState());
    LoRaWAN::theTxRxCycleState = txRxCycleState::waitForRandomTimeBeforeTransmit;
    TEST_ASSERT_EQUAL(txRxCycleState::waitForRandomTimeBeforeTransmit, LoRaWAN::getState());
    LoRaWAN::theTxRxCycleState = txRxCycleState::waitForTxComplete;
    TEST_ASSERT_EQUAL(txRxCycleState::waitForTxComplete, LoRaWAN::getState());
    LoRaWAN::theTxRxCycleState = txRxCycleState::waitForRx1Start;
    TEST_ASSERT_EQUAL(txRxCycleState::waitForRx1Start, LoRaWAN::getState());
    LoRaWAN::theTxRxCycleState = txRxCycleState::waitForRx1CompleteOrTimeout;
    TEST_ASSERT_EQUAL(txRxCycleState::waitForRx1CompleteOrTimeout, LoRaWAN::getState());
    LoRaWAN::theTxRxCycleState = txRxCycleState::waitForRx2Start;
    TEST_ASSERT_EQUAL(txRxCycleState::waitForRx2Start, LoRaWAN::getState());
    LoRaWAN::theTxRxCycleState = txRxCycleState::waitForRx2CompleteOrTimeout;
    TEST_ASSERT_EQUAL(txRxCycleState::waitForRx2CompleteOrTimeout, LoRaWAN::getState());
    LoRaWAN::theTxRxCycleState = txRxCycleState::waitForRxMessageReadout;
    TEST_ASSERT_EQUAL(txRxCycleState::waitForRxMessageReadout, LoRaWAN::getState());
    LoRaWAN::theTxRxCycleState = static_cast<txRxCycleState>(99);
    TEST_ASSERT_EQUAL(static_cast<txRxCycleState>(99), LoRaWAN::getState());
}

void test_goTo_handleEvents() {
    LoRaWAN::initialize();
    LoRaWAN::goTo(txRxCycleState::waitForRandomTimeBeforeTransmit);
    TEST_MESSAGE("Check that timer is started");
    loraWanEventBuffer.push(loRaWanEvent::timeOut);
    LoRaWAN::handleEvents();
    TEST_MESSAGE("Check that timer is stopped");
    TEST_MESSAGE("Check that SX126x transmission is started");
    TEST_ASSERT_EQUAL(txRxCycleState::waitForTxComplete, LoRaWAN::getState());
    loraWanEventBuffer.push(loRaWanEvent::sx126xTxComplete);
    LoRaWAN::handleEvents();
    TEST_ASSERT_EQUAL(txRxCycleState::waitForRx1Start, LoRaWAN::getState());
    loraWanEventBuffer.push(loRaWanEvent::timeOut);
    LoRaWAN::handleEvents();
    TEST_ASSERT_EQUAL(txRxCycleState::waitForRx1CompleteOrTimeout, LoRaWAN::getState());
    loraWanEventBuffer.push(loRaWanEvent::sx126xTimeout);
    LoRaWAN::handleEvents();
    TEST_ASSERT_EQUAL(txRxCycleState::waitForRx2Start, LoRaWAN::getState());
    loraWanEventBuffer.push(loRaWanEvent::timeOut);
    LoRaWAN::handleEvents();
    TEST_ASSERT_EQUAL(txRxCycleState::waitForRx2CompleteOrTimeout, LoRaWAN::getState());
    loraWanEventBuffer.push(loRaWanEvent::sx126xTimeout);
    LoRaWAN::handleEvents();
    TEST_ASSERT_EQUAL(txRxCycleState::idle, LoRaWAN::getState());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_messageTypeToString);
    RUN_TEST(test_getState);
    RUN_TEST(test_lorawanEventToString);
    RUN_TEST(test_txRxCycleStateToString);
    RUN_TEST(test_goTo_handleEvents);
    UNITY_END();
}