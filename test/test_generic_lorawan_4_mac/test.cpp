#include <unity.h>
#include <aeskey.hpp>
#include <lorawan.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <settingscollection.hpp>
#include <maccommand.hpp>
#include <hexascii.hpp>
#include <logging.hpp>
#include <sx126x.hpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

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

void test_process_linkCheckAnswer() {
    LoRaWAN::macIn.initialize();
    LoRaWAN::macIn.append(static_cast<uint8_t>(macCommand::linkCheckAnswer));
    LoRaWAN::macIn.append(0x01);
    LoRaWAN::macIn.append(0x02);
    LoRaWAN::processMacContents();
    // TEST_ASSERT_EQUAL(0x0102, LoRaWAN::linkCheckMargin);
    // TEST_ASSERT_EQUAL(0x0102, LoRaWAN::linkCheckGwCnt);
    TEST_ASSERT_TRUE(LoRaWAN::macIn.isEmpty());
}

void test_process_linkAdaptiveDataRateRequest() {
    LoRaWAN::macIn.initialize();
    LoRaWAN::macIn.append(static_cast<uint8_t>(macCommand::linkAdaptiveDataRateRequest));
    LoRaWAN::macIn.append(0x01);
    LoRaWAN::processMacContents();
    // TEST_ASSERT_EQUAL(0x01, LoRaWAN::linkAdaptiveDataRate);
    TEST_ASSERT_TRUE(LoRaWAN::macIn.isEmpty());
}
void test_process_dutyCycleRequest() {
    LoRaWAN::macIn.initialize();
    LoRaWAN::macIn.append(static_cast<uint8_t>(macCommand::dutyCycleRequest));
    LoRaWAN::macIn.append(0x01);
    LoRaWAN::processMacContents();
    // TEST_ASSERT_EQUAL(0x01, LoRaWAN::dutyCycle);
    TEST_ASSERT_TRUE(LoRaWAN::macIn.isEmpty());
}
void test_process_receiveParameterSetupRequest() {
    LoRaWAN::macIn.initialize();
    LoRaWAN::macIn.append(static_cast<uint8_t>(macCommand::receiveParameterSetupRequest));
    LoRaWAN::macIn.append(0x01);
    LoRaWAN::macIn.append(0x02);
    LoRaWAN::macIn.append(0x03);
    LoRaWAN::macIn.append(0x04);
    LoRaWAN::processMacContents();
    // TEST_ASSERT_EQUAL(0x01020304, LoRaWAN::receiveDelay1);
    TEST_ASSERT_TRUE(LoRaWAN::macIn.isEmpty());
}
void test_process_deviceStatusRequest() {
    LoRaWAN::macIn.initialize();
    LoRaWAN::macIn.append(static_cast<uint8_t>(macCommand::deviceStatusRequest));
    LoRaWAN::processMacContents();
    // TEST_ASSERT_EQUAL(0x01, LoRaWAN::battery);
    // TEST_ASSERT_EQUAL(0x02, LoRaWAN::margin);
    TEST_ASSERT_TRUE(LoRaWAN::macIn.isEmpty());
}
void test_process_newChannelRequest() {
    LoRaWAN::macIn.initialize();
    LoRaWAN::macIn.append(static_cast<uint8_t>(macCommand::newChannelRequest));
    LoRaWAN::macIn.append(0x01);
    LoRaWAN::macIn.append(0x02);
    LoRaWAN::macIn.append(0x03);
    LoRaWAN::macIn.append(0x04);
    LoRaWAN::macIn.append(0x05);
    LoRaWAN::processMacContents();
    // TEST_ASSERT_EQUAL(0x01, LoRaWAN::channelIndex);
    // TEST_ASSERT_EQUAL(0x02030405, LoRaWAN::frequency);
    TEST_ASSERT_TRUE(LoRaWAN::macIn.isEmpty());
}
void test_process_receiveTimingSetupRequest() {
    LoRaWAN::macIn.initialize();
    LoRaWAN::macIn.append(static_cast<uint8_t>(macCommand::receiveTimingSetupRequest));
    LoRaWAN::macIn.append(0x01);
    LoRaWAN::macIn.append(0x02);
    LoRaWAN::processMacContents();
    // TEST_ASSERT_EQUAL(0x0102, LoRaWAN::receiveDelay2);
    TEST_ASSERT_TRUE(LoRaWAN::macIn.isEmpty());
}
void test_process_linkAdaptiveDataRateAnswer() {
    LoRaWAN::macIn.initialize();
    LoRaWAN::macIn.append(static_cast<uint8_t>(macCommand::linkAdaptiveDataRateAnswer));
    LoRaWAN::macIn.append(0x01);
    LoRaWAN::processMacContents();
    // TEST_ASSERT_EQUAL(0x01, LoRaWAN::linkAdaptiveDataRate);
    TEST_ASSERT_TRUE(LoRaWAN::macIn.isEmpty());
}
void test_process_transmitParameterSetupRequest() {
    LoRaWAN::macIn.initialize();
    LoRaWAN::macIn.append(static_cast<uint8_t>(macCommand::transmitParameterSetupRequest));
    LoRaWAN::macIn.append(0x01);
    LoRaWAN::macIn.append(0x02);
    LoRaWAN::macIn.append(0x03);
    LoRaWAN::macIn.append(0x04);
    LoRaWAN::processMacContents();
    // TEST_ASSERT_EQUAL(0x01020304, LoRaWAN::transmitPower);
    TEST_ASSERT_TRUE(LoRaWAN::macIn.isEmpty());
}
void test_process_downlinkChannelRequest() {
    LoRaWAN::macIn.initialize();
    LoRaWAN::macIn.append(static_cast<uint8_t>(macCommand::downlinkChannelRequest));
    LoRaWAN::macIn.append(0x01);
    LoRaWAN::macIn.append(0x02);
    LoRaWAN::macIn.append(0x03);
    LoRaWAN::macIn.append(0x04);
    LoRaWAN::macIn.append(0x05);
    LoRaWAN::processMacContents();
    // TEST_ASSERT_EQUAL(0x01, LoRaWAN::channelIndex);
    // TEST_ASSERT_EQUAL(0x02030405, LoRaWAN::frequency);
    TEST_ASSERT_TRUE(LoRaWAN::macIn.isEmpty());
}
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
    LoRaWAN::processMacContents();
    // TEST_ASSERT_EQUAL(1391805806, LoRaWAN::gpsTime);
    // TEST_ASSERT_EQUAL(1707770588, LoRaWAN::unixTime);
    TEST_ASSERT_TRUE(LoRaWAN::macIn.isEmpty());
}

