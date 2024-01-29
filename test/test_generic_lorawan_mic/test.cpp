#include <unity.h>
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

// https://www.rfc-editor.org/rfc/rfc4493#page-14
// https://www.rfc-editor.org/rfc/rfc3686#page-9

//    --------------------------------------------------
//    Subkey Generation
//    K              2b7e1516 28aed2a6 abf71588 09cf4f3c
//    AES-128(key,0) 7df76b0c 1ab899b3 3e42f047 b91b546f
//    K1             fbeed618 35713366 7c85e08f 7236a8de
//    K2             f7ddac30 6ae266cc f90bc11e e46d513b
//    --------------------------------------------------

//    --------------------------------------------------
//    Example 1: len = 0
//    M              <empty string>
//    AES-CMAC       bb1d6929 e9593728 7fa37d12 9b756746
//    --------------------------------------------------

//    Example 2: len = 16
//    M              6bc1bee2 2e409f96 e93d7e11 7393172a
//    AES-CMAC       070a16b4 6b4d4144 f79bdd9d d04a287c
//    --------------------------------------------------

//    Example 3: len = 40
//    M              6bc1bee2 2e409f96 e93d7e11 7393172a
//                   ae2d8a57 1e03ac9c 9eb76fac 45af8e51
//                   30c81c46 a35ce411
//    AES-CMAC       dfa66747 de9ae630 30ca3261 1497c827
//    --------------------------------------------------

//    Example 4: len = 64
//    M              6bc1bee2 2e409f96 e93d7e11 7393172a
//                   ae2d8a57 1e03ac9c 9eb76fac 45af8e51
//                   30c81c46 a35ce411 e5fbc119 1a0a52ef
//                   f69f2445 df4f9b17 ad2b417b e66c3710
//    AES-CMAC       51f0bebf 7e3b9d92 fc497417 79363cfe
//    --------------------------------------------------

void setUp(void) {
}
void tearDown(void) {}

void test_keyGeneration() {
    LoRaWAN::networkKey.setFromHexString("2b7e151628aed2a6abf7158809cf4f3c");
    LoRaWAN::generateKeysK1K2();
    uint8_t result[aesKey::lengthAsBytes];
    hexAscii::hexStringToByteArray("FBEED618357133667C85E08F7236A8DE", result);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(result, LoRaWAN::keyK1.asBytes(), aesKey::lengthAsBytes);
    hexAscii::hexStringToByteArray("F7DDAC306AE266CCF90BC11EE46D513B", result);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(result, LoRaWAN::keyK2.asBytes(), aesKey::lengthAsBytes);
}

// void test_keyGeneration2() {
//     aesKey theKey;
//     uint8_t keyAsBinaryArray[16];
//     hexAscii::hexStringToByteArray("2b7e151628aed2a6abf7158809cf4f3c", keyAsBinaryArray);        // test key from rfc4493 : https://www.rfc-editor.org/rfc/rfc4493
//     theKey.setFromByteArray(keyAsBinaryArray);

//     aesKey keyK1;
//     unsigned char K1[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//     keyK1.setFromByteArray(K1);
//     aesKey keyK2;
//     unsigned char K2[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//     keyK2.setFromByteArray(K2);

//     Generate_Keys(theKey, keyK1, keyK2);

//     char keyAsHexString[33];
//     binaryArrayToHexString(keyK1.asBytes(), 16, keyAsHexString);
//     TEST_ASSERT_EQUAL_STRING("FBEED618357133667C85E08F7236A8DE", keyAsHexString);
//     binaryArrayToHexString(keyK2.asBytes(), 16, keyAsHexString);
//     TEST_ASSERT_EQUAL_STRING("F7DDAC306AE266CCF90BC11EE46D513B", keyAsHexString);
// }

void test_calculateMic01() {
    LoRaWAN::networkKey.setFromHexString("2b7e151628aed2a6abf7158809cf4f3c");
    LoRaWAN::generateKeysK1K2();

    static constexpr uint32_t clearTextLength{16};
    uint8_t clearText[clearTextLength];
    hexAscii::hexStringToByteArray("6bc1bee22e409f96e93d7e117393172a", clearText);        // test vector from rfc4493 : https://www.rfc-editor.org/rfc/rfc4493

    uint32_t mic = LoRaWAN::calculateMic(clearText, clearTextLength);
    TEST_ASSERT_EQUAL(0x070A16B4, mic);
}

void test_calculateMic02() {
    LoRaWAN::networkKey.setFromHexString("2b7e151628aed2a6abf7158809cf4f3c");
    LoRaWAN::generateKeysK1K2();

    static constexpr uint32_t clearTextLength{40};
    uint8_t clearText[clearTextLength];
    hexAscii::hexStringToByteArray("6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411", clearText);        // test vector from rfc4493 : https://www.rfc-editor.org/rfc/rfc4493

    uint32_t mic = LoRaWAN::calculateMic(clearText, clearTextLength);
    TEST_ASSERT_EQUAL(0xDFA66747, mic);
}

// void test_isValidMic1() {
//     static constexpr uint32_t testLoraPayloadLength{0x2B};
//     uint8_t rxTestMessage[testLoraPayloadLength] = {0x60, 0x92, 0x3B, 0x0B, 0x26, 0x00, 0x3B, 0x00, 0x00, 0x11, 0x7C, 0xD9, 0xC4, 0xD5, 0x27, 0xA1, 0x78, 0xBD, 0x07, 0x88, 0x8B, 0x67, 0x52, 0xBC, 0xE4, 0x47, 0x26, 0xF9, 0x2B, 0x62, 0x89, 0xC3, 0x06, 0x2F, 0x69, 0x96, 0x4D, 0xD9, 0x18, 0x22, 0x58, 0x5F, 0x55};
//     memcpy(LoRaWAN::rawMessage + LoRaWAN::b0BlockLength, rxTestMessage, testLoraPayloadLength);
//     LoRaWAN::setOffsetsAndLengthsRx(testLoraPayloadLength);
//     LoRaWAN::DevAddr.asUint32 = 0x260B3B92;
//     LoRaWAN::downlinkFrameCount.set(0x0000'003B);
//     TEST_ASSERT_TRUE(LoRaWAN::isValidMic());
// }

// void test_isValidMic2() {
//     static constexpr uint32_t testLoraPayloadLength{15};
//     uint8_t rxTestMessage[testLoraPayloadLength] = {0x60, 0x51, 0xAE, 0x0B, 0x26, 0x03, 0x01, 0x00, 0x02, 0x11, 0x02, 0x65, 0xF4, 0x53, 0xB5};
//     memcpy(LoRaWAN::rawMessage + LoRaWAN::b0BlockLength, rxTestMessage, testLoraPayloadLength);
//     LoRaWAN::setOffsetsAndLengthsRx(testLoraPayloadLength);
//     LoRaWAN::downlinkFrameCount.set(0x0000'003B);
//     TEST_ASSERT_TRUE(LoRaWAN::isValidMic());
// }

int main(int argc, char **argv) {
#ifndef generic
// Here we could setup the STM32 for target unit testing
#endif
    UNITY_BEGIN();
    RUN_TEST(test_keyGeneration);
    RUN_TEST(test_calculateMic01);
    RUN_TEST(test_calculateMic02);
    UNITY_END();
}