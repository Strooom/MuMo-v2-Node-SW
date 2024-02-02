#include <unity.h>
#include <lorawan.hpp>
#include <circularbuffer.hpp>
#include <lorawanevent.hpp>
#include <applicationevent.hpp>
#include <settingscollection.hpp>
#include <maccommand.hpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;
circularBuffer<loRaWanEvent, 16U> loraWanEventBuffer;

uint8_t mockSX126xDataBuffer[256];            // unitTesting mock for the LoRa Rx/Tx buffer, inside the SX126x
uint8_t mockSX126xRegisters[0x1000];          // unitTesting mock for the config registers, inside the SX126x
uint8_t mockSX126xCommandData[256][8];        // unitTesting mock for capturing the commands and their parameters to configure the SX126x

void setUp(void) {}
void tearDown(void) {}
void test_encryptPayload() {
    aesKey aKey;
    uint8_t keyAsBytes[16]{0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};
    aKey.setFromByteArray(keyAsBytes);
    static constexpr uint32_t clearTextPayloadLength{16};
    LoRaWAN::DevAddr.asUint32          = 0x12345678;
    LoRaWAN::uplinkFrameCount.asUint32 = 0xFFEEDDCC;
    LoRaWAN::setOffsetsAndLengthsTx(clearTextPayloadLength, 0);
    uint8_t clearTextPayload[clearTextPayloadLength]{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    LoRaWAN::insertPayload(clearTextPayload, clearTextPayloadLength);
    LoRaWAN::encryptPayload(aKey);

    // Using https://www.cryptool.org/en/cto/aes-step-by-step
    // Key :                       2b7e151628aed2a6abf7158809cf4f3c
    // Ai block i=0 :              01000000000078563412ccddeeff0001
    // Ai block after encryption : 4d9655c553debea0dfa6290bbad3ebc9
    // Then this needs to be XORed with the lorawan payload, here it is all zeroes so nothing changes for easy first test

    TEST_ASSERT_EQUAL_UINT8(0x4D, LoRaWAN::rawMessage[25]);
    TEST_ASSERT_EQUAL_UINT8(0x96, LoRaWAN::rawMessage[26]);
    TEST_ASSERT_EQUAL_UINT8(0x55, LoRaWAN::rawMessage[27]);
    TEST_ASSERT_EQUAL_UINT8(0xC5, LoRaWAN::rawMessage[28]);
    TEST_ASSERT_EQUAL_UINT8(0x53, LoRaWAN::rawMessage[29]);
    TEST_ASSERT_EQUAL_UINT8(0xDE, LoRaWAN::rawMessage[30]);
    TEST_ASSERT_EQUAL_UINT8(0xBE, LoRaWAN::rawMessage[31]);
    TEST_ASSERT_EQUAL_UINT8(0xA0, LoRaWAN::rawMessage[32]);
    TEST_ASSERT_EQUAL_UINT8(0xDF, LoRaWAN::rawMessage[33]);
    TEST_ASSERT_EQUAL_UINT8(0xA6, LoRaWAN::rawMessage[34]);
    TEST_ASSERT_EQUAL_UINT8(0x29, LoRaWAN::rawMessage[35]);
    TEST_ASSERT_EQUAL_UINT8(0x0B, LoRaWAN::rawMessage[36]);
    TEST_ASSERT_EQUAL_UINT8(0xBA, LoRaWAN::rawMessage[37]);
    TEST_ASSERT_EQUAL_UINT8(0xD3, LoRaWAN::rawMessage[38]);
    TEST_ASSERT_EQUAL_UINT8(0xEB, LoRaWAN::rawMessage[39]);
    TEST_ASSERT_EQUAL_UINT8(0xC9, LoRaWAN::rawMessage[40]);
}

void test_decryptPayload() {
    aesKey aKey;
    uint8_t keyAsBytes[16]{0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};
    aKey.setFromByteArray(keyAsBytes);
    static constexpr uint32_t clearTextPayloadLength{16};
    LoRaWAN::DevAddr.asUint32            = 0x12345678;
    LoRaWAN::downlinkFrameCount.asUint32 = 0xFFEEDDCC;
    LoRaWAN::setOffsetsAndLengthsTx(clearTextPayloadLength, 0);
    uint8_t clearTextPayload[clearTextPayloadLength]{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    LoRaWAN::insertPayload(clearTextPayload, clearTextPayloadLength);
    LoRaWAN::decryptPayload(aKey);

    // Using https://www.cryptool.org/en/cto/aes-step-by-step
    //  Key :                      2b7e151628aed2a6abf7158809cf4f3c
    // Ai block i=0 :              01000000000178563412ccddeeff0001
    // Ai block after encryption : d26ee43b33cd5463d48cb2c6cd710e21
    // Then this needs to be XORed with the lorawan payload, here it is all zeroes so nothing changes for easy first test

    TEST_ASSERT_EQUAL_UINT8(0xD2, LoRaWAN::rawMessage[25]);
    TEST_ASSERT_EQUAL_UINT8(0x6E, LoRaWAN::rawMessage[26]);
    TEST_ASSERT_EQUAL_UINT8(0xE4, LoRaWAN::rawMessage[27]);
    TEST_ASSERT_EQUAL_UINT8(0x3B, LoRaWAN::rawMessage[28]);
    TEST_ASSERT_EQUAL_UINT8(0x33, LoRaWAN::rawMessage[29]);
    TEST_ASSERT_EQUAL_UINT8(0xCD, LoRaWAN::rawMessage[30]);
    TEST_ASSERT_EQUAL_UINT8(0x54, LoRaWAN::rawMessage[31]);
    TEST_ASSERT_EQUAL_UINT8(0x63, LoRaWAN::rawMessage[32]);
    TEST_ASSERT_EQUAL_UINT8(0xD4, LoRaWAN::rawMessage[33]);
    TEST_ASSERT_EQUAL_UINT8(0x8C, LoRaWAN::rawMessage[34]);
    TEST_ASSERT_EQUAL_UINT8(0xB2, LoRaWAN::rawMessage[35]);
    TEST_ASSERT_EQUAL_UINT8(0xC6, LoRaWAN::rawMessage[36]);
    TEST_ASSERT_EQUAL_UINT8(0xCD, LoRaWAN::rawMessage[37]);
    TEST_ASSERT_EQUAL_UINT8(0x71, LoRaWAN::rawMessage[38]);
    TEST_ASSERT_EQUAL_UINT8(0x0E, LoRaWAN::rawMessage[39]);
    TEST_ASSERT_EQUAL_UINT8(0x21, LoRaWAN::rawMessage[40]);
}

void test_isValidMic1() {
    static constexpr uint32_t testLoraPayloadLength{0x2B};
    uint8_t rxTestMessage[testLoraPayloadLength] = {0x60, 0x92, 0x3B, 0x0B, 0x26, 0x00, 0x3B, 0x00, 0x00, 0x11, 0x7C, 0xD9, 0xC4, 0xD5, 0x27, 0xA1, 0x78, 0xBD, 0x07, 0x88, 0x8B, 0x67, 0x52, 0xBC, 0xE4, 0x47, 0x26, 0xF9, 0x2B, 0x62, 0x89, 0xC3, 0x06, 0x2F, 0x69, 0x96, 0x4D, 0xD9, 0x18, 0x22, 0x58, 0x5F, 0x55};
    memcpy(LoRaWAN::rawMessage + LoRaWAN::b0BlockLength, rxTestMessage, testLoraPayloadLength);
    LoRaWAN::setOffsetsAndLengthsRx(testLoraPayloadLength);
    LoRaWAN::DevAddr.asUint32 = 0x260B3B92;
    LoRaWAN::downlinkFrameCount.set(0x0000'003B);
    TEST_IGNORE_MESSAGE("This test is not yet implemented");
//    TEST_ASSERT_TRUE(LoRaWAN::isValidMic());
}

void test_isValidMic2() {
    static constexpr uint32_t testLoraPayloadLength{15};
    uint8_t rxTestMessage[testLoraPayloadLength] = {0x60, 0x51, 0xAE, 0x0B, 0x26, 0x03, 0x01, 0x00, 0x02, 0x11, 0x02, 0x65, 0xF4, 0x53, 0xB5};
    memcpy(LoRaWAN::rawMessage + LoRaWAN::b0BlockLength, rxTestMessage, testLoraPayloadLength);
    LoRaWAN::setOffsetsAndLengthsRx(testLoraPayloadLength);
    LoRaWAN::downlinkFrameCount.set(0x0000'003B);
    TEST_IGNORE_MESSAGE("This test is not yet implemented");
//    TEST_ASSERT_TRUE(LoRaWAN::isValidMic());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_encryptPayload);
    RUN_TEST(test_decryptPayload);
    RUN_TEST(test_isValidMic1);
    RUN_TEST(test_isValidMic2);
    UNITY_END();
}