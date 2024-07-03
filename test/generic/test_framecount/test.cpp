#include <unity.h>
#include <framecount.hpp>

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_initialize() {
    frameCount testFrameCount1;
    TEST_ASSERT_EQUAL(0, testFrameCount1.toUint32());
    frameCount testFrameCount2(0x12345678);
    TEST_ASSERT_EQUAL(0x12345678, testFrameCount2.toUint32());
}


void test_operatorAssign() {
    frameCount testFrameCount1;
    frameCount testFrameCount2;
    TEST_ASSERT_EQUAL(0, testFrameCount1.toUint32());
    TEST_ASSERT_EQUAL(0, testFrameCount2.toUint32());
    testFrameCount1 = 0x12345678;
    TEST_ASSERT_EQUAL(0x12345678, testFrameCount1.toUint32());
    testFrameCount2 = testFrameCount1;
    TEST_ASSERT_EQUAL(0x12345678, testFrameCount2.toUint32());
}

void test_operatorEqual() {
    frameCount testFrameCount1(0x12345678);
    frameCount testFrameCount2(0x12345678);
    frameCount testFrameCount3(0x87654321);
    TEST_ASSERT_TRUE(testFrameCount1 == testFrameCount2);
    TEST_ASSERT_FALSE(testFrameCount2 == testFrameCount3);
}

void test_operatorNotEqual() {
    frameCount testFrameCount1(0x12345678);
    frameCount testFrameCount2(0x12345678);
    frameCount testFrameCount3(0x87654321);
    TEST_ASSERT_TRUE(testFrameCount2 != testFrameCount3);
    TEST_ASSERT_FALSE(testFrameCount1 != testFrameCount2);
}

void test_operatorIncrement() {
    frameCount testFrameCount;
    TEST_ASSERT_EQUAL(0, testFrameCount.toUint32());
    testFrameCount++;
    TEST_ASSERT_EQUAL(1, testFrameCount.toUint32());
    testFrameCount++;
    TEST_ASSERT_EQUAL(2, testFrameCount.toUint32());
}

void test_guessFromUint16() {
    frameCount testFrameCount(0x12348000);
    testFrameCount.guessFromUint16(0x8010);
    TEST_ASSERT_EQUAL(0x12348010, testFrameCount.toUint32());
    testFrameCount.guessFromUint16(0x8010);
    TEST_ASSERT_EQUAL(0x12348010, testFrameCount.toUint32());
    testFrameCount.guessFromUint16(0x8110);
    TEST_ASSERT_EQUAL(0x12348010, testFrameCount.toUint32());
    testFrameCount.guessFromUint16(0x8109);
    TEST_ASSERT_EQUAL(0x12348109, testFrameCount.toUint32());
}

int main(int argc, char **argv) {
#ifndef generic
// Here we could setup the STM32 for target unit testing
#endif
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_operatorAssign);
    RUN_TEST(test_operatorEqual);
    RUN_TEST(test_operatorNotEqual);
    RUN_TEST(test_operatorIncrement);
    RUN_TEST(test_guessFromUint16);
    UNITY_END();
}