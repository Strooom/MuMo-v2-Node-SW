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

void test_macCommandToString() {
    TEST_ASSERT_EQUAL_STRING("linkCheckRequest", toString(macCommand::linkCheckRequest, linkDirection::uplink));
    TEST_ASSERT_EQUAL_STRING("linkCheckAnswer", toString(macCommand::linkCheckAnswer, linkDirection::downlink));

    TEST_ASSERT_EQUAL_STRING("linkAdaptiveDataRateAnswer", toString(macCommand::linkAdaptiveDataRateRequest, linkDirection::uplink));
    TEST_ASSERT_EQUAL_STRING("linkAdaptiveDataRateRequest", toString(macCommand::linkAdaptiveDataRateAnswer, linkDirection::downlink));

    TEST_ASSERT_EQUAL_STRING("dutyCycleAnswer", toString(macCommand::dutyCycleRequest, linkDirection::uplink));
    TEST_ASSERT_EQUAL_STRING("dutyCycleRequest", toString(macCommand::dutyCycleAnswer, linkDirection::downlink));

    TEST_ASSERT_EQUAL_STRING("receiveParameterSetupAnswer", toString(macCommand::receiveParameterSetupAnswer, linkDirection::uplink));
    TEST_ASSERT_EQUAL_STRING("receiveParameterSetupRequest", toString(macCommand::receiveParameterSetupRequest, linkDirection::downlink));

    TEST_ASSERT_EQUAL_STRING("deviceStatusAnswer", toString(macCommand::deviceStatusAnswer, linkDirection::uplink));
    TEST_ASSERT_EQUAL_STRING("deviceStatusRequest", toString(macCommand::deviceStatusRequest, linkDirection::downlink));

    TEST_ASSERT_EQUAL_STRING("newChannelAnswer", toString(macCommand::newChannelAnswer, linkDirection::uplink));
    TEST_ASSERT_EQUAL_STRING("newChannelRequest", toString(macCommand::newChannelRequest, linkDirection::downlink));

    TEST_ASSERT_EQUAL_STRING("receiveTimingSetupAnswer", toString(macCommand::receiveTimingSetupAnswer, linkDirection::uplink));
    TEST_ASSERT_EQUAL_STRING("receiveTimingSetupRequest", toString(macCommand::receiveTimingSetupRequest, linkDirection::downlink));

    TEST_ASSERT_EQUAL_STRING("dutyCycleAnswer", toString(macCommand::dutyCycleAnswer, linkDirection::uplink));
    TEST_ASSERT_EQUAL_STRING("dutyCycleRequest", toString(macCommand::dutyCycleRequest, linkDirection::downlink));

    TEST_ASSERT_EQUAL_STRING("transmitParameterSetupAnswer", toString(macCommand::transmitParameterSetupAnswer, linkDirection::uplink));
    TEST_ASSERT_EQUAL_STRING("transmitParameterSetupRequest", toString(macCommand::transmitParameterSetupRequest, linkDirection::downlink));

    TEST_ASSERT_EQUAL_STRING("downlinkChannelAnswer", toString(macCommand::downlinkChannelAnswer, linkDirection::uplink));
    TEST_ASSERT_EQUAL_STRING("downlinkChannelRequest", toString(macCommand::downlinkChannelRequest, linkDirection::downlink));

    TEST_ASSERT_EQUAL_STRING("deviceTimeRequest", toString(macCommand::deviceTimeRequest, linkDirection::uplink));
    TEST_ASSERT_EQUAL_STRING("deviceTimeAnswer", toString(macCommand::deviceTimeAnswer, linkDirection::downlink));

    TEST_ASSERT_EQUAL_STRING("unknown MAC command", toString(static_cast<macCommand>(99), linkDirection::uplink));
    TEST_ASSERT_EQUAL_STRING("unknown MAC command", toString(static_cast<macCommand>(99), linkDirection::downlink));
}

