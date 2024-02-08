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

void setUp(void) {}           // before test
void tearDown(void) {}        // after test



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


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_txRxCycleStateToString);
    UNITY_END();
}