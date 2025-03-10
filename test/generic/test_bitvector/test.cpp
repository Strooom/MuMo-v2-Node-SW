// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include <bitvector.hpp>

void setUp(void) {}
void tearDown(void) {}

void test_initialize() {
    bitVector<16U> testVector;
    TEST_ASSERT_EQUAL(0, testVector.level);
    TEST_ASSERT_EQUAL(16U, testVector.length);
    TEST_ASSERT_EQUAL(2U, testVector.lengthInBytes);

    bitVector<20U> testVector2;
    TEST_ASSERT_EQUAL(0, testVector2.level);
    TEST_ASSERT_EQUAL(20U, testVector2.length);
    TEST_ASSERT_EQUAL(3U, testVector2.lengthInBytes);
}

void test_Offset() {
    bitVector<20U> testVector;
    TEST_ASSERT_EQUAL(0, testVector.byteOffset(0));
    TEST_ASSERT_EQUAL(0, testVector.byteOffset(7));
    TEST_ASSERT_EQUAL(1, testVector.byteOffset(8));
    TEST_ASSERT_EQUAL(1, testVector.byteOffset(15));
}

void test_bitMask() {
    bitVector<20U> testVector;
    TEST_ASSERT_EQUAL(0b10000000, testVector.bitMask(0));
    TEST_ASSERT_EQUAL(0b00000001, testVector.bitMask(7));
    TEST_ASSERT_EQUAL(0b10000000, testVector.bitMask(8));
    TEST_ASSERT_EQUAL(0b00000001, testVector.bitMask(15));
}

void test_set_get() {
    bitVector<40U> testVector;
    testVector.setBit(0);
    TEST_ASSERT_TRUE(testVector.getBit(0));
    testVector.clearBit(0);
    TEST_ASSERT_FALSE(testVector.getBit(0));

    testVector.setBit(12);
    TEST_ASSERT_TRUE(testVector.getBit(12));
    testVector.clearBit(12);
    TEST_ASSERT_FALSE(testVector.getBit(12));

    testVector.setBit(20);
    TEST_ASSERT_TRUE(testVector.getBit(20));
    testVector.invertBit(20);
    TEST_ASSERT_FALSE(testVector.getBit(20));
    testVector.invertBit(20);
    TEST_ASSERT_TRUE(testVector.getBit(20));
}

void test_appendBits() {
    bitVector<40U> testVector;
    testVector.appendBits(0b00001111, 8);
    testVector.appendBits(0b10101010, 8);
    TEST_ASSERT_EQUAL(16, testVector.level);
    TEST_ASSERT_EQUAL(0x0F, testVector.data[0]);
    TEST_ASSERT_EQUAL(0xAA, testVector.data[1]);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_Offset);
    RUN_TEST(test_bitMask);
    RUN_TEST(test_set_get);
    RUN_TEST(test_appendBits);
    UNITY_END();
}