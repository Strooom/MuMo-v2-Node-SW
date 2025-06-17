#include <unity.h>
#include <framecount.hpp>

void setUp(void) {}
void tearDown(void) {}

void test_initialize() {
    frameCount testFrameCount1;
    TEST_ASSERT_EQUAL(0, testFrameCount1.getAsWord());
    frameCount testFrameCount2;
    testFrameCount2.setFromWord(0x12345678);
    TEST_ASSERT_EQUAL(0x12345678, testFrameCount2.getAsWord());
}

void test_setGet() {
    frameCount testFrameCount;
    testFrameCount.setFromWord(287454020);        // 0x11223344
    TEST_ASSERT_EQUAL(0x44, testFrameCount.getAsByte(0));
    TEST_ASSERT_EQUAL(0x33, testFrameCount.getAsByte(1));
    TEST_ASSERT_EQUAL(0x22, testFrameCount.getAsByte(2));
    TEST_ASSERT_EQUAL(0x11, testFrameCount.getAsByte(3));

    uint8_t testbytes[4] = {0xDD, 0xCC, 0xBB, 0xAA};
    testFrameCount.setFromByteArray(testbytes);
    TEST_ASSERT_EQUAL(2864434397, testFrameCount.getAsWord());
    
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
    RUN_TEST(test_setGet);
    RUN_TEST(test_operatorIncrement);
    RUN_TEST(test_guessFromUint16);
    UNITY_END();
}