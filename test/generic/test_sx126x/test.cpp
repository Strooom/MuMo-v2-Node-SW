// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include <sx126x.hpp>

extern uint8_t mockSX126xDataBuffer[256];            // unitTesting mock for the LoRa Rx/Tx buffer, inside the SX126x
extern uint8_t mockSX126xRegisters[0x1000];          // unitTesting mock for the config registers, inside the SX126x
extern uint8_t mockSX126xCommandData[256][8];        // unitTesting mock for capturing the commands and their parameters to configure the SX126x

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_readWriteTxBuffer() {
    uint8_t dataIn[sx126x::rxTxBufferLength];
    for (uint32_t i = 0; i < sx126x::rxTxBufferLength; i++) {
        dataIn[i] = i;
    }
    uint8_t dataOut[sx126x::rxTxBufferLength];
    sx126x::writeBuffer(dataIn, sx126x::rxTxBufferLength);
    sx126x::readBuffer(dataOut, sx126x::rxTxBufferLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(dataIn, dataOut, sx126x::rxTxBufferLength);
}

void test_executeSetGetCommand() {
    uint8_t commandDataIn[8];
    for (uint32_t i = 0; i < 8; i++) {
        commandDataIn[i] = i;
    }
    sx126x::executeSetCommand(sx126x::command::setTxParams, commandDataIn, 8);
    uint8_t commandDataOut[8];
    sx126x::executeGetCommand(sx126x::command::setTxParams, commandDataOut, 8);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(commandDataIn, commandDataOut, 8);
}

void test_readWriteRegisters() {
    uint8_t dataIn{123};
    uint8_t dataOut{};
    sx126x::writeRegisters(sx126x::registerAddress::LoRaSyncWordMSB, &dataIn, 1);
    sx126x::readRegisters(sx126x::registerAddress::LoRaSyncWordMSB, &dataOut, 1);
    TEST_ASSERT_EQUAL_UINT8(dataIn, dataOut);
}

void test_calculateFrequencyRegisterValue() {
    TEST_ASSERT_EQUAL_UINT32(0x36419999, sx126x::calculateFrequencyRegisterValue(868100000));
    TEST_ASSERT_EQUAL_UINT32(0x3644CCCC, sx126x::calculateFrequencyRegisterValue(868300000));
    TEST_ASSERT_EQUAL_UINT32(0x36380000, sx126x::calculateFrequencyRegisterValue(867500000));
    TEST_ASSERT_EQUAL_UINT32(0x363B3333, sx126x::calculateFrequencyRegisterValue(867700000));
    TEST_ASSERT_EQUAL_UINT32(0x36586666, sx126x::calculateFrequencyRegisterValue(869525000));
    TEST_ASSERT_EQUAL_UINT32(0x363E6666, sx126x::calculateFrequencyRegisterValue(867900000));
}

void test_initialize() {
    sx126x::initializeRadio();
    TEST_ASSERT_EQUAL_UINT8(0x01, mockSX126xCommandData[150][0]);
    // TODO : test all other config
}

void test_configForTransmit() {
    static constexpr uint32_t testPayloadLength{32};
    uint8_t testPayload[testPayloadLength]{};
    sx126x::configForTransmit(spreadingFactor::SF10, 86810000U, testPayload, testPayloadLength);
    TEST_IGNORE_MESSAGE("Implement me!");
}

void test_configForReceive() {
    TEST_IGNORE_MESSAGE("Implement me!");
}

void test_setTxParameters() {
    int8_t testTransmitPowerdBm{0};
    sx126x::setTxParameters(testTransmitPowerdBm);
    uint8_t expectedCommandData[2] = {0x00, 0x02};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedCommandData, mockSX126xCommandData[static_cast<int>(sx126x::command::setTxParams)], 2);

    testTransmitPowerdBm = 14;
    sx126x::setTxParameters(testTransmitPowerdBm);
    expectedCommandData[0] = 0x0E;
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedCommandData, mockSX126xCommandData[static_cast<int>(sx126x::command::setTxParams)], 2);

    testTransmitPowerdBm = -3;
    sx126x::setTxParameters(testTransmitPowerdBm);
    expectedCommandData[0] = 0xFD;
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedCommandData, mockSX126xCommandData[static_cast<int>(sx126x::command::setTxParams)], 2);
}

void test_setPacketParametersReceive() {
    sx126x::setPacketParametersReceive();
    uint8_t expectedCommandData[8] = {0x00, 0x08, 0x00, 0xFF, 0x00, 0x01, 0x00, 0x00};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedCommandData, mockSX126xCommandData[static_cast<int>(sx126x::command::setPacketParameters)], 8);
}

void test_setPacketParametersTransmit() {
    uint8_t testPayloadLength{32};
    sx126x::setPacketParametersTransmit(testPayloadLength);
    uint8_t expectedCommandData[8] = {0x00, 0x08, 0x00, testPayloadLength, 0x01, 0x00, 0x00, 0x00};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedCommandData, mockSX126xCommandData[static_cast<int>(sx126x::command::setPacketParameters)], 8);
}

void test_setModulationParameters() {
    TEST_IGNORE_MESSAGE("Implement me!");
}

void test_setRfFrequency() {
    TEST_IGNORE_MESSAGE("Implement me!");
}

void test_startReceive() {
    TEST_IGNORE_MESSAGE("Implement me!");
}

void test_startTransmit() {
    TEST_IGNORE_MESSAGE("Implement me!");
}

void test_goStandby() {
    TEST_IGNORE_MESSAGE("Implement me!");
}

void test_getStatus() {
    TEST_IGNORE_MESSAGE("Implement me!");
//sx126x::getStatus()
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_readWriteTxBuffer);
    RUN_TEST(test_executeSetGetCommand);
    RUN_TEST(test_readWriteRegisters);
    RUN_TEST(test_initialize);
    RUN_TEST(test_configForTransmit);
    RUN_TEST(test_configForReceive);
    RUN_TEST(test_calculateFrequencyRegisterValue);
    RUN_TEST(test_setTxParameters);
    RUN_TEST(test_setPacketParametersReceive);
    RUN_TEST(test_setPacketParametersTransmit);
    RUN_TEST(test_setModulationParameters);
    RUN_TEST(test_setRfFrequency);
    RUN_TEST(test_startReceive);
    RUN_TEST(test_startTransmit);
    RUN_TEST(test_goStandby);
    RUN_TEST(test_getStatus);
    UNITY_END();
}
