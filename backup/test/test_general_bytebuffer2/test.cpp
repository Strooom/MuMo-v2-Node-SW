#include <unity.h>
#include "bytebuffer2.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_initialize() {
    constexpr uint32_t testBufferLength{32};
    byteBuffer2<testBufferLength> aBuffer;
    TEST_ASSERT_EQUAL_UINT32(testBufferLength, aBuffer.length);           // buffer has correct length
    TEST_ASSERT_EQUAL_UINT32(0U, aBuffer.getLevel());                     // upon creation, the buffer is empty
    TEST_ASSERT_EQUAL_UINT32(testBufferLength, aBuffer.getFree());        //
    TEST_ASSERT_TRUE(aBuffer.isEmpty());                                  //

    uint32_t testValue{123U};
    aBuffer.append(testValue);                                                // use the buffer...
    TEST_ASSERT_EQUAL_UINT32(1U, aBuffer.getLevel());                         // so it should not be empty anymore
    TEST_ASSERT_EQUAL_UINT32(testBufferLength - 1, aBuffer.getFree());        //
    TEST_ASSERT_FALSE(aBuffer.isEmpty());                                     // so it should not be empty anymore

    aBuffer.initialize();                                                     //
    TEST_ASSERT_EQUAL_UINT32(0U, aBuffer.getLevel());                         // after it has been used, and then re-initialized, the buffer is empty again
    TEST_ASSERT_TRUE(aBuffer.isEmpty());                                      //
}

void test_append_byte() {
    constexpr uint32_t testBufferLength{32};
    byteBuffer2<testBufferLength> aBuffer;

    for (uint8_t testValue = 0; testValue < (testBufferLength - 1); testValue++) {
        aBuffer.append(testValue);
        TEST_ASSERT_EQUAL_UINT32(testValue + 1, aBuffer.getLevel());
        TEST_ASSERT_EQUAL_UINT8(testValue, aBuffer[testValue]);
    }
    TEST_ASSERT_EQUAL_UINT8(0U, aBuffer[testBufferLength - 1]);        // reading beyond the end of the meaningfull data returns 0
    TEST_ASSERT_EQUAL_UINT8(0U, aBuffer[testBufferLength]);            // reading beyond the buffer
}

void test_append_array() {
    constexpr uint32_t testBufferLength{32};
    constexpr uint32_t testArrayLength{4};
    byteBuffer2<testBufferLength> aBuffer;

    for (uint8_t index = 0; index < (testBufferLength / testArrayLength); index++) {
        uint8_t testArray[testArrayLength];
        testArray[0] = index * testArrayLength;
        testArray[1] = (index * testArrayLength) + 1;
        testArray[2] = (index * testArrayLength) + 2;
        testArray[3] = (index * testArrayLength) + 3;
        aBuffer.append(testArray, testArrayLength);
        TEST_ASSERT_EQUAL_UINT32((index + 1) * testArrayLength, aBuffer.getLevel());
    }

    for (uint8_t index = 0; index < testBufferLength; index++) {
        TEST_ASSERT_EQUAL_UINT8(index, aBuffer[index]);
    }

    aBuffer.append((uint8_t *)"1234567890ABCDEFGHIJ", 20U);        // attempt to append more than what we have room for
    TEST_ASSERT_EQUAL_UINT32(testBufferLength, aBuffer.getLevel());
}

void test_shiftLeft() {
    constexpr uint32_t testBufferLength{32};
    byteBuffer2<testBufferLength> aBuffer;
    aBuffer.append((uint8_t *)"1234567890ABCDEFGHIJ", 20U);
    aBuffer.shiftLeft(10U);
    TEST_ASSERT_EQUAL_UINT32(10U, aBuffer.getLevel());
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *)"ABCDEFGHIJ", aBuffer.asUint8Ptr(), 10U);

    aBuffer.shiftLeft(15U);
    TEST_ASSERT_EQUAL_UINT32(0U, aBuffer.getLevel());
}