void test_process_unknown_MAC_command() {
    LoRaWAN::macIn.initialize();
    LoRaWAN::macIn.append(0xFF);
    LoRaWAN::processMacContents();
    // Should have detected the error
    TEST_ASSERT_TRUE(LoRaWAN::macIn.isEmpty());
}

void test_dump() {
    TEST_MESSAGE("For Coverage only");
    logging::enable(logging::source::lorawanMac);
    LoRaWAN::dumpConfig();
    LoRaWAN::dumpState();
    LoRaWAN::dumpChannels();

    LoRaWAN::macIn.initialize();
    LoRaWAN::macIn.append(0x01);
    LoRaWAN::macOut.initialize();
    LoRaWAN::macOut.append(0x01);
    LoRaWAN::dumpMacIn();
    LoRaWAN::dumpMacOut();
    logging::disable(logging::source::lorawanMac);
}

void test_parse_1() {
    char testRawMessage[]                = "6011990b2600b000002261d02e6dc32e0495279335d16b41e109c11cb7d5df80232fd81280de42be47c964b382879a3f";
    uint32_t testRawMessageLength        = strlen(testRawMessage);
    uint32_t testRawMessageLengthInBytes = testRawMessageLength / 2;
    hexAscii::hexStringToByteArray(mockSX126xDataBuffer, testRawMessage);
    mockSX126xCommandData[static_cast<uint8_t>(sx126x::command::getRxBufferStatus)][0] = testRawMessageLengthInBytes;
    LoRaWAN::DevAddr                                                                   = 0x260B9911;
    LoRaWAN::applicationKey.setFromHexString("E54E4411F4FE8955904197C8D824BC2C");
    LoRaWAN::networkKey.setFromHexString("8DD6FEB76D7754A78538BA5089A834AA");
    LoRaWAN::generateKeysK1K2();
    LoRaWAN::macIn.initialize();
    LoRaWAN::decodeMessage();
    LoRaWAN::processMacContents();
}

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
    RUN_TEST(test_dump);

    RUN_TEST(test_parse_1);
    UNITY_END();
}