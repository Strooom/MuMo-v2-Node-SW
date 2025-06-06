#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>
#include <gpio.hpp>
#include <stm32wle5_aes.hpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_initialize() {
    stm32wle5_aes::initialize(aesMode::EBC);
    TEST_ASSERT_EQUAL_UINT32(0x4, AES->CR);
    stm32wle5_aes::initialize(aesMode::CBC);
    TEST_ASSERT_EQUAL_UINT32(0x24, AES->CR);
    stm32wle5_aes::initialize(aesMode::CTR);
    TEST_ASSERT_EQUAL_UINT32(0x44, AES->CR);
}

void test_enable_disable() {
    stm32wle5_aes::initialize(aesMode::EBC);
    stm32wle5_aes::enable();
    TEST_ASSERT_EQUAL_UINT32(0x5, AES->CR);
    stm32wle5_aes::disable();
    TEST_ASSERT_EQUAL_UINT32(0x4, AES->CR);
}

void test_setKey() {
    aesKey testKey;
    testKey.setFromHexString("2b7e151628aed2a6abf7158809cf4f3c");
    stm32wle5_aes::setKey(testKey);
    TEST_IGNORE_MESSAGE("For code tracing only as key-registers cannot be read back...");
}

void test_setInitVector() {
    aesBlock testData;
    testData.setFromHexString("2b7e151628aed2a6abf7158809cf4f3c");
    stm32wle5_aes::setInitializationVector(testData);
    TEST_ASSERT_EQUAL_UINT32(0x09cf4f3c, AES->IVR0);
    TEST_ASSERT_EQUAL_UINT32(0xabf71588, AES->IVR1);
    TEST_ASSERT_EQUAL_UINT32(0x28aed2a6, AES->IVR2);
    TEST_ASSERT_EQUAL_UINT32(0x2b7e1516, AES->IVR3);
}

void test_hwEncryptSingleBlock() {
    aesKey key;
    key.setFromHexString("2b7e151628aed2a6abf7158809cf4f3c");
    aesBlock inputOutput;
    inputOutput.setFromHexString("6bc1bee22e409f96e93d7e117393172a");        // test-vector 1
    inputOutput.encrypt(key);
    aesBlock expectedCypherText;
    expectedCypherText.setFromHexString("3ad77bb40d7a3660a89ecaf32466ef97");

    for (size_t i = 0; i < aesBlock::lengthInBytes; ++i) {
        TEST_ASSERT_EQUAL(expectedCypherText.getAsByte(i), inputOutput.getAsByte(i));
    }

    inputOutput.setFromHexString("ae2d8a571e03ac9c9eb76fac45af8e51");        // test-vector 2
    inputOutput.encrypt(key);
    expectedCypherText.setFromHexString("f5d3d58503b9699de785895a96fdbaaf");
    for (size_t i = 0; i < aesBlock::lengthInBytes; ++i) {
        TEST_ASSERT_EQUAL(expectedCypherText.getAsByte(i), inputOutput.getAsByte(i));
    }

    inputOutput.setFromHexString("30c81c46a35ce411e5fbc1191a0a52ef");        // test-vector 3
    inputOutput.encrypt(key);
    expectedCypherText.setFromHexString("43b1cd7f598ece23881b00e3ed030688");
    for (size_t i = 0; i < aesBlock::lengthInBytes; ++i) {
        TEST_ASSERT_EQUAL(expectedCypherText.getAsByte(i), inputOutput.getAsByte(i));
    }

    inputOutput.setFromHexString("f69f2445df4f9b17ad2b417be66c3710");        // test-vector 4
    inputOutput.encrypt(key);
    expectedCypherText.setFromHexString("7b0c785e27e8ad3f8223207104725dd4");
    for (size_t i = 0; i < aesBlock::lengthInBytes; ++i) {
        TEST_ASSERT_EQUAL(expectedCypherText.getAsByte(i), inputOutput.getAsByte(i));
    }
}

