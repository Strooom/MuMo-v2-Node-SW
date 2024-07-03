#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <aesblock.hpp>
#include <hexascii.hpp>
#include <lorawan.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <settingscollection.hpp>
#include <maccommand.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

uint8_t allZeroes[LoRaWAN::rawMessageLength];
uint8_t allOnes[LoRaWAN::rawMessageLength];
uint8_t testClearText[LoRaWAN::rawMessageLength];
uint32_t testClearTextLength{};
uint32_t testCypherTextLength{};

void setUp(void) {
    memset(allZeroes, 0, LoRaWAN::rawMessageLength);
    memset(allOnes, 0xFF, LoRaWAN::rawMessageLength);
    for (uint32_t index = 0; index < 256; index++) {
        testClearText[index] = index;
    }
}

void tearDown(void) {        // after each test
}

void test_prepareBlockAiTx() {
    aesBlock testBlock;
    LoRaWAN::DevAddr          = 0x12345678;
    LoRaWAN::uplinkFrameCount = 0xFFEEDDCC;
    uint32_t testBlockIndex{7};
    LoRaWAN::prepareBlockAi(testBlock, linkDirection::uplink, testBlockIndex);
    aesBlock expectedBlock{0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x56, 0x34, 0x12, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x07};
    TEST_ASSERT_TRUE(expectedBlock == testBlock);
}

void test_prepareBlockAiRx() {
    aesBlock testBlock;
    LoRaWAN::DevAddr            = 0x12345678;
    LoRaWAN::downlinkFrameCount = 0xFFEEDDCC;
    uint32_t testBlockIndex{3};
    LoRaWAN::prepareBlockAi(testBlock, linkDirection::downlink, testBlockIndex);
    aesBlock expectedBlock{0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x78, 0x56, 0x34, 0x12, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x03};
    TEST_ASSERT_TRUE(expectedBlock == testBlock);
}

void test_encryptPayload() {
    // Useful tools for generating test vectors and expected results
    // Encryption : https://www.cryptool.org/en/cto/aes-step-by-step
    // XOR byte arrays : https://tomeko.net/online_tools/xor.php?lang=en
    // other : https://aesencryptiondecryption.tool-kit.dev/
    // other : https://devtoolcafe.com/tools/aes

    LoRaWAN::clearRawMessage();
    LoRaWAN::applicationKey.setFromHexString("2B7E151628AED2A6ABF7158809CF4F3C");
    LoRaWAN::DevAddr          = 0x12345678;
    LoRaWAN::uplinkFrameCount = 0xFFEEDDCC;

    // Case 1 : 16 bytes payload = 1 block

    testClearTextLength = 16;
    LoRaWAN::clearRawMessage();
    LoRaWAN::setOffsetsAndLengthsTx(testClearTextLength, 0);
    LoRaWAN::insertPayload(testClearText, testClearTextLength);
    LoRaWAN::encryptDecryptPayload(LoRaWAN::applicationKey, linkDirection::uplink);
    const uint8_t expectedEncryptedPayload1[16]{0x4D, 0x97, 0x57, 0xC6, 0x57, 0xDB, 0xB8, 0xA7, 0xD7, 0xAF, 0x23, 0x00, 0xB6, 0xDE, 0xE5, 0xC6};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedEncryptedPayload1, LoRaWAN::rawMessage + LoRaWAN::framePayloadOffset, testClearTextLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage, LoRaWAN::framePayloadOffset);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage + LoRaWAN::micOffset, LoRaWAN::rawMessageLength - LoRaWAN::micOffset);

    // Case 2 : 32 bytes payload = 2 block

    testClearTextLength = 32;
    LoRaWAN::clearRawMessage();
    LoRaWAN::setOffsetsAndLengthsTx(testClearTextLength, 0);
    LoRaWAN::insertPayload(testClearText, testClearTextLength);
    LoRaWAN::encryptDecryptPayload(LoRaWAN::applicationKey, linkDirection::uplink);
    const uint8_t expectedEncryptedPayload2[32]{0x4D, 0x97, 0x57, 0xC6, 0x57, 0xDB, 0xB8, 0xA7, 0xD7, 0xAF, 0x23, 0x00, 0xB6, 0xDE, 0xE5, 0xC6, 0x83, 0x67, 0xFF, 0x84, 0x34, 0x2F, 0xA5, 0x48, 0x93, 0xE8, 0xB1, 0xC3, 0xAA, 0x23, 0x89, 0x31};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedEncryptedPayload2, LoRaWAN::rawMessage + LoRaWAN::framePayloadOffset, testClearTextLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage, LoRaWAN::framePayloadOffset);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage + LoRaWAN::micOffset, LoRaWAN::rawMessageLength - LoRaWAN::micOffset);

    // Case 3 : 40 bytes payload : padding needed to 3 blocks

    testClearTextLength = 40;
    LoRaWAN::clearRawMessage();
    LoRaWAN::setOffsetsAndLengthsTx(testClearTextLength, 0);
    LoRaWAN::insertPayload(testClearText, testClearTextLength);
    LoRaWAN::encryptDecryptPayload(LoRaWAN::applicationKey, linkDirection::uplink);
    const uint8_t expectedEncryptedPayload3[40]{0x4D, 0x97, 0x57, 0xC6, 0x57, 0xDB, 0xB8, 0xA7, 0xD7, 0xAF, 0x23, 0x00, 0xB6, 0xDE, 0xE5, 0xC6, 0x83, 0x67, 0xFF, 0x84, 0x34, 0x2F, 0xA5, 0x48, 0x93, 0xE8, 0xB1, 0xC3, 0xAA, 0x23, 0x89, 0x31, 0x4B, 0x2D, 0x32, 0xC0, 0x59, 0x4C, 0x67, 0x32};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedEncryptedPayload3, LoRaWAN::rawMessage + LoRaWAN::framePayloadOffset, testClearTextLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage, LoRaWAN::framePayloadOffset);
    // TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage + LoRaWAN::micOffset, LoRaWAN::rawMessageLength - LoRaWAN::micOffset); As there is padding, not everything after the payload is untouched

    // Case 4 : 0 bytes payload : edge case, no encryption needed

    testClearTextLength = 0;
    LoRaWAN::clearRawMessage();
    LoRaWAN::setOffsetsAndLengthsTx(testClearTextLength, 0);
    LoRaWAN::insertPayload(testClearText, testClearTextLength);
    LoRaWAN::encryptDecryptPayload(LoRaWAN::applicationKey, linkDirection::uplink);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage, LoRaWAN::rawMessageLength);
}

