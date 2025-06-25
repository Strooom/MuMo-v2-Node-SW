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

extern uint8_t mockSX126xDataBuffer[256];            // unitTesting mock for the LoRa Rx/Tx buffer, inside the SX126x
extern uint8_t mockSX126xRegisters[0x1000];          // unitTesting mock for the config registers, inside the SX126x
extern uint8_t mockSX126xCommandData[256][8];        // unitTesting mock for capturing the commands and their parameters to configure the SX126x

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

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
    logging::enable(logging::destination::uart2);
    logging::enable(logging::source::lorawanData);
    logging::enable(logging::source::lorawanEvents);
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
}

void test_parse_1() {
    // Parsing a message actually sent by TTN and captured on the gateway
    char testRawMessage[]                = "6011990b2600b000002261d02e6dc32e0495279335d16b41e109c11cb7d5df80232fd81280de42be47c964b382879a3f";
    uint32_t testRawMessageLength        = strlen(testRawMessage);
    uint32_t testRawMessageLengthInBytes = testRawMessageLength / 2;
    hexAscii::hexStringToByteArray(mockSX126xDataBuffer, testRawMessage, testRawMessageLength);
    mockSX126xCommandData[static_cast<uint8_t>(sx126x::command::getRxBufferStatus)][0] = testRawMessageLengthInBytes;
    LoRaWAN::DevAddr.setFromWord(0x260B9911);
    LoRaWAN::applicationKey.setFromHexString("E54E4411F4FE8955904197C8D824BC2C");
    LoRaWAN::networkKey.setFromHexString("8DD6FEB76D7754A78538BA5089A834AA");
    LoRaWAN::generateKeysK1K2();
    LoRaWAN::macIn.initialize();
    LoRaWAN::decodeMessage();
    LoRaWAN::processMacContents();
}


void test_construct_1() {
    // constructing a message, actually received by TTN and captured on their portal
    // FcntUp : 13451
    // Payload ClearText : 02017EC5536412835C40107EC553648D77A241117EC5536450424042127EC553645C897E44
    // Payload Encrypted : a1b61bb7d0a47f5c1d42c7f292b5a34aa326f69bd6ac525c173b97010e49ff92a86d88e2db
    // Fport 16

    // Raw Payload : 4011990b26008b3410a1b61bb7d0a47f5c1d42c7f292b5a34aa326f69bd6ac525c173b97010e49ff92a86d88e2db0184de4a
    // FrmPayload :                    a1b61bb7d0a47f5c1d42c7f292b5a34aa326f69bd6ac525c173b97010e49ff92a86d88e2db

    uint8_t testFramePort{16};

    static constexpr uint32_t testExpectedRawMessageLength{50};
    uint8_t testExpectedRawMessage[testExpectedRawMessageLength];
    hexAscii::hexStringToByteArray(testExpectedRawMessage, "4011990b26008b3410a1b61bb7d0a47f5c1d42c7f292b5a34aa326f69bd6ac525c173b97010e49ff92a86d88e2db0184de4a", 100U);

    static constexpr uint32_t testClearTextPayloadLength{37};
    uint8_t testClearTextPayload[testClearTextPayloadLength];
    hexAscii::hexStringToByteArray(testClearTextPayload, "02017EC5536412835C40107EC553648D77A241117EC5536450424042127EC553645C897E44", 74U);

    LoRaWAN::DevAddr.setFromWord(0x260B9911);
    LoRaWAN::applicationKey.setFromHexString("E54E4411F4FE8955904197C8D824BC2C");
    LoRaWAN::networkKey.setFromHexString("8DD6FEB76D7754A78538BA5089A834AA");
    LoRaWAN::generateKeysK1K2();
    LoRaWAN::uplinkFrameCount.setFromWord(13451);

    // LoRaWAN::sendUplink(testFramePort, testClearTextPayload, testClearTextPayloadLengthInBytes);

    LoRaWAN::setOffsetsAndLengthsTx(testClearTextPayloadLength, 0);                       // Check : assuming no Fopts but I'm not sure as I don't know the internals of the sender of this message
    LoRaWAN::insertHeaders(nullptr, 0, testClearTextPayloadLength, testFramePort);        //
    LoRaWAN::insertPayload(testClearTextPayload, testClearTextPayloadLength);
    LoRaWAN::encryptDecryptPayload(LoRaWAN::applicationKey, linkDirection::uplink);
    LoRaWAN::insertBlockB0(linkDirection::uplink, LoRaWAN::uplinkFrameCount);
    LoRaWAN::insertMic();

    TEST_ASSERT_EQUAL_UINT8_ARRAY(testExpectedRawMessage, LoRaWAN::rawMessage + LoRaWAN::macHeaderOffset, testExpectedRawMessageLength);
}

void test_construct_2() {
    // constructing a message, actually captured on gateway
    // FcntUp : 0
    // Payload ClearText : 00000000000000000000000000000000
    // Payload Encrypted : 4eaa794c99d25ebeffcee0463c3b34de
    // Fport 7

    // appsKey ECF61A5B18BFBF81EF4FA7DBA764CE8B

    // Raw Payload : 401bc70b26000000074eaa794c99d25ebeffcee0463c3b34de0e169362
    // FrmPayload :                    4eaa794c99d25ebeffcee0463c3b34de
    // Mic :                                                           0e169362
    
    uint8_t testFramePort{7};

    static constexpr uint32_t testExpectedRawMessageLength{29};
    uint8_t testExpectedRawMessage[testExpectedRawMessageLength];
    hexAscii::hexStringToByteArray(testExpectedRawMessage, "401bc70b26000000074eaa794c99d25ebeffcee0463c3b34def5926d68", 58U);

    static constexpr uint32_t testClearTextPayloadLength{16};
    uint8_t testClearTextPayload[testClearTextPayloadLength];
    hexAscii::hexStringToByteArray(testClearTextPayload, "00000000000000000000000000000000",32U);

    LoRaWAN::DevAddr.setFromWord(0x260BC71B);
    LoRaWAN::applicationKey.setFromHexString("ECF61A5B18BFBF81EF4FA7DBA764CE8B");
    LoRaWAN::networkKey.setFromHexString("34CE07A8DDE81F4C29A0AED7B4F1D7BB");
    LoRaWAN::generateKeysK1K2();
    LoRaWAN::uplinkFrameCount.setFromWord(0);

    // LoRaWAN::sendUplink(testFramePort, testClearTextPayload, testClearTextPayloadLengthInBytes);

    LoRaWAN::setOffsetsAndLengthsTx(testClearTextPayloadLength, 0);                       // Check : assuming no Fopts but I'm not sure as I don't know the internals of the sender of this message
    LoRaWAN::insertHeaders(nullptr, 0, testClearTextPayloadLength, testFramePort);        //
    LoRaWAN::insertPayload(testClearTextPayload, testClearTextPayloadLength);
    LoRaWAN::encryptDecryptPayload(LoRaWAN::applicationKey, linkDirection::uplink);
    LoRaWAN::insertBlockB0(linkDirection::uplink, LoRaWAN::uplinkFrameCount);
    LoRaWAN::insertMic();

    TEST_ASSERT_EQUAL_UINT8_ARRAY(testExpectedRawMessage, LoRaWAN::rawMessage + LoRaWAN::macHeaderOffset, testExpectedRawMessageLength);
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
    RUN_TEST(test_construct_1);
    RUN_TEST(test_construct_2);
    UNITY_END();
}