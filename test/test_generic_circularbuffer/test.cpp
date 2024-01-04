#include <unity.h>

#include <circularbuffer.hpp>

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_initialize() {
    constexpr uint32_t testBufferLength{4};
    circularBuffer<uint8_t, testBufferLength> theBuffer;
    TEST_ASSERT_EQUAL_UINT32(0, theBuffer.head);
    TEST_ASSERT_EQUAL_UINT32(0, theBuffer.level);
    TEST_ASSERT_EQUAL_UINT32(testBufferLength, theBuffer.length);
    TEST_ASSERT_TRUE(theBuffer.isEmpty());
    TEST_ASSERT_FALSE(theBuffer.hasEvents());
}

void test_push_pop() {
    constexpr uint32_t testBufferLength{4};
    circularBuffer<uint8_t, testBufferLength> theBuffer;
    theBuffer.push(0x11);
    theBuffer.push(0x22);
    theBuffer.push(0x33);
    theBuffer.push(0x44);
    TEST_ASSERT_EQUAL_UINT32(testBufferLength, theBuffer.level);
    TEST_ASSERT_EQUAL_UINT32(0, theBuffer.head);
    TEST_ASSERT_FALSE(theBuffer.isEmpty());
    TEST_ASSERT_TRUE(theBuffer.hasEvents());

    TEST_ASSERT_EQUAL_UINT32(0x11, theBuffer.pop());
    TEST_ASSERT_EQUAL_UINT32(0x22, theBuffer.pop());
    TEST_ASSERT_EQUAL_UINT32(0x33, theBuffer.pop());
    TEST_ASSERT_EQUAL_UINT32(0x44, theBuffer.pop());
    TEST_ASSERT_EQUAL_UINT32(0, theBuffer.level);
    TEST_ASSERT_EQUAL_UINT32(0, theBuffer.head);
    TEST_ASSERT_TRUE(theBuffer.isEmpty());
    TEST_ASSERT_FALSE(theBuffer.hasEvents());
}

void test_push_pop_wrap() {
    constexpr uint32_t testBufferLength{4};
    circularBuffer<uint8_t, testBufferLength> theBuffer;
    theBuffer.push(0x11);
    theBuffer.push(0x22);
    (void)theBuffer.pop();
    (void)theBuffer.pop();
    TEST_ASSERT_EQUAL_UINT32(0, theBuffer.level);
    TEST_ASSERT_EQUAL_UINT32(2, theBuffer.head);

    theBuffer.push(0x33);
    theBuffer.push(0x44);
    theBuffer.push(0x55);
    theBuffer.push(0x66);

    TEST_ASSERT_EQUAL_UINT32(testBufferLength, theBuffer.level);
    TEST_ASSERT_EQUAL_UINT32(2, theBuffer.head);
    TEST_ASSERT_EQUAL_UINT32(0x33, theBuffer.pop());
    TEST_ASSERT_EQUAL_UINT32(0x44, theBuffer.pop());
    TEST_ASSERT_EQUAL_UINT32(0x55, theBuffer.pop());
    TEST_ASSERT_EQUAL_UINT32(0x66, theBuffer.pop());
    TEST_ASSERT_EQUAL_UINT32(0, theBuffer.level);
    TEST_ASSERT_EQUAL_UINT32(2, theBuffer.head);
}

void test_push_overflow() {
    constexpr uint32_t testBufferLength{4};
    circularBuffer<uint8_t, testBufferLength> theBuffer;
    theBuffer.push(0x11);
    theBuffer.push(0x22);
    theBuffer.push(0x33);
    theBuffer.push(0x44);
    theBuffer.push(0x55);
    theBuffer.push(0x66);

    TEST_ASSERT_EQUAL_UINT32(testBufferLength, theBuffer.level);
    TEST_ASSERT_EQUAL_UINT32(2, theBuffer.head);
    TEST_ASSERT_EQUAL_UINT32(0x33, theBuffer.pop());
    TEST_ASSERT_EQUAL_UINT32(0x44, theBuffer.pop());
    TEST_ASSERT_EQUAL_UINT32(0x55, theBuffer.pop());
    TEST_ASSERT_EQUAL_UINT32(0x66, theBuffer.pop());
    TEST_ASSERT_EQUAL_UINT32(0, theBuffer.level);
    TEST_ASSERT_EQUAL_UINT32(2, theBuffer.head);
}

void test_pop_underflow() {
    constexpr uint32_t testBufferLength{4};
    circularBuffer<uint8_t, testBufferLength> theBuffer;
    TEST_ASSERT_EQUAL_UINT32(0x00, theBuffer.pop());
    TEST_ASSERT_EQUAL_UINT32(0, theBuffer.level);
    TEST_ASSERT_EQUAL_UINT32(0, theBuffer.head);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_push_pop);
    RUN_TEST(test_push_pop_wrap);
    RUN_TEST(test_push_overflow);
    RUN_TEST(test_pop_underflow);
    UNITY_END();
}