void test_decryptPayload() {
    const uint32_t numberOfHeaderBytes{13};        // MHDR[1] FHDR[7] FPORT[1] MIC[4]
    LoRaWAN::applicationKey.setFromHexString("2B7E151628AED2A6ABF7158809CF4F3C");
    LoRaWAN::DevAddr            = 0x12345678;
    LoRaWAN::downlinkFrameCount = 0xFFEEDDCC;

    // Case 1 : 16 bytes payload = 1 block

    testCypherTextLength = 16;
    // testCyperText is constructed by taking Ai block (Rx), encrypt with the key (https://www.cryptool.org/en/cto/aes-step-by-step) and then XOR the output with the clear text (https://tomeko.net/online_tools/xor.php?lang=en)
    uint8_t testCypherText1[16]{0xD2, 0x6F, 0xE6, 0x38, 0x37, 0xC8, 0x52, 0x64, 0xDC, 0x85, 0xB8, 0xCD, 0xC1, 0x7C, 0x00, 0x2E};
    LoRaWAN::clearRawMessage();
    LoRaWAN::setOffsetsAndLengthsRx(testCypherTextLength + numberOfHeaderBytes);
    memcpy(LoRaWAN::rawMessage + LoRaWAN::framePayloadOffset, testCypherText1, testCypherTextLength);
    LoRaWAN::encryptDecryptPayload(LoRaWAN::applicationKey);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(testClearText, LoRaWAN::rawMessage + LoRaWAN::framePayloadOffset, testCypherTextLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage, LoRaWAN::framePayloadOffset);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage + LoRaWAN::micOffset, LoRaWAN::rawMessageLength - LoRaWAN::micOffset);

    // Case 3 : 40 bytes payload

    testCypherTextLength = 40;
    uint8_t testCypherText2[40]{0xD2, 0x6F, 0xE6, 0x38, 0x37, 0xC8, 0x52, 0x64, 0xDC, 0x85, 0xB8, 0xCD, 0xC1, 0x7C, 0x00, 0x2E, 0x94, 0x4C, 0x5A, 0x69, 0x96, 0x94, 0xFE, 0x92, 0x73, 0x43, 0xEA, 0x7C, 0x6D, 0x32, 0xA3, 0xCE, 0x0A, 0xD9, 0x8B, 0x25, 0xDE, 0xEE, 0xA2, 0xEF};
    LoRaWAN::clearRawMessage();
    LoRaWAN::setOffsetsAndLengthsRx(testCypherTextLength + numberOfHeaderBytes);
    memcpy(LoRaWAN::rawMessage + LoRaWAN::framePayloadOffset, testCypherText2, testCypherTextLength);
    LoRaWAN::encryptDecryptPayload(LoRaWAN::applicationKey);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(testClearText, LoRaWAN::rawMessage + LoRaWAN::framePayloadOffset, testCypherTextLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage, LoRaWAN::framePayloadOffset);
    // TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage + LoRaWAN::micOffset, LoRaWAN::rawMessageLength - LoRaWAN::micOffset); As there is padding, not everything after the payload is untouched

    // Case 4 : 0 bytes payload : edge case, no decryption needed

    testCypherTextLength = 0;
    LoRaWAN::clearRawMessage();
    LoRaWAN::setOffsetsAndLengthsRx(testCypherTextLength + numberOfHeaderBytes);
    LoRaWAN::encryptDecryptPayload(LoRaWAN::applicationKey);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage, LoRaWAN::rawMessageLength);
}

