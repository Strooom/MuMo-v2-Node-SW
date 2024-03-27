#include <unity.h>
#include "aesblock.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_initialize() {
    aesBlock aBlock;
    uint8_t expectedBytes[aesBlock::lengthAsBytes]{};
    uint32_t expectedWords[aesBlock::lengthAsWords]{};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBytes, aBlock.asBytes(), aesBlock::lengthAsBytes);
    TEST_ASSERT_EQUAL_UINT32_ARRAY(expectedWords, aBlock.asWords(), aesBlock::lengthAsWords);
}

void test_setFromBytes() {
    aesBlock aBlock;
    uint8_t expectedBytes[aesBlock::lengthAsBytes]{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    uint32_t expectedWords[aesBlock::lengthAsWords]{0x03020100, 0x07060504, 0x0B0A0908, 0x0F0E0D0C};
    aBlock.set(expectedBytes);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBytes, aBlock.asBytes(), aesBlock::lengthAsBytes);
    TEST_ASSERT_EQUAL_UINT32_ARRAY(expectedWords, aBlock.asWords(), aesBlock::lengthAsWords);
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_setFromBytes);
    UNITY_END();
}