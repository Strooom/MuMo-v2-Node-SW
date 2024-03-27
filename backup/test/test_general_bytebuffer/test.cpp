#include <unity.h>

#include "bytebuffer.h"
#include "hextools.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test




void test_initialize() {
    byteBuffer aBuffer;
    TEST_ASSERT_EQUAL_UINT32(0U, aBuffer.length);
    TEST_ASSERT_EQUAL_UINT32(0U, aBuffer.buffer[0]);
    TEST_ASSERT_EQUAL_UINT32(0U, aBuffer.buffer[byteBuffer::maxLength - 1]);
    // TEST_ASSERT_EQUAL_STRING("3AD77BB40D7A3660A89ECAF32466EF97", outputAsHexString);
}

void test_set1() {
    byteBuffer aBuffer;
    uint8_t sourceData[4] = {0x01, 0x02, 0x03, 0x04};
    aBuffer.set(sourceData, 4);
    TEST_ASSERT_EQUAL_UINT32(4U, aBuffer.length);
    TEST_ASSERT_EQUAL_UINT32(1U, aBuffer.buffer[0]);
    TEST_ASSERT_EQUAL_UINT32(4U, aBuffer.buffer[aBuffer.length - 1]);
    TEST_ASSERT_EQUAL_UINT32(0U, aBuffer.buffer[aBuffer.length]);
}

void test_set2() {
    byteBuffer aBuffer;
    char sourceData[4] = "ABC";
    aBuffer.set(sourceData);
    TEST_ASSERT_EQUAL_UINT32(3U, aBuffer.length);
    TEST_ASSERT_EQUAL_UINT32('A', aBuffer.buffer[0]);
    TEST_ASSERT_EQUAL_UINT32('C', aBuffer.buffer[aBuffer.length - 1]);
    TEST_ASSERT_EQUAL_UINT32(0U, aBuffer.buffer[aBuffer.length]);
}

void test_setFromHexAscii() {
    byteBuffer aBuffer;
    char sourceData[8] = "0180FE";
    aBuffer.setFromHexAscii(sourceData);
    TEST_ASSERT_EQUAL_UINT32(3U, aBuffer.length);
    TEST_ASSERT_EQUAL_UINT32(0x01, aBuffer.buffer[0]);
    TEST_ASSERT_EQUAL_UINT32(0xFE, aBuffer.buffer[aBuffer.length - 1]);
    TEST_ASSERT_EQUAL_UINT32(0U, aBuffer.buffer[aBuffer.length]);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_set1);
    RUN_TEST(test_set2);
    RUN_TEST(test_setFromHexAscii);
    UNITY_END();
}