// https://www.rfc-editor.org/rfc/rfc4493#page-14
// https://www.rfc-editor.org/rfc/rfc3686#page-9

//    --------------------------------------------------
//    Subkey Generation
//    K              2b7e1516 28aed2a6 abf71588 09cf4f3c
//    AES-128(key,0) 7df76b0c 1ab899b3 3e42f047 b91b546f
//    K1             fbeed618 35713366 7c85e08f 7236a8de
//    K2             f7ddac30 6ae266cc f90bc11e e46d513b
//    --------------------------------------------------

void test_keyGeneration() {
    LoRaWAN::networkKey.setFromHexString("2b7e151628aed2a6abf7158809cf4f3c");
    LoRaWAN::generateKeysK1K2();

    aesBlock toBeK1;
    hexAscii::hexStringToByteArray(toBeK1.asBytes(), "fbeed618357133667c85e08f7236a8de");
    aesBlock toBeK2;
    hexAscii::hexStringToByteArray(toBeK2.asBytes(), "f7ddac306ae266ccf90bc11ee46d513b");

    TEST_ASSERT_TRUE(toBeK1 == LoRaWAN::K1);
    TEST_ASSERT_TRUE(toBeK2 == LoRaWAN::K2);
}

void test_calculateMic() {
    // Set of testVectors not related to LoRaWAN
    LoRaWAN::clearRawMessage();
    LoRaWAN::networkKey.setFromHexString("2b7e151628aed2a6abf7158809cf4f3c");
    LoRaWAN::generateKeysK1K2();
    LoRaWAN::micOffset = 0;
    LoRaWAN::padForMicCalculation(0);
    TEST_ASSERT_EQUAL(0x29691DBB, LoRaWAN::calculateMic());

    static constexpr uint32_t clearText1Length{16};
    uint8_t clearText1[clearText1Length];
    hexAscii::hexStringToByteArray(clearText1, "6bc1bee22e409f96e93d7e117393172a");        // test vector from rfc4493 : https://www.rfc-editor.org/rfc/rfc4493
    LoRaWAN::clearRawMessage();
    memcpy(LoRaWAN::rawMessage, clearText1, clearText1Length);
    LoRaWAN::micOffset         = clearText1Length;
    LoRaWAN::loRaPayloadLength = LoRaWAN::micOffset - LoRaWAN::b0BlockLength;
    LoRaWAN::padForMicCalculation(LoRaWAN::loRaPayloadLength);
    TEST_ASSERT_EQUAL(0xB4160A07, LoRaWAN::calculateMic());

    static constexpr uint32_t clearText2Length{40};
    uint8_t clearText2[clearText2Length];
    hexAscii::hexStringToByteArray(clearText2, "6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411");        // test vector from rfc4493 : https://www.rfc-editor.org/rfc/rfc4493
    LoRaWAN::clearRawMessage();
    memcpy(LoRaWAN::rawMessage, clearText2, clearText2Length);
    LoRaWAN::micOffset         = clearText2Length;
    LoRaWAN::loRaPayloadLength = LoRaWAN::micOffset - LoRaWAN::b0BlockLength;
    LoRaWAN::padForMicCalculation(LoRaWAN::loRaPayloadLength);
    TEST_ASSERT_EQUAL(0x4767A6DF, LoRaWAN::calculateMic());

    static constexpr uint32_t clearText3Length{64};
    uint8_t clearText3[clearText3Length];
    hexAscii::hexStringToByteArray(clearText3, "6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710");        // test vector from rfc4493 : https://www.rfc-editor.org/rfc/rfc4493
    LoRaWAN::clearRawMessage();
    memcpy(LoRaWAN::rawMessage, clearText3, clearText3Length);
    LoRaWAN::micOffset         = clearText3Length;
    LoRaWAN::loRaPayloadLength = LoRaWAN::micOffset - LoRaWAN::b0BlockLength;
    LoRaWAN::padForMicCalculation(LoRaWAN::loRaPayloadLength);
    TEST_ASSERT_EQUAL(0xBFBEF051, LoRaWAN::calculateMic());
}

