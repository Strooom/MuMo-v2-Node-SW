#include <unity.h>
#include <aesblock.hpp>
#include <hexascii.hpp>
#include <lorawan.hpp>
#include <circularbuffer.hpp>
#include <lorawanevent.hpp>
#include <applicationevent.hpp>
#include <settingscollection.hpp>
#include <maccommand.hpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;
circularBuffer<loRaWanEvent, 16U> loraWanEventBuffer;

uint8_t mockSX126xDataBuffer[256];
uint8_t mockSX126xRegisters[0x1000];
uint8_t mockSX126xCommandData[256][8];

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
void tearDown(void) {}

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
    LoRaWAN::encryptPayload(LoRaWAN::applicationKey);
    const uint8_t expectedEncryptedPayload1[16]{0x4D, 0x97, 0x57, 0xC6, 0x57, 0xDB, 0xB8, 0xA7, 0xD7, 0xAF, 0x23, 0x00, 0xB6, 0xDE, 0xE5, 0xC6};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedEncryptedPayload1, LoRaWAN::rawMessage + LoRaWAN::framePayloadOffset, testClearTextLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage, LoRaWAN::framePayloadOffset);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage + LoRaWAN::micOffset, LoRaWAN::rawMessageLength - LoRaWAN::micOffset);

    // Case 2 : 32 bytes payload = 2 block

    testClearTextLength = 32;
    LoRaWAN::clearRawMessage();
    LoRaWAN::setOffsetsAndLengthsTx(testClearTextLength, 0);
    LoRaWAN::insertPayload(testClearText, testClearTextLength);
    LoRaWAN::encryptPayload(LoRaWAN::applicationKey);
    const uint8_t expectedEncryptedPayload2[32]{0x4D, 0x97, 0x57, 0xC6, 0x57, 0xDB, 0xB8, 0xA7, 0xD7, 0xAF, 0x23, 0x00, 0xB6, 0xDE, 0xE5, 0xC6, 0x83, 0x67, 0xFF, 0x84, 0x34, 0x2F, 0xA5, 0x48, 0x93, 0xE8, 0xB1, 0xC3, 0xAA, 0x23, 0x89, 0x31};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedEncryptedPayload2, LoRaWAN::rawMessage + LoRaWAN::framePayloadOffset, testClearTextLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage, LoRaWAN::framePayloadOffset);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage + LoRaWAN::micOffset, LoRaWAN::rawMessageLength - LoRaWAN::micOffset);

    // Case 3 : 40 bytes payload : padding needed to 3 blocks

    testClearTextLength = 40;
    LoRaWAN::clearRawMessage();
    LoRaWAN::setOffsetsAndLengthsTx(testClearTextLength, 0);
    LoRaWAN::insertPayload(testClearText, testClearTextLength);
    LoRaWAN::encryptPayload(LoRaWAN::applicationKey);
    const uint8_t expectedEncryptedPayload3[40]{0x4D, 0x97, 0x57, 0xC6, 0x57, 0xDB, 0xB8, 0xA7, 0xD7, 0xAF, 0x23, 0x00, 0xB6, 0xDE, 0xE5, 0xC6, 0x83, 0x67, 0xFF, 0x84, 0x34, 0x2F, 0xA5, 0x48, 0x93, 0xE8, 0xB1, 0xC3, 0xAA, 0x23, 0x89, 0x31, 0x4B, 0x2D, 0x32, 0xC0, 0x59, 0x4C, 0x67, 0x32};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedEncryptedPayload3, LoRaWAN::rawMessage + LoRaWAN::framePayloadOffset, testClearTextLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage, LoRaWAN::framePayloadOffset);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage + LoRaWAN::micOffset, LoRaWAN::rawMessageLength - LoRaWAN::micOffset);

    // Case 4 : 0 bytes payload : edge case, no encryption needed

    testClearTextLength = 0;
    LoRaWAN::clearRawMessage();
    LoRaWAN::setOffsetsAndLengthsTx(testClearTextLength, 0);
    LoRaWAN::insertPayload(testClearText, testClearTextLength);
    LoRaWAN::encryptPayload(LoRaWAN::applicationKey);
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
    LoRaWAN::decryptPayload(LoRaWAN::applicationKey);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(testClearText, LoRaWAN::rawMessage + LoRaWAN::framePayloadOffset, testCypherTextLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage, LoRaWAN::framePayloadOffset);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage + LoRaWAN::micOffset, LoRaWAN::rawMessageLength - LoRaWAN::micOffset);

    // Case 3 : 40 bytes payload

    testCypherTextLength = 40;
    uint8_t testCypherText2[40]{0xD2, 0x6F, 0xE6, 0x38, 0x37, 0xC8, 0x52, 0x64, 0xDC, 0x85, 0xB8, 0xCD, 0xC1, 0x7C, 0x00, 0x2E, 0x94, 0x4C, 0x5A, 0x69, 0x96, 0x94, 0xFE, 0x92, 0x73, 0x43, 0xEA, 0x7C, 0x6D, 0x32, 0xA3, 0xCE, 0x0A, 0xD9, 0x8B, 0x25, 0xDE, 0xEE, 0xA2, 0xEF};
    LoRaWAN::clearRawMessage();
    LoRaWAN::setOffsetsAndLengthsRx(testCypherTextLength + numberOfHeaderBytes);
    memcpy(LoRaWAN::rawMessage + LoRaWAN::framePayloadOffset, testCypherText2, testCypherTextLength);
    LoRaWAN::decryptPayload(LoRaWAN::applicationKey);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(testClearText, LoRaWAN::rawMessage + LoRaWAN::framePayloadOffset, testCypherTextLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage, LoRaWAN::framePayloadOffset);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(allZeroes, LoRaWAN::rawMessage + LoRaWAN::micOffset, LoRaWAN::rawMessageLength - LoRaWAN::micOffset);

    // Case 4 : 0 bytes payload : edge case, no decryption needed

    testCypherTextLength = 0;
    LoRaWAN::clearRawMessage();
    LoRaWAN::setOffsetsAndLengthsRx(testCypherTextLength + numberOfHeaderBytes);
    LoRaWAN::decryptPayload(LoRaWAN::applicationKey);
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
    TEST_ASSERT_EQUAL(0xbb1d6929, LoRaWAN::mic());

    static constexpr uint32_t clearText1Length{16};
    uint8_t clearText1[clearText1Length];
    hexAscii::hexStringToByteArray(clearText1, "6bc1bee22e409f96e93d7e117393172a");        // test vector from rfc4493 : https://www.rfc-editor.org/rfc/rfc4493
    LoRaWAN::clearRawMessage();
    memcpy(LoRaWAN::rawMessage, clearText1, clearText1Length);
    LoRaWAN::micOffset = clearText1Length;
    TEST_ASSERT_EQUAL(0x070A16B4, LoRaWAN::mic());

    static constexpr uint32_t clearText2Length{40};
    uint8_t clearText2[clearText2Length];
    hexAscii::hexStringToByteArray(clearText2, "6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411");        // test vector from rfc4493 : https://www.rfc-editor.org/rfc/rfc4493
    LoRaWAN::clearRawMessage();
    memcpy(LoRaWAN::rawMessage, clearText2, clearText2Length);
    LoRaWAN::micOffset = clearText2Length;
    TEST_ASSERT_EQUAL(0xDFA66747, LoRaWAN::mic());

    static constexpr uint32_t clearText3Length{64};
    uint8_t clearText3[clearText3Length];
    hexAscii::hexStringToByteArray(clearText3, "6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710");        // test vector from rfc4493 : https://www.rfc-editor.org/rfc/rfc4493
    LoRaWAN::clearRawMessage();
    memcpy(LoRaWAN::rawMessage, clearText3, clearText3Length);
    LoRaWAN::micOffset = clearText3Length;
    TEST_ASSERT_EQUAL(0x51f0bebf, LoRaWAN::mic());
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
    LoRaWAN::padForMicCalculation();
    TEST_ASSERT_EQUAL_UINT32(0x13f14e5e, LoRaWAN::mic());

    // B0 = 49000000000078563412ccddeeff0020
    // B0 encrypted = 21d081424901ece9d6db3d3c2e3a2a51
    // 2nd block =    407856341200CCDD0800010203040506 // Headers and payload
    // B0Encrypted XOR 2ndBlock = 61A8D7765B012034DEDB3C3E2D3E2F57
    // K1 = fbeed618357133667c85e08f7236a8de
    // XORed with K1 = 9A46016E6E701352A25EDCB15F088789
    // encrypted = 13f14e5ec845c3848aa4365f3eb073cd
    // MIC = 13f14e5e

    // Case 2 : 8 bytes padding

    static constexpr uint32_t testFramePayload2Length{3};        //  this will result in 28 bytes total input into the MIC calculation : B0[16] MHDR[1] FHDR[7] FPORT[1] FRMPayload[3]
    uint8_t testFramePayload2[testFramePayload2Length]{0x00, 0x01, 0x02};
    LoRaWAN::clearRawMessage();
    LoRaWAN::setOffsetsAndLengthsTx(testFramePayload2Length, 0);
    TEST_ASSERT_EQUAL_UINT32(28, LoRaWAN::micOffset);
    LoRaWAN::insertHeaders(nullptr, 0, testFramePayload2Length, 8);
    LoRaWAN::insertBlockB0(linkDirection::uplink, LoRaWAN::uplinkFrameCount);
    LoRaWAN::insertPayload(testFramePayload2, testFramePayload2Length);
    LoRaWAN::padForMicCalculation();
    TEST_ASSERT_EQUAL_UINT32(0x390449df, LoRaWAN::mic());

    // B0 = 49000000000078563412ccddeeff001C
    // B0 encrypted = 197a1d1f5f39fe5d71b81d338a837aea
    // 2nd block =    407856341200CCDD0800010280000000 // Headers and payload + 4 bytes padding
    // B0Encrypted XOR 2ndBlock = 59024B2B4D39328079B81C310A837AEA
    // K2 = f7ddac306ae266ccf90bc11ee46d513b
    // XORed with K2 = AEDFE71B27DB544C80B3DD2FEEEE2BD1
    // encrypted = 390449df1a4d3584f09cabd901c6f484
    // MIC = 390449df
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
    LoRaWAN::padForMicCalculation();
    TEST_ASSERT_EQUAL_UINT32(0x62b3e738, LoRaWAN::mic());

    // B0 = 49000000000178563412CCDDEEFF0020
    // B0 encrypted = 7ecfdf416401b063796d61b32a7e9b6c
    // 2nd block =    607856341200CCDD0100010203040506
    // B0Encrypted XOR 2ndBlock = 1EB7897576017CBE786D60B1297A9E6A

    // K1 = fbeed618357133667c85e08f7236a8de
    // XORed with K1 = E5595F6D43704FD804E8803E5B4C36B4
    // encrypted = 62b3e7381f956d85cb3ddac69cad4412
    // MIC = 62b3e738

    // Case 2 : 8 bytes padding

    static constexpr uint32_t testLoRaPayload2Length{16};
    uint8_t testLoRaPayload2[testLoRaPayload2Length]{0x60, 0x78, 0x56, 0x34, 0x12, 0x00, 0xCC, 0xDD, 0x01, 0x00, 0x01, 0x02, 0x13, 0xF1, 0x4E, 0x5E};

    LoRaWAN::clearRawMessage();
    LoRaWAN::setOffsetsAndLengthsRx(testLoRaPayload2Length);
    TEST_ASSERT_EQUAL_UINT32(28, LoRaWAN::micOffset);
    memcpy(LoRaWAN::rawMessage + LoRaWAN::macHeaderOffset, testLoRaPayload2, testLoRaPayload2Length);
    LoRaWAN::insertBlockB0(linkDirection::downlink, LoRaWAN::downlinkFrameCount);
    LoRaWAN::padForMicCalculation();
    TEST_ASSERT_EQUAL_UINT32(0x4fbbd7f7, LoRaWAN::mic());

    // B0 = 49000000000178563412CCDDEEFF001C
    // B0 encrypted = ceba936414cc4abfe4bea4adb775a00c
    // 2nd block =    607856341200CCDD0100010280000000
    // B0Encrypted XOR 2ndBlock = AEC2C55006CC8662E5BEA5AF3775A00C

    // K2 = f7ddac306ae266ccf90bc11ee46d513b
    // XORed with K2 = 591F69606C2EE0AE1CB564B1D318F137
    // encrypted = 4fbbd7f7ab1d94b434e65df6356ee5c9
    // MIC = 4fbbd7f7
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
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