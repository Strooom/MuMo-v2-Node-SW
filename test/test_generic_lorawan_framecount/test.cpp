#include <unity.h>
#include <framecount.hpp>


void setUp(void) {}
void tearDown(void) {}

void test_initialize() {
    frameCount testFrameCount1;
    TEST_ASSERT_EQUAL(0, testFrameCount1.asUint32);

    frameCount testFrameCount2(0x123);
    TEST_ASSERT_EQUAL(0x123, testFrameCount2.asUint32);
}

void test_set() {
    frameCount testFrameCount1;
    TEST_ASSERT_EQUAL(0, testFrameCount1.asUint32);
    testFrameCount1.set(0x123);
    TEST_ASSERT_EQUAL(0x123, testFrameCount1.asUint32);
}

void test_operatorAssign() {
    frameCount testFrameCount1;
    TEST_ASSERT_EQUAL(0, testFrameCount1.asUint32);
    testFrameCount1 = 0x123;
    TEST_ASSERT_EQUAL(0x123, testFrameCount1.asUint32);
}

void test_increment() {
    frameCount testFrameCount1;
    TEST_ASSERT_EQUAL(0, testFrameCount1.asUint32);
    testFrameCount1.increment();
    TEST_ASSERT_EQUAL(1, testFrameCount1.asUint32);
    testFrameCount1.increment();
    TEST_ASSERT_EQUAL(2, testFrameCount1.asUint32);
}

void test_guess() {
    TEST_IGNORE_MESSAGE("Not implemented yet");
}

int main(int argc, char **argv) {
#ifndef generic
// Here we could setup the STM32 for target unit testing
#endif
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_set);
    RUN_TEST(test_operatorAssign);
    RUN_TEST(test_increment);
    RUN_TEST(test_guess);
    UNITY_END();
}