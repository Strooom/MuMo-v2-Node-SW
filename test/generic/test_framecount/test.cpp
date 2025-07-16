#include <unity.h>
#include <framecount.hpp>

void setUp(void) {}
void tearDown(void) {}

void test_initialize() {
    frameCount testFrameCount1;
    TEST_ASSERT_EQUAL(0, testFrameCount1.getAsWord());
    uint8_t expectedBytes1[frameCount::lengthInBytes]{0, 0, 0, 0};
    for (uint32_t index = 0; index < frameCount::lengthInBytes; index++) {
        TEST_ASSERT_EQUAL(expectedBytes1[index], testFrameCount1.getAsByte(index));
    }

    frameCount testFrameCount2;
    testFrameCount2.setFromWord(0x12345678);
    TEST_ASSERT_EQUAL(0x12345678, testFrameCount2.getAsWord());
    uint8_t expectedBytes2[frameCount::lengthInBytes]{0x78, 0x56, 0x34, 0x12};
    for (uint32_t index = 0; index < frameCount::lengthInBytes; index++) {
        TEST_ASSERT_EQUAL(expectedBytes2[index], testFrameCount2.getAsByte(index));
    }
}

void test_setFromByteArray() {
    frameCount testFrameCount;
    uint8_t testBytes[frameCount::lengthInBytes]{0x78, 0x56, 0x34, 0x12};
    testFrameCount.setFromByteArray(testBytes);
    TEST_ASSERT_EQUAL(0x12345678, testFrameCount.getAsWord());
}

void test_SetFromWord() {
    frameCount testFrameCount;
    testFrameCount.setFromWord(0x12345678);
    TEST_ASSERT_EQUAL(0x12345678, testFrameCount.getAsWord());
    uint8_t expectedBytes[frameCount::lengthInBytes]{0x78, 0x56, 0x34, 0x12};
    for (uint32_t index = 0; index < frameCount::lengthInBytes; index++) {
        TEST_ASSERT_EQUAL(expectedBytes[index], testFrameCount.getAsByte(index));
    }
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

void test_accessBytes() {
    frameCount testFrameCount;
    testFrameCount.setFromWord(0x12345678);
    TEST_ASSERT_EQUAL(0x12345678, testFrameCount.getAsWord());
    TEST_ASSERT_EQUAL(0x78, testFrameCount.getAsByte(0));
    TEST_ASSERT_EQUAL(0x56, testFrameCount.getAsByte(1));
    TEST_ASSERT_EQUAL(0x34, testFrameCount.getAsByte(2));
    TEST_ASSERT_EQUAL(0x12, testFrameCount.getAsByte(3));
}
void test_increment2() {
    frameCount testFrameCount;
    testFrameCount.setFromWord(10);
    testFrameCount.increment();
    TEST_ASSERT_EQUAL(11, testFrameCount.getAsWord());
    testFrameCount.increment();
    TEST_ASSERT_EQUAL(12, testFrameCount.getAsWord());

}


int main(int argc, char **argv) {
#ifndef generic
// Here we could setup the STM32 for target unit testing
#endif
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
     RUN_TEST(test_setFromByteArray);
    RUN_TEST(test_SetFromWord);
    RUN_TEST(test_operatorIncrement);
    RUN_TEST(test_guessFromUint16);
    RUN_TEST(test_accessBytes);
    RUN_TEST(test_increment2);
    UNITY_END();
}