void test_removeNonStickyMacStuff() {
    LoRaWAN::macOut.initialize();
    // Put every possible uplink mac command (+ the command parameters, if any) in the buffer

    LoRaWAN::macOut.append(static_cast<uint8_t>(macCommand::linkCheckRequest));
    LoRaWAN::macOut.append(static_cast<uint8_t>(macCommand::linkAdaptiveDataRateAnswer));
    LoRaWAN::macOut.append(0x00);
    LoRaWAN::macOut.append(static_cast<uint8_t>(macCommand::dutyCycleAnswer));
    LoRaWAN::macOut.append(static_cast<uint8_t>(macCommand::receiveParameterSetupAnswer));        // Sticky
    LoRaWAN::macOut.append(0x00);
    LoRaWAN::macOut.append(static_cast<uint8_t>(macCommand::deviceStatusAnswer));
    LoRaWAN::macOut.append(0x00);
    LoRaWAN::macOut.append(0x00);
    LoRaWAN::macOut.append(static_cast<uint8_t>(macCommand::newChannelAnswer));
    LoRaWAN::macOut.append(0x00);
    LoRaWAN::macOut.append(static_cast<uint8_t>(macCommand::receiveTimingSetupAnswer));            // Sticky
    LoRaWAN::macOut.append(static_cast<uint8_t>(macCommand::transmitParameterSetupAnswer));        // Sticky
    LoRaWAN::macOut.append(static_cast<uint8_t>(macCommand::downlinkChannelAnswer));               // Sticky
    LoRaWAN::macOut.append(0x00);
    LoRaWAN::macOut.append(static_cast<uint8_t>(macCommand::deviceTimeRequest));

    LoRaWAN::removeNonStickyMacStuff();

    // Check that only the sticky ones are left
    TEST_ASSERT_EQUAL(0x06, LoRaWAN::macOut.getLevel());
    TEST_ASSERT_EQUAL(static_cast<uint8_t>(macCommand::receiveParameterSetupAnswer), LoRaWAN::macOut[0]);
    TEST_ASSERT_EQUAL(0x00, LoRaWAN::macOut[1]);
    TEST_ASSERT_EQUAL(static_cast<uint8_t>(macCommand::receiveTimingSetupAnswer), LoRaWAN::macOut[2]);
    TEST_ASSERT_EQUAL(static_cast<uint8_t>(macCommand::transmitParameterSetupAnswer), LoRaWAN::macOut[3]);
    TEST_ASSERT_EQUAL(static_cast<uint8_t>(macCommand::downlinkChannelAnswer), LoRaWAN::macOut[4]);
    TEST_ASSERT_EQUAL(0x00, LoRaWAN::macOut[5]);
}

void test_process_linkCheckAnswer() {}
void test_process_linkAdaptiveDataRateRequest() {}
void test_process_dutyCycleRequest() {}
void test_process_receiveParameterSetupRequest() {}
void test_process_deviceStatusRequest() {}
void test_process_newChannelRequest() {}
void test_process_receiveTimingSetupRequest() {}
void test_process_linkAdaptiveDataRateAnswer() {}
void test_process_transmitParameterSetupRequest() {}
void test_process_downlinkChannelRequest() {}
void test_process_deviceTimeAnswer() {
    // UTC	Feb 12, 2024	20:43:08
    // Data	6E	45	F5	52
    // GPS = 1391805806
    // unix = 1707770588

    LoRaWAN::macIn.initialize();
    LoRaWAN::macIn.append(static_cast<uint8_t>(macCommand::deviceTimeAnswer));
    LoRaWAN::macIn.append(0x6E);
    LoRaWAN::macIn.append(0x45);
    LoRaWAN::macIn.append(0xF5);
    LoRaWAN::macIn.append(0x52);
    LoRaWAN::macIn.append(0x00);
    LoRaWAN::processDeviceTimeAnswer();
    // TEST_ASSERT_EQUAL(1391805806, LoRaWAN::gpsTime);
    // TEST_ASSERT_EQUAL(1707770588, LoRaWAN::unixTime);
}
void test_process_unknown_MAC_command() {}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_macCommandToString);
    RUN_TEST(test_removeNonStickyMacStuff);
    RUN_TEST(test_process_linkCheckAnswer);
    RUN_TEST(test_process_linkAdaptiveDataRateRequest);
    RUN_TEST(test_process_dutyCycleRequest);
    RUN_TEST(test_process_receiveParameterSetupRequest);
    RUN_TEST(test_process_deviceStatusRequest);
    RUN_TEST(test_process_newChannelRequest);
    RUN_TEST(test_process_receiveTimingSetupRequest);
    RUN_TEST(test_process_linkAdaptiveDataRateAnswer);
    RUN_TEST(test_process_transmitParameterSetupRequest);
    RUN_TEST(test_process_downlinkChannelRequest);
    RUN_TEST(test_process_deviceTimeAnswer);
    RUN_TEST(test_process_unknown_MAC_command);
    UNITY_END();
}