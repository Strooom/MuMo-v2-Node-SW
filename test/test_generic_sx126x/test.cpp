// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include <sx126x.hpp>

uint8_t mockSX126xDataBuffer[256];            // unitTesting mock for the LoRa Rx/Tx buffer, inside the SX126x
uint8_t mockSX126xRegisters[0x1000];          // unitTesting mock for the config registers, inside the SX126x
uint8_t mockSX126xCommandData[256][8];        // unitTesting mock for capturing the commands and their parameters to configure the SX126x

void setUp(void) {}                           // before test
void tearDown(void) {}                        // after test

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


// void test_configForTransmit() {
//     sx126x::configForTransmit();
//     TEST_ASSERT_EQUAL_UINT8(0x01, mockSX126xCommandData[150][0]);

// }

// void test_configForReceive() {
//     sx126x::configForReceive();
//     TEST_ASSERT_EQUAL_UINT8(0x01, mockSX126xCommandData[150][0]);

// }


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_readWriteTxBuffer);
    RUN_TEST(test_executeSetGetCommand);
    RUN_TEST(test_readWriteRegisters);
    RUN_TEST(test_initialize);
    //RUN_TEST(test_configForTransmit);
    //RUN_TEST(test_configForReceive);
    RUN_TEST(test_calculateFrequencyRegisterValue);
    UNITY_END();
}