void test_hwEncryptLoRaWANPayload() {
    aesKey key;
    key.setFromHexString("2b7e151628aed2a6abf7158809cf4f3c");
    aesBlock A1;
    A1.setFromHexString("01000000000078563412ccddeeff0001");
    aesBlock input;
    input.setFromHexString("00000000000000000000000000000000");
    aesBlock output;
    aesBlock toBe;

    stm32wle5_aes::initialize(aesMode::CTR);
    stm32wle5_aes::setKey(key);
    stm32wle5_aes::setInitializationVector(A1);
    stm32wle5_aes::enable();

    // first block

    stm32wle5_aes::write(input);
    while (!stm32wle5_aes::isComputationComplete()) {
    }
    stm32wle5_aes::clearComputationComplete();
    stm32wle5_aes::read(output);
    toBe.setFromHexString("4d9655c553debea0dfa6290bbad3ebc9");
    for (size_t i = 0; i < aesBlock::lengthInBytes; ++i) {
        TEST_ASSERT_EQUAL(output.getAsByte(i), toBe.getAsByte(i));
    }

    // second block

    stm32wle5_aes::write(input);
    while (!stm32wle5_aes::isComputationComplete()) {
    }
    stm32wle5_aes::clearComputationComplete();
    stm32wle5_aes::read(output);
    toBe.setFromHexString("9376ed97203ab35f8bf1abd8b63e972e");
    for (size_t i = 0; i < aesBlock::lengthInBytes; ++i) {
        TEST_ASSERT_EQUAL(output.getAsByte(i), toBe.getAsByte(i));
    }

    // third block

    stm32wle5_aes::write(input);
    while (!stm32wle5_aes::isComputationComplete()) {
    }
    stm32wle5_aes::clearComputationComplete();
    stm32wle5_aes::read(output);
    toBe.setFromHexString("6b0c10e37d694115c32d4f7a807db808");
    for (size_t i = 0; i < aesBlock::lengthInBytes; ++i) {
        TEST_ASSERT_EQUAL(output.getAsByte(i), toBe.getAsByte(i));
    }
}

void test_hwCalculateMic() {
    aesKey key;
    key.setFromHexString("2b7e151628aed2a6abf7158809cf4f3c");
    aesBlock allZeroes;
    allZeroes.setFromHexString("00000000000000000000000000000000");
    aesBlock output;
    aesBlock toBe;

    stm32wle5_aes::initialize(aesMode::CBC);
    stm32wle5_aes::setKey(key);
    stm32wle5_aes::setInitializationVector(allZeroes);
    stm32wle5_aes::enable();

    // first block

    stm32wle5_aes::write(allZeroes);
    while (!stm32wle5_aes::isComputationComplete()) {
    }
    stm32wle5_aes::clearComputationComplete();
    stm32wle5_aes::read(output);
    toBe.setFromHexString("7df76b0c1ab899b33e42f047b91b546f");
    for (size_t i = 0; i < aesBlock::lengthInBytes; ++i) {
        TEST_ASSERT_EQUAL(output.getAsByte(i), toBe.getAsByte(i));
    }

    // second block

    stm32wle5_aes::write(allZeroes);
    while (!stm32wle5_aes::isComputationComplete()) {
    }
    stm32wle5_aes::clearComputationComplete();
    stm32wle5_aes::read(output);
    toBe.setFromHexString("a9dcf5aa138056e259e7be57958e72d8");
    for (size_t i = 0; i < aesBlock::lengthInBytes; ++i) {
        TEST_ASSERT_EQUAL(output.getAsByte(i), toBe.getAsByte(i));
    }

    // third block

    stm32wle5_aes::write(allZeroes);
    while (!stm32wle5_aes::isComputationComplete()) {
    }
    stm32wle5_aes::clearComputationComplete();
    stm32wle5_aes::read(output);
    toBe.setFromHexString("626caecce6b25a25524cb32b7ec1374e");
    for (size_t i = 0; i < aesBlock::lengthInBytes; ++i) {
        TEST_ASSERT_EQUAL(output.getAsByte(i), toBe.getAsByte(i));
    }
}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(2000);        // required for testing framework to connect
    SystemClock_Config();

    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_enable_disable);
    RUN_TEST(test_setKey);
    RUN_TEST(test_setInitVector);
    RUN_TEST(test_hwEncryptSingleBlock);
    RUN_TEST(test_hwEncryptLoRaWANPayload);
    RUN_TEST(test_hwCalculateMic);
    UNITY_END();
}