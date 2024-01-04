#include <unity.h>
#include "aesblock.h"
#include "aeskey.h"

void setUp(void) {}
void tearDown(void) {}

void test_initialize() {
    aesBlock aBlock;
    uint8_t expectedBytes[aesKey::lengthAsBytes]{};
    uint32_t expectedWords[aesKey::lengthAsWords]{};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBytes, aBlock.asBytes(), aesKey::lengthAsBytes);
    TEST_ASSERT_EQUAL_UINT32_ARRAY(expectedWords, aBlock.asWords(), aesKey::lengthAsWords);
}

void test_setFromBytes() {
    aesBlock aBlock;
    uint8_t expectedBytes[aesKey::lengthAsBytes]{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    uint32_t expectedWords[aesKey::lengthAsWords]{0x03020100, 0x07060504, 0x0B0A0908, 0x0F0E0D0C};
    aBlock.set(expectedBytes);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBytes, aBlock.asBytes(), aesKey::lengthAsBytes);
    TEST_ASSERT_EQUAL_UINT32_ARRAY(expectedWords, aBlock.asWords(), aesKey::lengthAsWords);
}

void test_setAsArray() {
    aesBlock aBlock;
    aBlock[0]  = 0x00;
    aBlock[1]  = 0x11;
    aBlock[2]  = 0x22;
    aBlock[3]  = 0x33;
    aBlock[4]  = 0x44;
    aBlock[5]  = 0x55;
    aBlock[6]  = 0x66;
    aBlock[7]  = 0x77;
    aBlock[8]  = 0x88;
    aBlock[9]  = 0x99;
    aBlock[10] = 0xAA;
    aBlock[11] = 0xBB;
    aBlock[12] = 0xCC;
    aBlock[13] = 0xDD;
    aBlock[14] = 0xEE;
    aBlock[15] = 0xFF;

    TEST_ASSERT_EQUAL_UINT8(aBlock[0], 0x00);
    TEST_ASSERT_EQUAL_UINT8(aBlock[1], 0x11);
    TEST_ASSERT_EQUAL_UINT8(aBlock[2], 0x22);
    TEST_ASSERT_EQUAL_UINT8(aBlock[3], 0x33);
    TEST_ASSERT_EQUAL_UINT8(aBlock[4], 0x44);
    TEST_ASSERT_EQUAL_UINT8(aBlock[5], 0x55);
    TEST_ASSERT_EQUAL_UINT8(aBlock[6], 0x66);
    TEST_ASSERT_EQUAL_UINT8(aBlock[7], 0x77);
    TEST_ASSERT_EQUAL_UINT8(aBlock[8], 0x88);
    TEST_ASSERT_EQUAL_UINT8(aBlock[9], 0x99);
    TEST_ASSERT_EQUAL_UINT8(aBlock[10], 0xAA);
    TEST_ASSERT_EQUAL_UINT8(aBlock[11], 0xBB);
    TEST_ASSERT_EQUAL_UINT8(aBlock[12], 0xCC);
    TEST_ASSERT_EQUAL_UINT8(aBlock[13], 0xDD);
    TEST_ASSERT_EQUAL_UINT8(aBlock[14], 0xEE);
    TEST_ASSERT_EQUAL_UINT8(aBlock[15], 0xFF);

    uint8_t expectedBytes[aesKey::lengthAsBytes]{0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBytes, aBlock.asBytes(), 16);


}

void test_encrypt() {
    aesKey aKey;
    uint8_t keyAsBytes[16]{0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};
    aKey.set(keyAsBytes);

    aesBlock aBlock;
    uint8_t clearText[16]{0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96, 0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A};        // common NIST test vector plaintext 6bc1bee22e409f96e93d7e117393172a
    aBlock.set(clearText);

    aBlock.encrypt(aKey);

    uint8_t expectedCypherText[16]{0x3A, 0xD7, 0x7B, 0xB4, 0x0D, 0x7A, 0x36, 0x60, 0xA8, 0x9E, 0xCA, 0xF3, 0x24, 0x66, 0xEF, 0x97};        // common NIST test vector ciphertext 3ad77bb40d7a3660a89ecaf32466ef97
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedCypherText, aBlock.asBytes(), 16);
}