void test_calculateMicTx() {
    LoRaWAN::networkKey.setFromHexString("2B7E151628AED2A6ABF7158809CF4F3C");
    LoRaWAN::generateKeysK1K2();
    LoRaWAN::DevAddr          = 0x12345678;
    LoRaWAN::uplinkFrameCount = 0xFFEEDDCC;

    // Case 1 : no padding

    static constexpr uint32_t testFramePayload1Length{7};        //  this will result in 32 bytes total input into the MIC calculation : B0[16] MHDR[1] FHDR[7] FPORT[1] FRMPayload[7]
    uint8_t testFramePayload1[testFramePayload1Length]{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    LoRaWAN::clearRawMessage();
    LoRaWAN::setOffsetsAndLengthsTx(testFramePayload1Length, 0);
    TEST_ASSERT_EQUAL_UINT32(32, LoRaWAN::micOffset);
    LoRaWAN::insertHeaders(nullptr, 0, testFramePayload1Length, 8);
    LoRaWAN::insertBlockB0(linkDirection::uplink, LoRaWAN::uplinkFrameCount);
    LoRaWAN::insertPayload(testFramePayload1, testFramePayload1Length);
    LoRaWAN::padForMicCalculation(testFramePayload1Length);
    TEST_ASSERT_EQUAL_UINT32(0xffc94fb8, LoRaWAN::calculateMic());

    // B0 = 49000000000078563412ccddeeff0010
    // B0 encrypted = d25dde70ee4706c6e9ef6eacb10aa1a3
    // 2nd block =    407856341200CCDD0800010203040506 // Headers and payload
    // B0Encrypted XOR 2ndBlock = 92258844FC47CA1BE1EF6FAEB20EA4A5
    // K1 = fbeed618357133667c85e08f7236a8de
    // XORed with K1 = 69CB5E5CC936F97D9D6A8F21C0380C7B

    // encrypted = b84fc9ff7fc4b98614d4d62999186c0b
    // MIC = b84fc9ff

    // Case 2 : 8 bytes padding

    static constexpr uint32_t testFramePayload2Length{3};        //  this will result in 28 bytes total input into the MIC calculation : B0[16] MHDR[1] FHDR[7] FPORT[1] FRMPayload[3]
    uint8_t testFramePayload2[testFramePayload2Length]{0x00, 0x01, 0x02};
    LoRaWAN::clearRawMessage();
    LoRaWAN::setOffsetsAndLengthsTx(testFramePayload2Length, 0);
    TEST_ASSERT_EQUAL_UINT32(28, LoRaWAN::micOffset);
    LoRaWAN::insertHeaders(nullptr, 0, testFramePayload2Length, 8);
    LoRaWAN::insertBlockB0(linkDirection::uplink, LoRaWAN::uplinkFrameCount);
    LoRaWAN::insertPayload(testFramePayload2, testFramePayload2Length);
    LoRaWAN::padForMicCalculation(testFramePayload2Length);
    TEST_ASSERT_EQUAL_UINT32(0x35490cc4, LoRaWAN::calculateMic());

    // B0 = 49000000000078563412ccddeeff000C
    // B0 encrypted = c2c9ebb37822305c3ae0c72ab26153c5
    // 2nd block =    407856341200CCDD0800010280000000 // Headers and payload + 4 bytes padding
    // B0Encrypted XOR 2ndBlock = 82B1BD876A22FC8132E0C628326153C5
    // K2 = f7ddac306ae266ccf90bc11ee46d513b
    // XORed with K2 = 756C11B700C09A4DCBEB0736D60C02FE
    // encrypted = c40c4935798729bc7704021d86a726d2
    // MIC = c40c4935
}

void test_calculateMicRx() {
    LoRaWAN::networkKey.setFromHexString("2B7E151628AED2A6ABF7158809CF4F3C");
    LoRaWAN::generateKeysK1K2();
    LoRaWAN::DevAddr            = 0x12345678;
    LoRaWAN::downlinkFrameCount = 0xFFEEDDCC;

    // Case 1 : no padding

    static constexpr uint32_t testLoRaPayload1Length{20};
    uint8_t testLoRaPayload1[testLoRaPayload1Length]{0x60, 0x78, 0x56, 0x34, 0x12, 0x00, 0xCC, 0xDD, 0x01, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x13, 0xF1, 0x4E, 0x5E};
    LoRaWAN::clearRawMessage();
    LoRaWAN::setOffsetsAndLengthsRx(testLoRaPayload1Length);
    TEST_ASSERT_EQUAL_UINT32(32, LoRaWAN::micOffset);
    memcpy(LoRaWAN::rawMessage + LoRaWAN::macHeaderOffset, testLoRaPayload1, testLoRaPayload1Length);
    LoRaWAN::insertBlockB0(linkDirection::downlink, LoRaWAN::downlinkFrameCount);
    LoRaWAN::padForMicCalculation(testLoRaPayload1Length - LoRaWAN::micLength);
    TEST_ASSERT_EQUAL_UINT32(0x7b0df7c0, LoRaWAN::calculateMic());

    // B0 = 49000000000178563412CCDDEEFF0010
    // B0 encrypted = c9c9448fb0419fd65025d6dbe062bcea
    // 2nd block =    607856341200CCDD0100010203040506
    // B0Encrypted XOR 2ndBlock = A9B112BBA241530B5125D7D9E366B9EC

    // K1 = fbeed618357133667c85e08f7236a8de
    // XORed with K1 = 525FC4A39730606D2DA0375691501132

    // encrypted = c0f70d7bae9312863c6a870244b94fb9
    // MIC = c0f70d7b

    // Case 2 : 8 bytes padding

    static constexpr uint32_t testLoRaPayload2Length{16};
    uint8_t testLoRaPayload2[testLoRaPayload2Length]{0x60, 0x78, 0x56, 0x34, 0x12, 0x00, 0xCC, 0xDD, 0x01, 0x00, 0x01, 0x02, 0x13, 0xF1, 0x4E, 0x5E};

    LoRaWAN::clearRawMessage();
    LoRaWAN::setOffsetsAndLengthsRx(testLoRaPayload2Length);
    TEST_ASSERT_EQUAL_UINT32(28, LoRaWAN::micOffset);
    memcpy(LoRaWAN::rawMessage + LoRaWAN::macHeaderOffset, testLoRaPayload2, testLoRaPayload2Length);
    LoRaWAN::insertBlockB0(linkDirection::downlink, LoRaWAN::downlinkFrameCount);
    LoRaWAN::padForMicCalculation(testLoRaPayload2Length - LoRaWAN::micLength);
    TEST_ASSERT_EQUAL_UINT32(0x9af6a8d9, LoRaWAN::calculateMic());

    // B0 = 49000000000178563412CCDDEEFF000C
    // B0 encrypted = 27885b415bc3970c3d1d0caa39354791
    // 2nd block =    607856341200CCDD0100010280000000
    // B0Encrypted XOR 2ndBlock = 47F00D7549C35BD13C1D0DA8B9354791

    // K2 = f7ddac306ae266ccf90bc11ee46d513b
    // XORed with K2 = B02DA14523213D1DC516CCB65D5816AA
    // encrypted = d9a8f69a5a8a264c3abf7e07fbaac967
    // MIC = d9a8f69a
}

int main(int argc, char** argv) {
    HAL_Init();
    HAL_Delay(2000);
    SystemClock_Config();

    UNITY_BEGIN();
    MX_USART1_UART_Init();
    RUN_TEST(test_prepareBlockAiTx);
    RUN_TEST(test_prepareBlockAiRx);
    RUN_TEST(test_encryptPayload);
    RUN_TEST(test_decryptPayload);
    RUN_TEST(test_keyGeneration);
    RUN_TEST(test_calculateMic);
    RUN_TEST(test_calculateMicTx);
    RUN_TEST(test_calculateMicRx);
    UNITY_END();
}