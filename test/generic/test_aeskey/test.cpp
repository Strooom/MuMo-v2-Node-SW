#include <unity.h>
#include <aeskey.hpp>
#include <sbox.hpp>
#include <rcon.hpp>

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_initialize() {
    aesKey aKey;
    uint8_t expectedBytes[aesKey::lengthInBytes]{};
    uint32_t expectedWords[aesKey::lengthInWords]{};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBytes, aKey.asBytes(), aesKey::lengthInBytes);
    TEST_ASSERT_EQUAL_UINT32_ARRAY(expectedWords, aKey.asWords(), aesKey::lengthInWords);
}

void test_setFromByteArray() {
    aesKey aKey;
    uint8_t expectedBytes[aesKey::lengthInBytes]{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    uint32_t expectedWords[aesKey::lengthInWords]{0x03020100, 0x07060504, 0x0B0A0908, 0x0F0E0D0C};
    aKey.setFromByteArray(expectedBytes);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBytes, aKey.asBytes(), aesKey::lengthInBytes);
    TEST_ASSERT_EQUAL_UINT32_ARRAY(expectedWords, aKey.asWords(), aesKey::lengthInWords);
}

void test_setFromWordArray() {
    aesKey aKey;
    uint8_t expectedBytes[aesKey::lengthInBytes]{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    uint32_t expectedWords[aesKey::lengthInWords]{0x03020100, 0x07060504, 0x0B0A0908, 0x0F0E0D0C};
    aKey.setFromWordArray(expectedWords);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBytes, aKey.asBytes(), aesKey::lengthInBytes);
    TEST_ASSERT_EQUAL_UINT32_ARRAY(expectedWords, aKey.asWords(), aesKey::lengthInWords);
}

void test_setFromHexString() {
    aesKey aKey;
    uint8_t expectedBytes[aesKey::lengthInBytes]{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    uint32_t expectedWords[aesKey::lengthInWords]{0x03020100, 0x07060504, 0x0B0A0908, 0x0F0E0D0C};
    aKey.setFromHexString("000102030405060708090A0B0C0D0E0F");
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBytes, aKey.asBytes(), aesKey::lengthInBytes);
    TEST_ASSERT_EQUAL_UINT32_ARRAY(expectedWords, aKey.asWords(), aesKey::lengthInWords);
}

void test_swapLittleBigEndian() {
    uint32_t wordIn = 0x33221100;
    TEST_ASSERT_EQUAL_UINT32(0x00112233, aesKey::swapLittleBigEndian(wordIn));
}

void test_expandKey() {
    aesKey theKey;
    uint8_t inputBytesKey[16]{0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};        // Using a common NIST testvector : 2B7E151628AED2A6ABF7158809CF4F3C
    theKey.setFromByteArray(inputBytesKey);

    uint8_t expectedExpandedKey[176]{0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
                                     0xa0, 0xfa, 0xfe, 0x17, 0x88, 0x54, 0x2c, 0xb1, 0x23, 0xa3, 0x39, 0x39, 0x2a, 0x6c, 0x76, 0x05,
                                     0xf2, 0xc2, 0x95, 0xf2, 0x7a, 0x96, 0xb9, 0x43, 0x59, 0x35, 0x80, 0x7a, 0x73, 0x59, 0xf6, 0x7f,
                                     0x3d, 0x80, 0x47, 0x7d, 0x47, 0x16, 0xfe, 0x3e, 0x1e, 0x23, 0x7e, 0x44, 0x6d, 0x7a, 0x88, 0x3b,
                                     0xef, 0x44, 0xa5, 0x41, 0xa8, 0x52, 0x5b, 0x7f, 0xb6, 0x71, 0x25, 0x3b, 0xdb, 0x0b, 0xad, 0x00,
                                     0xd4, 0xd1, 0xc6, 0xf8, 0x7c, 0x83, 0x9d, 0x87, 0xca, 0xf2, 0xb8, 0xbc, 0x11, 0xf9, 0x15, 0xbc,
                                     0x6d, 0x88, 0xa3, 0x7a, 0x11, 0x0b, 0x3e, 0xfd, 0xdb, 0xf9, 0x86, 0x41, 0xca, 0x00, 0x93, 0xfd,
                                     0x4e, 0x54, 0xf7, 0x0e, 0x5f, 0x5f, 0xc9, 0xf3, 0x84, 0xa6, 0x4f, 0xb2, 0x4e, 0xa6, 0xdc, 0x4f,
                                     0xea, 0xd2, 0x73, 0x21, 0xb5, 0x8d, 0xba, 0xd2, 0x31, 0x2b, 0xf5, 0x60, 0x7f, 0x8d, 0x29, 0x2f,
                                     0xac, 0x77, 0x66, 0xf3, 0x19, 0xfa, 0xdc, 0x21, 0x28, 0xd1, 0x29, 0x41, 0x57, 0x5c, 0x00, 0x6e,
                                     0xd0, 0x14, 0xf9, 0xa8, 0xc9, 0xee, 0x25, 0x89, 0xe1, 0x3f, 0x0c, 0xc8, 0xb6, 0x63, 0x0c, 0xa6};

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedExpandedKey, theKey.expandedKey, 176);
}


void test_calculateRoundKey() {
    TEST_IGNORE_MESSAGE("Implemented test vector for calculateRoundKey");
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_setFromByteArray);
    RUN_TEST(test_setFromWordArray);
    RUN_TEST(test_setFromHexString);
    RUN_TEST(test_swapLittleBigEndian);

    RUN_TEST(test_expandKey);
    RUN_TEST(test_calculateRoundKey);
    UNITY_END();
}