void test_encrypt_step_by_step() {
    aesKey aKey;
    uint8_t keyAsBytes[16]{0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};        // common NIST test vector key
    aKey.set(keyAsBytes);

    aesBlock aBlock;
    uint8_t clearText[16]{0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96, 0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A};        // common NIST test vector plaintext
    aBlock.set(clearText);

    // Step 1 XOR plaintext with key
    aBlock.XOR(aKey.asBytes());
    uint8_t expectedOutput0[16]{0x40, 0xbf, 0xab, 0xf4, 0x06, 0xee, 0x4d, 0x30, 0x42, 0xca, 0x6b, 0x99, 0x7a, 0x5c, 0x58, 0x16};        // follow along on https://www.cryptool.org/en/cto/aes-step-by-step
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedOutput0, aBlock.asBytes(), 16);

    // Step 2 substitute bytes
    aBlock.substituteBytes();
    uint8_t expectedOutput1[16]{0x09, 0x08, 0x62, 0xbf, 0x6f, 0x28, 0xe3, 0x04, 0x2c, 0x74, 0x7f, 0xee, 0xda, 0x4a, 0x6a, 0x47};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedOutput1, aBlock.asBytes(), 16);

    // Step 3 shift rows / permutation
    aBlock.shiftRows();
    uint8_t expectedOutput2[16]{0x09, 0x28, 0x7f, 0x47, 0x6f, 0x74, 0x6a, 0xbf, 0x2c, 0x4a, 0x62, 0x04, 0xda, 0x08, 0xe3, 0xee};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedOutput2, aBlock.asBytes(), 16);

    // Step 4 mix columns
    aBlock.mixColumns();
    uint8_t expectedOutput3[16]{0x52, 0x9f, 0x16, 0xc2, 0x97, 0x86, 0x15, 0xca, 0xe0, 0x1a, 0xae, 0x54, 0xba, 0x1a, 0x26, 0x59};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedOutput3, aBlock.asBytes(), 16);

    // Step 5 XOR with key
    aBlock.XOR(aKey.expandedKey + 16);
    uint8_t expectedOutput4[16]{0xf2, 0x65, 0xe8, 0xd5, 0x1f, 0xd2, 0x39, 0x7b, 0xc3, 0xb9, 0x97, 0x6d, 0x90, 0x76, 0x50, 0x5c};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedOutput4, aBlock.asBytes(), 16);
}

// Tests the correct mapping of a 16 byte vector to a 4x4 matrix in the way AES State expects it, in both directions
void testMatrixToVectorToMatrix() {
    uint8_t vectorIn[16]{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    uint8_t matrixOut[4][4];
    aesBlock::vectorToMatrix(vectorIn, matrixOut);
    TEST_ASSERT_EQUAL_UINT8(0x00, matrixOut[0][0]);
    TEST_ASSERT_EQUAL_UINT8(0x01, matrixOut[1][0]);
    TEST_ASSERT_EQUAL_UINT8(0x02, matrixOut[2][0]);
    TEST_ASSERT_EQUAL_UINT8(0x03, matrixOut[3][0]);

    TEST_ASSERT_EQUAL_UINT8(0x04, matrixOut[0][1]);
    TEST_ASSERT_EQUAL_UINT8(0x05, matrixOut[1][1]);
    TEST_ASSERT_EQUAL_UINT8(0x06, matrixOut[2][1]);
    TEST_ASSERT_EQUAL_UINT8(0x07, matrixOut[3][1]);

    TEST_ASSERT_EQUAL_UINT8(0x08, matrixOut[0][2]);
    TEST_ASSERT_EQUAL_UINT8(0x09, matrixOut[1][2]);
    TEST_ASSERT_EQUAL_UINT8(0x0A, matrixOut[2][2]);
    TEST_ASSERT_EQUAL_UINT8(0x0B, matrixOut[3][2]);

    TEST_ASSERT_EQUAL_UINT8(0x0C, matrixOut[0][3]);
    TEST_ASSERT_EQUAL_UINT8(0x0D, matrixOut[1][3]);
    TEST_ASSERT_EQUAL_UINT8(0x0E, matrixOut[2][3]);
    TEST_ASSERT_EQUAL_UINT8(0x0F, matrixOut[3][3]);

    uint8_t vectorOut[16];
    aesBlock::matrixToVector(matrixOut, vectorOut);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(vectorIn, vectorOut, 16);
}

void testBytesToWordsToBytes() {
    uint8_t bytesIn[16]{0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00};
    uint32_t wordsOut[4];
    aesBlock::bytesToWords(bytesIn, wordsOut);
    TEST_ASSERT_EQUAL_UINT32(0xFFEEDDCC, wordsOut[0]);
    TEST_ASSERT_EQUAL_UINT32(0xBBAA9988, wordsOut[1]);
    TEST_ASSERT_EQUAL_UINT32(0x77665544, wordsOut[2]);
    TEST_ASSERT_EQUAL_UINT32(0x33221100, wordsOut[3]);

    uint8_t bytesOut[16];
    aesBlock::wordsToBytes(wordsOut, bytesOut);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(bytesIn, bytesOut, 16);
}

void test_XOR() {
    aesBlock aBlock;
    uint8_t inputBytesBlock[16]{0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96, 0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A};        // Using testvectors from https://csrc.nist.gov/projects/cryptographic-algorithm-validation-program/block-ciphers#AES
    aBlock.set(inputBytesBlock);

    aesKey theKey;
    uint8_t inputBytesKey[16]{0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};        // Using testvectors from https://csrc.nist.gov/projects/cryptographic-algorithm-validation-program/block-ciphers#AES
    theKey.set(inputBytesKey);

    aBlock.XOR(theKey.asBytes());

    uint8_t expectedBytes[16]{0x40, 0xbf, 0xab, 0xf4, 0x06, 0xee, 0x4d, 0x30, 0x42, 0xca, 0x6b, 0x99, 0x7a, 0x5c, 0x58, 0x16};        // verified from https://www.cryptool.org/en/cto/aes-step-by-step
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBytes, aBlock.asBytes(), 16);
}

