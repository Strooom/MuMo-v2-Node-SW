// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include <bitvector.hpp>

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_neededLengthForExtBuffer() {
    TEST_ASSERT_EQUAL(0, bitVector::neededLengthInBytes(0));
    TEST_ASSERT_EQUAL(1, bitVector::neededLengthInBytes(1));
    TEST_ASSERT_EQUAL(1, bitVector::neededLengthInBytes(8));
    TEST_ASSERT_EQUAL(2, bitVector::neededLengthInBytes(9));
    TEST_ASSERT_EQUAL(2, bitVector::neededLengthInBytes(16));
}

void test_initialize() {
    uint32_t testLength{40};
    uint8_t storage[bitVector::neededLengthInBytes(testLength)];
    bitVector testVector{storage, testLength};

    TEST_ASSERT_EQUAL(0, testVector.level);
    TEST_ASSERT_EQUAL(testLength, testVector.length);
    TEST_ASSERT_EQUAL(0, testVector.levelInBits());
    TEST_ASSERT_EQUAL(testLength, testVector.lengthInBits());
}


void test_Offset() {
    uint32_t testLength{40};
    uint8_t storage[bitVector::neededLengthInBytes(testLength)];
    bitVector testVector{storage, testLength};
    TEST_ASSERT_EQUAL(0, testVector.byteOffset(0));
    TEST_ASSERT_EQUAL(0, testVector.byteOffset(7));
    TEST_ASSERT_EQUAL(1, testVector.byteOffset(8));
    TEST_ASSERT_EQUAL(1, testVector.byteOffset(15));
    }

void test_bitMask() {
    uint32_t testLength{40};
    uint8_t storage[bitVector::neededLengthInBytes(testLength)];
    bitVector testVector{storage, testLength};
    TEST_ASSERT_EQUAL(0b10000000, testVector.bitMask(0));
    TEST_ASSERT_EQUAL(0b00000001, testVector.bitMask(7));
    TEST_ASSERT_EQUAL(0b10000000, testVector.bitMask(8));
    TEST_ASSERT_EQUAL(0b00000001, testVector.bitMask(15));
}

void test_set_get() {
    uint32_t testLength{40};
    uint8_t storage[bitVector::neededLengthInBytes(testLength)];
    bitVector testVector{storage, testLength};
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
    uint32_t testLength{40};
    uint8_t storage[bitVector::neededLengthInBytes(testLength)];
    bitVector testVector{storage, testLength};
    testVector.appendBits(0b00001111, 8);
    testVector.appendBits(0b10101010, 8);
    TEST_ASSERT_EQUAL(16, testVector.level);
    TEST_ASSERT_EQUAL(0x0F, testVector.data[0]);
    TEST_ASSERT_EQUAL(0xAA, testVector.data[1]);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_neededLengthForExtBuffer);
    RUN_TEST(test_initialize);
    RUN_TEST(test_Offset);
    RUN_TEST(test_bitMask);
    RUN_TEST(test_set_get);
    RUN_TEST(test_appendBits);
    UNITY_END();
}