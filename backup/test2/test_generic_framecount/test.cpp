#include <unity.h>
#include "framecount.h"

void setUp(void) {}
void tearDown(void) {}

void test_initialize() {
    frameCount count1;
    TEST_ASSERT_EQUAL_UINT32(0, count1.asUint32);        // default constructor initializes to 0

    frameCount count2(0x12345678);
    TEST_ASSERT_EQUAL_UINT32(0x12345678, count2.asUint32);
}

void test_set() {
    frameCount count1;
    TEST_ASSERT_EQUAL_UINT32(0, count1.asUint32);        // default constructor initializes to 0

    count1.set(0x12345678);
    TEST_ASSERT_EQUAL_UINT32(0x12345678, count1.asUint32);

    count1 = 0x87654321;
    TEST_ASSERT_EQUAL_UINT32(0x87654321, count1.asUint32);

    uint8_t count2Bytes[4] = {0x77, 0x55, 0x33, 0x11};
    count1.set(count2Bytes);
    TEST_ASSERT_EQUAL_UINT32(0x11335577, count1.asUint32);


}

void test_increment() {
    frameCount count1;
    TEST_ASSERT_EQUAL_UINT32(0, count1.asUint32);        // default constructor initializes to 0

    count1.increment();
    TEST_ASSERT_EQUAL_UINT32(1, count1.asUint32);

    count1.set(123);
    count1.increment();
    TEST_ASSERT_EQUAL_UINT32(124, count1.asUint32);
}

void test_guess() {
    TEST_ASSERT_EQUAL_UINT32(0x00000000, frameCount::guessFromUint16(0x00000000, 0x0000));
    TEST_ASSERT_EQUAL_UINT32(0x00000001, frameCount::guessFromUint16(0x00000000, 0x0001));
    TEST_ASSERT_EQUAL_UINT32(0x00000080, frameCount::guessFromUint16(0x00000000, 0x0080));
    TEST_ASSERT_EQUAL_UINT32(0x00000000, frameCount::guessFromUint16(0x00000000, 0xF000));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_set);
    RUN_TEST(test_increment);
    RUN_TEST(test_guess);
    UNITY_END();
}