void test_shiftRight() {
    constexpr uint32_t testBufferLength{32};
    byteBuffer2<testBufferLength> aBuffer;
    aBuffer.append((uint8_t *)"1234567890", 10U);
    TEST_ASSERT_EQUAL_UINT32(10U, aBuffer.getLevel());
    aBuffer.shiftRight(10U);
    TEST_ASSERT_EQUAL_UINT32(20U, aBuffer.getLevel());
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *)"12345678901234567890", aBuffer.asUint8Ptr(), 20U);
    aBuffer.shiftRight(20U);
    TEST_ASSERT_EQUAL_UINT32(32U, aBuffer.getLevel());
}

void test_prefix_byte() {
    constexpr uint32_t testBufferLength{32};
    byteBuffer2<testBufferLength> aBuffer;
    aBuffer.append(123U);
    aBuffer.prefix(10U);
    TEST_ASSERT_EQUAL_UINT32(2U, aBuffer.getLevel());
    TEST_ASSERT_EQUAL_UINT8(10U, aBuffer[0]);
    TEST_ASSERT_EQUAL_UINT8(123U, aBuffer[1]);
}

void test_prefix_array() {
    constexpr uint32_t testBufferLength{32};
    byteBuffer2<testBufferLength> aBuffer;
    aBuffer.append(0x04);
    constexpr uint32_t testArrayLength{4};
    uint8_t testArray[testArrayLength];
    testArray[0] = 0x00;
    testArray[1] = 0x01;
    testArray[2] = 0x02;
    testArray[3] = 0x03;
    aBuffer.prefix(testArray, testArrayLength);
    TEST_ASSERT_EQUAL_UINT32(5U, aBuffer.getLevel());
    TEST_ASSERT_EQUAL_UINT8(0U, aBuffer[0]);
    TEST_ASSERT_EQUAL_UINT8(1U, aBuffer[1]);
    TEST_ASSERT_EQUAL_UINT8(2U, aBuffer[2]);
    TEST_ASSERT_EQUAL_UINT8(3U, aBuffer[3]);
    TEST_ASSERT_EQUAL_UINT8(4U, aBuffer[4]);
}

void test_set() {
    constexpr uint32_t testBufferLength{32};
    byteBuffer2<testBufferLength> aBuffer;
    aBuffer.set((uint8_t *)"1234567890", 10U);
    TEST_ASSERT_EQUAL_UINT32(0U, aBuffer.getLevel());        // setting the data overwrites the existing data but does NOT change the level
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *)"1234567890", aBuffer.asUint8Ptr(), 10U);

    aBuffer.set((uint8_t *)"ABCDE", 5U, 5U);        // set with an offset, overwrites bytes 5..10
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *)"ABCDE", aBuffer.asUint8Ptr(5U), 5U);
}

void test_consume() {
    constexpr uint32_t testBufferLength{32};
    byteBuffer2<testBufferLength> aBuffer;
    aBuffer.append((uint8_t *)"1234567890", 10U);
    aBuffer.consume(5U);
    TEST_ASSERT_EQUAL_UINT32(5U, aBuffer.getLevel());
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *)"67890", aBuffer.asUint8Ptr(), 5U);
}

void test_truncate() {
    constexpr uint32_t testBufferLength{32};
    byteBuffer2<testBufferLength> aBuffer;
    aBuffer.append((uint8_t *)"1234567890", 10U);
    aBuffer.truncate(5U);
    TEST_ASSERT_EQUAL_UINT32(5U, aBuffer.getLevel());
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t *)"12345", aBuffer.asUint8Ptr(), 5U);

    aBuffer.truncate(10U);        // truncate more than what's in the buffer
    TEST_ASSERT_EQUAL_UINT32(0, aBuffer.getLevel());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_append_byte);
    RUN_TEST(test_append_array);
    RUN_TEST(test_shiftLeft);
    RUN_TEST(test_shiftRight);
    RUN_TEST(test_prefix_byte);
    RUN_TEST(test_prefix_array);
    RUN_TEST(test_set);
    RUN_TEST(test_consume);
    RUN_TEST(test_truncate);
    UNITY_END();
}