#include <unity.h>

#include "aeskey.h"
#include "Struct.h"
#include "Encrypt.h"
#include "bytebuffer.h"
#include "hextools.h"

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

void test_keyGeneration() {
    aesKey theKey;
    theKey.setFromASCII("2b7e151628aed2a6abf7158809cf4f3c");        // test key from rfc4493 : https://www.rfc-editor.org/rfc/rfc4493

    unsigned char K1[16] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    unsigned char K2[16] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    Generate_Keys(theKey.asUnsignedChar(), K1, K2);

    char keyAsHexString[33];
    binaryArrayToHexString(K1, 16, keyAsHexString);
    TEST_ASSERT_EQUAL_STRING("FBEED618357133667C85E08F7236A8DE", keyAsHexString);
    binaryArrayToHexString(K2, 16, keyAsHexString);
    TEST_ASSERT_EQUAL_STRING("F7DDAC306AE266CCF90BC11EE46D513B", keyAsHexString);
}

void test_calculateMic0() {
    aesKey theKey;
    theKey.setFromASCII("2b7e151628aed2a6abf7158809cf4f3c");        // test key from rfc4493 : https://www.rfc-editor.org/rfc/rfc4493

    byteBuffer clearText;
    clearText.clear();        // test vector from rfc4493 : https://www.rfc-editor.org/rfc/rfc4493
    sBuffer sourceData;
    sourceData.Data    = clearText.buffer;        // copy to an sBuffer as this is what the 3rd party library expects
    sourceData.Counter = clearText.length;        // idem

    unsigned char micAsBinaryArray[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    Calculate_MIC2(&sourceData, theKey.asUnsignedChar(), micAsBinaryArray);
    char micAsHexString[33];
    binaryArrayToHexString(micAsBinaryArray, 16, micAsHexString);
    TEST_ASSERT_EQUAL_STRING("BB1D6929E95937287FA37D129B756746", micAsHexString);
}

void test_calculateMic1() {
    aesKey theKey;
    theKey.setFromASCII("2b7e151628aed2a6abf7158809cf4f3c");        // test key from rfc4493 : https://www.rfc-editor.org/rfc/rfc4493

    byteBuffer clearText;
    clearText.setFromHexAscii("6bc1bee22e409f96e93d7e117393172a");        // test vector from rfc4493 : https://www.rfc-editor.org/rfc/rfc4493
    sBuffer sourceData;
    sourceData.Data    = clearText.buffer;        // copy to an sBuffer as this is what the 3rd party library expects
    sourceData.Counter = clearText.length;        // idem

    unsigned char micAsBinaryArray[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    Calculate_MIC2(&sourceData, theKey.asUnsignedChar(), micAsBinaryArray);
    char micAsHexString[33];
    binaryArrayToHexString(micAsBinaryArray, 16, micAsHexString);
    TEST_ASSERT_EQUAL_STRING("070A16B46B4D4144F79BDD9DD04A287C", micAsHexString);
}

void test_calculateMic2() {
    aesKey theKey;
    theKey.setFromASCII("2b7e151628aed2a6abf7158809cf4f3c");        // test key from rfc4493 : https://www.rfc-editor.org/rfc/rfc4493

    byteBuffer clearText;
    clearText.setFromHexAscii("6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411");        // test vector from rfc4493 : https://www.rfc-editor.org/rfc/rfc4493
    sBuffer sourceData;
    sourceData.Data    = clearText.buffer;        // copy to an sBuffer as this is what the 3rd party library expects
    sourceData.Counter = clearText.length;        // idem

    unsigned char micAsBinaryArray[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    Calculate_MIC2(&sourceData, theKey.asUnsignedChar(), micAsBinaryArray);
    char micAsHexString[33];
    binaryArrayToHexString(micAsBinaryArray, 16, micAsHexString);
    TEST_ASSERT_EQUAL_STRING("DFA66747DE9AE63030CA32611497C827", micAsHexString);
}

void test_calculateMic3() {
    aesKey theKey;
    theKey.setFromASCII("2b7e151628aed2a6abf7158809cf4f3c");        // test key from rfc4493 : https://www.rfc-editor.org/rfc/rfc4493

    byteBuffer clearText;
    clearText.setFromHexAscii("6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710");        // test vector from rfc4493 : https://www.rfc-editor.org/rfc/rfc4493
    sBuffer sourceData;
    sourceData.Data    = clearText.buffer;        // copy to an sBuffer as this is what the 3rd party library expects
    sourceData.Counter = clearText.length;        // idem

    unsigned char micAsBinaryArray[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    Calculate_MIC2(&sourceData, theKey.asUnsignedChar(), micAsBinaryArray);
    char micAsHexString[33];
    binaryArrayToHexString(micAsBinaryArray, 16, micAsHexString);
    TEST_ASSERT_EQUAL_STRING("51F0BEBF7E3B9D92FC49741779363CFE", micAsHexString);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_keyGeneration);
    RUN_TEST(test_calculateMic0);
    RUN_TEST(test_calculateMic1);
    RUN_TEST(test_calculateMic2);
    RUN_TEST(test_calculateMic3);
    UNITY_END();
}