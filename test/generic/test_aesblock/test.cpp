#include <unity.h>
#include <aesblock.hpp>
#include <aeskey.hpp>

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_initialize() {
    aesBlock testBlock;
    uint8_t expectedBytes[aesKey::lengthInBytes]{};
    uint32_t expectedWords[aesKey::lengthInWords]{};
    for (uint32_t index = 0; index < aesBlock::lengthInBytes; ++index) {
        TEST_ASSERT_EQUAL_UINT8(expectedBytes[index], testBlock.getAsByte(index));
    }
    for (uint32_t index = 0; index < aesBlock::lengthInWords; ++index) {
        TEST_ASSERT_EQUAL_UINT32(expectedWords[index], testBlock.getAsWord(index));
    }
}

void test_setFromByteArray() {
    aesBlock testBlock;
    uint8_t expectedBytes[aesKey::lengthInBytes]{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    uint32_t expectedWords[aesKey::lengthInWords]{0x03020100, 0x07060504, 0x0B0A0908, 0x0F0E0D0C};
    testBlock.setFromByteArray(expectedBytes);
    for (uint32_t index = 0; index < aesBlock::lengthInBytes; ++index) {
        TEST_ASSERT_EQUAL_UINT8(expectedBytes[index], testBlock.getAsByte(index));
    }
    for (uint32_t index = 0; index < aesBlock::lengthInWords; ++index) {
        TEST_ASSERT_EQUAL_UINT32(expectedWords[index], testBlock.getAsWord(index));
    }
}

// void test_setFromWordArray() {
//     aesBlock testBlock;
//     uint8_t expectedBytes[aesKey::lengthInBytes]{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
//     uint32_t expectedWords[aesKey::lengthInWords]{0x03020100, 0x07060504, 0x0B0A0908, 0x0F0E0D0C};
//     testBlock.setFromWordArray(expectedWords);
//     for (uint32_t index = 0; index < aesBlock::lengthInBytes; ++index) {
//         TEST_ASSERT_EQUAL_UINT8(expectedBytes[index], testBlock.getAsByte(index));
//     }
//     for (uint32_t index = 0; index < aesBlock::lengthInWords; ++index) {
//         TEST_ASSERT_EQUAL_UINT32(expectedWords[index], testBlock.getAsWord(index));
//     }
// }

void test_setFromHexString() {
    aesBlock testBlock;
    testBlock.setFromHexString("000102030405060708090A0B0C0D0E0F");
    uint8_t expectedBytes[aesKey::lengthInBytes]{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    uint32_t expectedWords[aesKey::lengthInWords]{0x03020100, 0x07060504, 0x0B0A0908, 0x0F0E0D0C};
    for (uint32_t index = 0; index < aesBlock::lengthInBytes; ++index) {
        TEST_ASSERT_EQUAL_UINT8(expectedBytes[index], testBlock.getAsByte(index));
    }
    for (uint32_t index = 0; index < aesBlock::lengthInWords; ++index) {
        TEST_ASSERT_EQUAL_UINT32(expectedWords[index], testBlock.getAsWord(index));
    }
}

void test_getByte() {
    aesBlock testBlock;
    testBlock.setFromHexString("00112233445566778899AABBCCDDEEFF");

    TEST_ASSERT_EQUAL_UINT8(testBlock.getAsByte(0), 0x00);
    TEST_ASSERT_EQUAL_UINT8(testBlock.getAsByte(1), 0x11);
    TEST_ASSERT_EQUAL_UINT8(testBlock.getAsByte(2), 0x22);
    TEST_ASSERT_EQUAL_UINT8(testBlock.getAsByte(3), 0x33);
    TEST_ASSERT_EQUAL_UINT8(testBlock.getAsByte(4), 0x44);
    TEST_ASSERT_EQUAL_UINT8(testBlock.getAsByte(5), 0x55);
    TEST_ASSERT_EQUAL_UINT8(testBlock.getAsByte(6), 0x66);
    TEST_ASSERT_EQUAL_UINT8(testBlock.getAsByte(7), 0x77);
    TEST_ASSERT_EQUAL_UINT8(testBlock.getAsByte(8), 0x88);
    TEST_ASSERT_EQUAL_UINT8(testBlock.getAsByte(9), 0x99);
    TEST_ASSERT_EQUAL_UINT8(testBlock.getAsByte(10), 0xAA);
    TEST_ASSERT_EQUAL_UINT8(testBlock.getAsByte(11), 0xBB);
    TEST_ASSERT_EQUAL_UINT8(testBlock.getAsByte(12), 0xCC);
    TEST_ASSERT_EQUAL_UINT8(testBlock.getAsByte(13), 0xDD);
    TEST_ASSERT_EQUAL_UINT8(testBlock.getAsByte(14), 0xEE);
    TEST_ASSERT_EQUAL_UINT8(testBlock.getAsByte(15), 0xFF);
}

void test_encrypt() {
    aesKey aKey;
    aKey.setFromHexString("2b7e151628aed2a6abf7158809cf4f3c");        // common NIST test vector key
    aesBlock aBlock;
    aBlock.setFromHexString("6bc1bee22e409f96e93d7e117393172a");        // common NIST test vector plaintext 6bc1bee22e409f96e93d7e117393172a
    aBlock.encrypt(aKey);
    uint8_t expectedCypherText[16]{0x3A, 0xD7, 0x7B, 0xB4, 0x0D, 0x7A, 0x36, 0x60, 0xA8, 0x9E, 0xCA, 0xF3, 0x24, 0x66, 0xEF, 0x97};        // common NIST test vector ciphertext 3ad77bb40d7a3660a89ecaf32466ef97
    for (size_t i = 0; i < 16; ++i) {
        TEST_ASSERT_EQUAL_UINT8(expectedCypherText[i], aBlock.getAsByte(i));
    }
}

void test_encrypt_step_by_step() {
    aesKey aKey;
    aKey.setFromHexString("2b7e151628aed2a6abf7158809cf4f3c");        // common NIST test vector key

    aesBlock aBlock;
    uint8_t clearText[16]{0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96, 0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A};        // common NIST test vector plaintext
    aBlock.setFromByteArray(clearText);

    // Step 1 XOR plaintext with key
    uint8_t tmpKey[16];
    for (size_t byteIndex = 0; byteIndex < 16; ++byteIndex) {
        tmpKey[byteIndex] = aKey.getAsByte(byteIndex);
    }
    aBlock.XOR(tmpKey);
    uint8_t expectedOutput0[16]{0x40, 0xbf, 0xab, 0xf4, 0x06, 0xee, 0x4d, 0x30, 0x42, 0xca, 0x6b, 0x99, 0x7a, 0x5c, 0x58, 0x16};        // follow along on https://www.cryptool.org/en/cto/aes-step-by-step
    for (size_t i = 0; i < 16; ++i) {
        TEST_ASSERT_EQUAL_UINT8(expectedOutput0[i], aBlock.getAsByte(i));
    }

    // Step 2 substitute bytes
    aBlock.substituteBytes();
    uint8_t expectedOutput1[16]{0x09, 0x08, 0x62, 0xbf, 0x6f, 0x28, 0xe3, 0x04, 0x2c, 0x74, 0x7f, 0xee, 0xda, 0x4a, 0x6a, 0x47};
    for (size_t i = 0; i < 16; ++i) {
        TEST_ASSERT_EQUAL_UINT8(expectedOutput1[i], aBlock.getAsByte(i));
    }

    // Step 3 shift rows / permutation
    aBlock.shiftRows();
    uint8_t expectedOutput2[16]{0x09, 0x28, 0x7f, 0x47, 0x6f, 0x74, 0x6a, 0xbf, 0x2c, 0x4a, 0x62, 0x04, 0xda, 0x08, 0xe3, 0xee};
    for (size_t i = 0; i < 16; ++i) {
        TEST_ASSERT_EQUAL_UINT8(expectedOutput2[i], aBlock.getAsByte(i));
    }

    // Step 4 mix columns
    aBlock.mixColumns();
    uint8_t expectedOutput3[16]{0x52, 0x9f, 0x16, 0xc2, 0x97, 0x86, 0x15, 0xca, 0xe0, 0x1a, 0xae, 0x54, 0xba, 0x1a, 0x26, 0x59};
    for (size_t i = 0; i < 16; ++i) {
        TEST_ASSERT_EQUAL_UINT8(expectedOutput3[i], aBlock.getAsByte(i));
    }

    // Step 5 XOR with key
    aBlock.XOR(aKey.expandedKey + 16);
    uint8_t expectedOutput4[16]{0xf2, 0x65, 0xe8, 0xd5, 0x1f, 0xd2, 0x39, 0x7b, 0xc3, 0xb9, 0x97, 0x6d, 0x90, 0x76, 0x50, 0x5c};
    for (size_t i = 0; i < 16; ++i) {
        TEST_ASSERT_EQUAL_UINT8(expectedOutput4[i], aBlock.getAsByte(i));
    }
}

// Tests the correct mapping of a 16 byte vector to a 4x4 matrix in the way AES State expects it, in both directions
void testMatrixToVectorToMatrix() {
    uint8_t vectorIn[16]{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    uint8_t matrixOut[4][4];
    aesBlock::vectorToMatrix(matrixOut, vectorIn);
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
    aesBlock::matrixToVector(vectorOut, matrixOut);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(vectorIn, vectorOut, 16);
}

void testBytesToWordsToBytes() {
    uint8_t bytesIn[16]{0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00};
    uint32_t wordsOut[4];
    aesBlock::bytesToWords(wordsOut, bytesIn);
    TEST_ASSERT_EQUAL_UINT32(0xFFEEDDCC, wordsOut[0]);
    TEST_ASSERT_EQUAL_UINT32(0xBBAA9988, wordsOut[1]);
    TEST_ASSERT_EQUAL_UINT32(0x77665544, wordsOut[2]);
    TEST_ASSERT_EQUAL_UINT32(0x33221100, wordsOut[3]);

    uint8_t bytesOut[16];
    aesBlock::wordsToBytes(bytesOut, wordsOut);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(bytesIn, bytesOut, 16);
}

void test_swapLittleBigEndian() {
    uint32_t wordIn = 0x33221100;
    TEST_ASSERT_EQUAL_UINT32(0x00112233, aesBlock::swapLittleBigEndian(wordIn));
}

void test_XOR() {
    aesBlock aBlock;
    uint8_t inputBytesBlock[16]{0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96, 0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A};        // Using testvectors from https://csrc.nist.gov/projects/cryptographic-algorithm-validation-program/block-ciphers#AES
    aBlock.setFromByteArray(inputBytesBlock);

    aesKey theKey;
    theKey.setFromHexString("2b7e151628aed2a6abf7158809cf4f3c");        // common NIST test vector key

    uint8_t tmpKey[16];
    for (size_t byteIndex = 0; byteIndex < 16; ++byteIndex) {
        tmpKey[byteIndex] = theKey.getAsByte(byteIndex);
    }
    aBlock.XOR(tmpKey);

    uint8_t expectedBytes[16]{0x40, 0xbf, 0xab, 0xf4, 0x06, 0xee, 0x4d, 0x30, 0x42, 0xca, 0x6b, 0x99, 0x7a, 0x5c, 0x58, 0x16};        // verified from https://www.cryptool.org/en/cto/aes-step-by-step
    for (size_t i = 0; i < 16; ++i) {
        TEST_ASSERT_EQUAL_UINT8(expectedBytes[i], aBlock.getAsByte(i));
    }
}

void test_substituteBytes() {
    // Using testvectors from https://csrc.nist.gov/projects/cryptographic-algorithm-validation-program/block-ciphers#AES
    // verified from https://www.cryptool.org/en/cto/aes-step-by-step
    aesBlock aBlock;
    uint8_t inputBytes[16]{0x40, 0xbf, 0xab, 0xf4, 0x06, 0xee, 0x4d, 0x30, 0x42, 0xca, 0x6b, 0x99, 0x7a, 0x5c, 0x58, 0x16};
    aBlock.setFromByteArray(inputBytes);
    aBlock.substituteBytes();
    uint8_t bytesOut[16]{0x09, 0x08, 0x62, 0xbf, 0x6f, 0x28, 0xe3, 0x04, 0x2c, 0x74, 0x7f, 0xee, 0xda, 0x4a, 0x6a, 0x47};

    for (size_t i = 0; i < 16; ++i) {
        TEST_ASSERT_EQUAL_UINT8(bytesOut[i], aBlock.getAsByte(i));
    }
}

void test_shiftRows() {
    // Using testvectors from https://csrc.nist.gov/projects/cryptographic-algorithm-validation-program/block-ciphers#AES
    // verified from https://www.cryptool.org/en/cto/aes-step-by-step, where it is called permutation
    aesBlock aBlock;
    uint8_t inputBytes[16]{0x09, 0x08, 0x62, 0xbf, 0x6f, 0x28, 0xe3, 0x04, 0x2c, 0x74, 0x7f, 0xee, 0xda, 0x4a, 0x6a, 0x47};
    aBlock.setFromByteArray(inputBytes);
    aBlock.shiftRows();
    uint8_t bytesOut[16]{0x09, 0x28, 0x7f, 0x47, 0x6f, 0x74, 0x6a, 0xbf, 0x2c, 0x4a, 0x62, 0x04, 0xda, 0x08, 0xe3, 0xee};

    for (size_t i = 0; i < 16; ++i) {
        TEST_ASSERT_EQUAL_UINT8(bytesOut[i], aBlock.getAsByte(i));
    }
}

void test_mixColumns() {
    aesBlock aBlock;
    uint8_t inputBytes[16]{0x09, 0x28, 0x7f, 0x47, 0x6f, 0x74, 0x6a, 0xbf, 0x2c, 0x4a, 0x62, 0x04, 0xda, 0x08, 0xe3, 0xee};
    aBlock.setFromByteArray(inputBytes);
    aBlock.mixColumns();
    uint8_t expectedOutput[16]{0x52, 0x9f, 0x16, 0xc2, 0x97, 0x86, 0x15, 0xca, 0xe0, 0x1a, 0xae, 0x54, 0xba, 0x1a, 0x26, 0x59};
    for (size_t i = 0; i < 16; ++i) {
        TEST_ASSERT_EQUAL_UINT8(expectedOutput[i], aBlock.getAsByte(i));
    }
}

void test_nmbrOfBlocksFromBytes() {
    TEST_ASSERT_EQUAL(0, aesBlock::nmbrOfBlocksFromBytes(0));
    TEST_ASSERT_EQUAL(1, aesBlock::nmbrOfBlocksFromBytes(1));
    TEST_ASSERT_EQUAL(1, aesBlock::nmbrOfBlocksFromBytes(16));
    TEST_ASSERT_EQUAL(2, aesBlock::nmbrOfBlocksFromBytes(17));
    TEST_ASSERT_EQUAL(2, aesBlock::nmbrOfBlocksFromBytes(32));
}

void test_incompleteLastBlockSizeFromBytes() {
    TEST_ASSERT_EQUAL(0, aesBlock::incompleteLastBlockSizeFromBytes(0));
    TEST_ASSERT_EQUAL(1, aesBlock::incompleteLastBlockSizeFromBytes(1));
    TEST_ASSERT_EQUAL(15, aesBlock::incompleteLastBlockSizeFromBytes(15));
    TEST_ASSERT_EQUAL(0, aesBlock::incompleteLastBlockSizeFromBytes(16));
    TEST_ASSERT_EQUAL(1, aesBlock::incompleteLastBlockSizeFromBytes(17));
}

void test_nmbrOfBytesToPad() {
    TEST_ASSERT_EQUAL(16, aesBlock::calculateNmbrOfBytesToPad(0));        // exception in the AES-CMAC algorithm
    TEST_ASSERT_EQUAL(15, aesBlock::calculateNmbrOfBytesToPad(1));
    TEST_ASSERT_EQUAL(1, aesBlock::calculateNmbrOfBytesToPad(15));
    TEST_ASSERT_EQUAL(0, aesBlock::calculateNmbrOfBytesToPad(16));
    TEST_ASSERT_EQUAL(15, aesBlock::calculateNmbrOfBytesToPad(1 + 64));
    TEST_ASSERT_EQUAL(1, aesBlock::calculateNmbrOfBytesToPad(15 + 64));
    TEST_ASSERT_EQUAL(0, aesBlock::calculateNmbrOfBytesToPad(16 + 64));
}

void test_shiftLeft() {
    unsigned char input[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    aesBlock testBlock;
    testBlock.setFromByteArray(input);
    testBlock.shiftLeft();
    unsigned char output[16] = {0x0, 0x02, 0x04, 0x06, 0x08, 0x0A, 0xC, 0x0E, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1E};
    for (size_t i = 0; i < 16; ++i) {
        TEST_ASSERT_EQUAL_UINT8(output[i], testBlock.getAsByte(i));
    }
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_setFromByteArray);
    // RUN_TEST(test_setFromWordArray);
    RUN_TEST(test_setFromHexString);
    RUN_TEST(test_getByte);

    RUN_TEST(test_nmbrOfBlocksFromBytes);
    RUN_TEST(test_incompleteLastBlockSizeFromBytes);
    RUN_TEST(test_nmbrOfBytesToPad);

    RUN_TEST(test_swapLittleBigEndian);
    RUN_TEST(test_encrypt);

    RUN_TEST(testMatrixToVectorToMatrix);
    RUN_TEST(testBytesToWordsToBytes);

    RUN_TEST(test_XOR);
    RUN_TEST(test_substituteBytes);
    RUN_TEST(test_shiftRows);
    RUN_TEST(test_mixColumns);
    RUN_TEST(test_shiftLeft);

    RUN_TEST(test_encrypt_step_by_step);

    UNITY_END();
}