#include <unity.h>
#include "block.h"
#include "aeskey.h"
#include "Struct.h"
#include "Encrypt.h"
#include "bytebuffer.h"
#include "hextools.h"
#include "AES-128.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_blockSet() {
    block aBlock;
    uint8_t expectedValue[block::length]{0U};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedValue, aBlock.rawData, 16);

    uint8_t expectedValue2[block::length]{0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U, 11U, 12U, 13U, 14U, 15U};
    aBlock.setFromBinaryData(expectedValue2);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedValue2, aBlock.rawData, 16);

    uint8_t expectedValue3[block::length]{15U, 14U, 13U, 12U, 11U, 10U, 9U, 8U, 7U, 6U, 5U, 4U, 3U, 2U, 1U, 0U};
    aBlock.setFromHexString("0F0E0D0C0B0A09080706050403020100");
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedValue3, aBlock.rawData, 16);
}

void test_blockToString() {
    block aBlock;
    char destinationData[33]{0};
    TEST_ASSERT_EQUAL_STRING(destinationData, "");
    uint8_t sourceData[block::length]{0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U, 11U, 12U, 13U, 14U, 15U};
    aBlock.setFromBinaryData(sourceData);
    aBlock.toString(destinationData);
    TEST_ASSERT_EQUAL_STRING("000102030405060708090A0B0C0D0E0F", destinationData);
}

void test_xorBlock() {
    block blockOne;
    block blockTwo;
    blockOne.setFromHexString("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
    blockTwo.setFromHexString("55555555555555555555555555555555");
    blockOne.xorBlock(blockTwo);
    uint8_t expectedValue[block::length]{
        0xAA,
        0xAA,
        0xAA,
        0xAA,
        0xAA,
        0xAA,
        0xAA,
        0xAA,
        0xAA,
        0xAA,
        0xAA,
        0xAA,
        0xAA,
        0xAA,
        0xAA,
        0xAA
    };
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedValue, blockOne.rawData, 16);
}

void test_substituteBytes() {
    TEST_IGNORE_MESSAGE("to be implemented");
}

void test_addRoundKey() {
    TEST_IGNORE_MESSAGE("to be implemented");
}

void test_shiftRows() {
    TEST_IGNORE_MESSAGE("to be implemented");
}

void test_mixColumns() {
    TEST_IGNORE_MESSAGE("to be implemented");
}

void test_keyExpansion() {
    TEST_IGNORE_MESSAGE("to be implemented");
}

void test_keyGeneration() {
    TEST_IGNORE_MESSAGE("to be implemented");
}

// what tests do we need?
// key expansion / generation keys
// 1. encrypt a block of data
// 2. calculate MIC over some (payload) data

void test_encryptBlock0() {
    aesKey theKey;
    theKey.setFromASCII("2B7E151628AED2A6ABF7158809CF4F3C");

    unsigned char input[16];                                                  // cleartext
    hexStringToBinaryArray("6BC1BEE22E409F96E93D7E117393172A", input);        //

    AES_Encrypt(input, theKey.asUnsignedChar());        // Encryption

    char outputAsHexString[33];
    binaryArrayToHexString(input, 16, outputAsHexString);
    TEST_ASSERT_EQUAL_STRING("3AD77BB40D7A3660A89ECAF32466EF97", outputAsHexString);
}

void test_encryptBlock1() {
    aesKey theKey;
    theKey.setFromASCII("2B7E151628AED2A6ABF7158809CF4F3C");

    unsigned char plainText[16];
    hexStringToBinaryArray("AE2D8A571E03AC9C9EB76FAC45AF8E51", plainText);

    AES_Encrypt(plainText, theKey.asUnsignedChar());

    char cipherAsHexString[33];
    binaryArrayToHexString(plainText, 16, cipherAsHexString);

    TEST_ASSERT_EQUAL_STRING("F5D3D58503B9699DE785895A96FDBAAF", cipherAsHexString);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_blockSet);
    RUN_TEST(test_blockToString);

    RUN_TEST(test_xorBlock);
    RUN_TEST(test_mixColumns);
    RUN_TEST(test_shiftRows);
    RUN_TEST(test_addRoundKey);
    RUN_TEST(test_substituteBytes);
    RUN_TEST(test_keyExpansion);
    RUN_TEST(test_keyGeneration);

    RUN_TEST(test_encryptBlock0);
    RUN_TEST(test_encryptBlock1);
    UNITY_END();
}