#include <unity.h>
#include <framecount.hpp>

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_initialize() {
    frameCount testFrameCount1;
    TEST_ASSERT_EQUAL(0, testFrameCount1.getAsWord());
    frameCount testFrameCount2;
    testFrameCount2.setFromWord(0x12345678);
    TEST_ASSERT_EQUAL(0x12345678, testFrameCount2.getAsWord());
}


void test_operatorIncrement() {
    frameCount testFrameCount;
    TEST_ASSERT_EQUAL(0, testFrameCount.getAsWord());
    testFrameCount.increment();
    TEST_ASSERT_EQUAL(1, testFrameCount.getAsWord());
    testFrameCount.increment();
    TEST_ASSERT_EQUAL(2, testFrameCount.getAsWord());
}

void test_guessFromUint16() {
    frameCount testFrameCount;
    testFrameCount.setFromWord(0x12348000);
    testFrameCount.guessFromUint16(0x8010);
    TEST_ASSERT_EQUAL(0x12348010, testFrameCount.getAsWord());
    testFrameCount.guessFromUint16(0x8010);
    TEST_ASSERT_EQUAL(0x12348010, testFrameCount.getAsWord());
    testFrameCount.guessFromUint16(0x8110);
    TEST_ASSERT_EQUAL(0x12348010, testFrameCount.getAsWord());
    testFrameCount.guessFromUint16(0x8109);
    TEST_ASSERT_EQUAL(0x12348109, testFrameCount.getAsWord());
}

int main(int argc, char **argv) {
#ifndef generic
// Here we could setup the STM32 for target unit testing
#endif
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_operatorIncrement);
    RUN_TEST(test_guessFromUint16);
    UNITY_END();
}