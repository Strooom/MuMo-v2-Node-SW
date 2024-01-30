#include <unity.h>

#include <aeskey.hpp>

#include "Struct.h"
#include "Encrypt.h"
// #include "bytebuffer.h"
// #include "hextools.h"

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
    uint8_t result[aesKey::lengthAsBytes];
    hexAscii::hexStringToByteArray("FBEED618357133667C85E08F7236A8DE", result);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(result, LoRaWAN::keyK1.asBytes(), aesKey::lengthAsBytes);
    hexAscii::hexStringToByteArray("F7DDAC306AE266CCF90BC11EE46D513B", result);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(result, LoRaWAN::keyK2.asBytes(), aesKey::lengthAsBytes);
}

// https://www.rfc-editor.org/rfc/rfc4493#page-14
// https://www.rfc-editor.org/rfc/rfc3686#page-9

//    --------------------------------------------------
//    Example 1: len = 0
//    K              2b7e1516 28aed2a6 abf71588 09cf4f3c
//    M              <empty string>
//    AES-CMAC       bb1d6929 e9593728 7fa37d12 9b756746
//    --------------------------------------------------

//    Example 2: len = 16
//    K              2b7e1516 28aed2a6 abf71588 09cf4f3c
//    M              6bc1bee2 2e409f96 e93d7e11 7393172a
//    AES-CMAC       070a16b4 6b4d4144 f79bdd9d d04a287c
//    --------------------------------------------------

//    Example 3: len = 40
//    K              2b7e1516 28aed2a6 abf71588 09cf4f3c
//    M              6bc1bee2 2e409f96 e93d7e11 7393172a
//                   ae2d8a57 1e03ac9c 9eb76fac 45af8e51
//                   30c81c46 a35ce411
//    AES-CMAC       dfa66747 de9ae630 30ca3261 1497c827
//    --------------------------------------------------

//    Example 4: len = 64
//    K              2b7e1516 28aed2a6 abf71588 09cf4f3c
//    M              6bc1bee2 2e409f96 e93d7e11 7393172a
//                   ae2d8a57 1e03ac9c 9eb76fac 45af8e51
//                   30c81c46 a35ce411 e5fbc119 1a0a52ef
//                   f69f2445 df4f9b17 ad2b417b e66c3710
//    AES-CMAC       51f0bebf 7e3b9d92 fc497417 79363cfe
//    --------------------------------------------------

void test_calculateMic01() {
    // TODO : this test fails, as the zero payload case is causing a loop index problem.
    // It's not really that important, as for LoRaWAN, the payload will never be zero length because there is always the B0 block 
    LoRaWAN::networkKey.setFromHexString("2b7e151628aed2a6abf7158809cf4f3c");
    LoRaWAN::generateKeysK1K2();
    static constexpr uint32_t clearTextLength{0};
    uint8_t clearText[clearTextLength];
    TEST_ASSERT_EQUAL(0xbb1d6929, LoRaWAN::mic(clearText, clearTextLength));
}

void test_calculateMic02() {
    LoRaWAN::networkKey.setFromHexString("2b7e151628aed2a6abf7158809cf4f3c");
    LoRaWAN::generateKeysK1K2();
    static constexpr uint32_t clearTextLength{16};
    uint8_t clearText[clearTextLength];
    hexAscii::hexStringToByteArray("6bc1bee22e409f96e93d7e117393172a", clearText);        // test vector from rfc4493 : https://www.rfc-editor.org/rfc/rfc4493
    TEST_ASSERT_EQUAL(0x070A16B4, LoRaWAN::mic(clearText, clearTextLength));
}

void test_calculateMic03() {
    LoRaWAN::networkKey.setFromHexString("2b7e151628aed2a6abf7158809cf4f3c");
    LoRaWAN::generateKeysK1K2();
    static constexpr uint32_t clearTextLength{40};
    uint8_t clearText[clearTextLength];
    hexAscii::hexStringToByteArray("6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411", clearText);        // test vector from rfc4493 : https://www.rfc-editor.org/rfc/rfc4493
    aesBlock output;
    TEST_ASSERT_EQUAL(0xDFA66747, LoRaWAN::mic(clearText, clearTextLength));
}

void test_calculateMic04() {
    LoRaWAN::networkKey.setFromHexString("2b7e151628aed2a6abf7158809cf4f3c");
    LoRaWAN::generateKeysK1K2();
    static constexpr uint32_t clearTextLength{64};
    uint8_t clearText[clearTextLength];
    hexAscii::hexStringToByteArray("6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710", clearText);        // test vector from rfc4493 : https://www.rfc-editor.org/rfc/rfc4493
    aesBlock output;
    TEST_ASSERT_EQUAL(0x51f0bebf, LoRaWAN::mic(clearText, clearTextLength));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_keyGeneration);
    RUN_TEST(test_calculateMic01);
    RUN_TEST(test_calculateMic02);
    RUN_TEST(test_calculateMic03);
    RUN_TEST(test_calculateMic04);
    UNITY_END();
}