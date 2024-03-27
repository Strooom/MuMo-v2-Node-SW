// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

// This test application verifies the correct operation of the LoRaWAN MAC layer.
// Each test inputs a received MAC command with its parameters, and checks the resulting LoRaWAN state change as well as the generated MAC response


#include <unity.h>
#include <cstring>
#include "lorawan.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

// The following objects are needed by the LoRaWAN layer to function - they are created by the application.
#include "nvs.h"
#include "sx126x.h"
#include "eventbuffer.h"
#include "applicationevent.h"
nonVolatileStorage nvs;
eventBuffer<loRaWanEvent, 16U> loraWanEventBuffer;
eventBuffer<applicationEvent, 16U> applicationEventBuffer;
sx126x theRadio;

void test_initialize() {
    LoRaWAN theNetwork;
    theNetwork.macIn.initialize();
    theNetwork.macOut.initialize();
    TEST_ASSERT_TRUE(theNetwork.macIn.isEmpty());
    TEST_ASSERT_TRUE(theNetwork.macOut.isEmpty());
}

void test_newChannelRequest() {
    constexpr uint32_t testVectorInLength        = 6;
    const uint8_t testVectorIn[testVectorInLength] = {0x07, 0x03, 0x18, 0x4F, 0x84, 0x50};

    LoRaWAN theNetwork;
    theNetwork.macIn.initialize();
    theNetwork.macIn.append(testVectorIn, testVectorInLength);
    TEST_ASSERT_EQUAL_UINT32(testVectorInLength, theNetwork.macIn.getLevel());
    theNetwork.processMacContents();
    TEST_ASSERT_TRUE(theNetwork.macIn.isEmpty());
    TEST_ASSERT_FALSE(theNetwork.macOut.isEmpty());

    constexpr uint32_t testVectorOutLength        = 2;
    const uint8_t testVectorOut[testVectorOutLength] = {0x07, 0x03};

    TEST_ASSERT_EQUAL_UINT32(testVectorOutLength, theNetwork.macOut.getLevel());
    TEST_ASSERT_EQUAL_INT8_ARRAY(testVectorOut, theNetwork.macOut.asUint8Ptr(), testVectorOutLength);

    // TODO : check the resulting state change
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_newChannelRequest);
    UNITY_END();
}