void test_substituteBytes() {
    // Using testvectors from https://csrc.nist.gov/projects/cryptographic-algorithm-validation-program/block-ciphers#AES
    // verified from https://www.cryptool.org/en/cto/aes-step-by-step
    aesBlock aBlock;
    uint8_t inputBytes[16]{0x40, 0xbf, 0xab, 0xf4, 0x06, 0xee, 0x4d, 0x30, 0x42, 0xca, 0x6b, 0x99, 0x7a, 0x5c, 0x58, 0x16};
    aBlock.set(inputBytes);
    aBlock.substituteBytes();
    uint8_t bytesOut[16]{0x09, 0x08, 0x62, 0xbf, 0x6f, 0x28, 0xe3, 0x04, 0x2c, 0x74, 0x7f, 0xee, 0xda, 0x4a, 0x6a, 0x47};

    TEST_ASSERT_EQUAL_UINT8_ARRAY(bytesOut, aBlock.asBytes(), 16);
}

void test_shiftRows() {
    // Using testvectors from https://csrc.nist.gov/projects/cryptographic-algorithm-validation-program/block-ciphers#AES
    // verified from https://www.cryptool.org/en/cto/aes-step-by-step, where it is called permutation
    aesBlock aBlock;
    uint8_t inputBytes[16]{0x09, 0x08, 0x62, 0xbf, 0x6f, 0x28, 0xe3, 0x04, 0x2c, 0x74, 0x7f, 0xee, 0xda, 0x4a, 0x6a, 0x47};
    aBlock.set(inputBytes);
    aBlock.shiftRows();
    uint8_t bytesOut[16]{0x09, 0x28, 0x7f, 0x47, 0x6f, 0x74, 0x6a, 0xbf, 0x2c, 0x4a, 0x62, 0x04, 0xda, 0x08, 0xe3, 0xee};

    TEST_ASSERT_EQUAL_UINT8_ARRAY(bytesOut, aBlock.asBytes(), 16);
}

void test_mixColumns() {
    aesBlock aBlock;
    uint8_t inputBytes[16]{0x09, 0x28, 0x7f, 0x47, 0x6f, 0x74, 0x6a, 0xbf, 0x2c, 0x4a, 0x62, 0x04, 0xda, 0x08, 0xe3, 0xee};
    aBlock.set(inputBytes);
    aBlock.mixColumns();
    uint8_t expectedOutput[16]{0x52, 0x9f, 0x16, 0xc2, 0x97, 0x86, 0x15, 0xca, 0xe0, 0x1a, 0xae, 0x54, 0xba, 0x1a, 0x26, 0x59};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedOutput, aBlock.asBytes(), 16);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_setFromBytes);
    RUN_TEST(test_setAsArray);
    RUN_TEST(testMatrixToVectorToMatrix);
    RUN_TEST(testBytesToWordsToBytes);

    RUN_TEST(test_XOR);
    RUN_TEST(test_substituteBytes);
    RUN_TEST(test_shiftRows);
    RUN_TEST(test_mixColumns);

    RUN_TEST(test_encrypt_step_by_step);
    RUN_TEST(test_encrypt);
    UNITY_END();
}