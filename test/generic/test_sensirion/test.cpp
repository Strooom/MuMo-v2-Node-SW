// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <unity.h>
#include <sensirion.hpp>

void setUp(void) {}
void tearDown(void) {}

void test_crc() {
    uint8_t testData[2] = {0xBE, 0xEF};
    TEST_ASSERT_EQUAL(0x92, sensirion::crc(0xBE, 0xEF));        // testvector from SHT4X datasheet, page 11.
                                                                // testvectors from SCD4x datasheet
    TEST_ASSERT_EQUAL(0xA2, sensirion::crc(0x66, 0x67));
    TEST_ASSERT_EQUAL(0x3C, sensirion::crc(0x5E, 0xB9));
    TEST_ASSERT_EQUAL(0x48, sensirion::crc(0x07, 0xE6));
    TEST_ASSERT_EQUAL(0x63, sensirion::crc(0x09, 0x12));
    TEST_ASSERT_EQUAL(0x09, sensirion::crc(0x07, 0x9E));
    TEST_ASSERT_EQUAL(0x42, sensirion::crc(0x04, 0x4C));
    TEST_ASSERT_EQUAL(0x42, sensirion::crc(0x03, 0xDB));

    //    TEST_ASSERT_EQUAL(0, sensirion::crc(0x05, 0x00));
}

void test_Checkcrc() {
    uint8_t testData1[9] = {0xBE, 0xEF, 0x92, 0xBE, 0xEF, 0x92, 0xBE, 0xEF, 0x92};
    TEST_ASSERT_TRUE(sensirion::checkCrc(testData1, 9U));
    uint8_t testData2[9] = {0xBE, 0xEF, 0x92, 0xBE, 0xFF, 0x92, 0xBE, 0xEF, 0x92};
    TEST_ASSERT_FALSE(sensirion::checkCrc(testData2, 9U));
}

void test_insertCrc() {
    uint8_t testDataIn[9] = {0xBE, 0xEF, 0x00, 0xBE, 0xEF, 0x00, 0xBE, 0xEF, 0x0};
    uint8_t expected[9]   = {0xBE, 0xEF, 0x92, 0xBE, 0xEF, 0x92, 0xBE, 0xEF, 0x92};
    sensirion::insertCrc(testDataIn, 9U);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, testDataIn, 9U);
}

void test_asUint16() {
    uint8_t testDataIn[3] = {0x66, 0x67, 0xA2};
    TEST_ASSERT_EQUAL(0x6667, sensirion::asUint16(testDataIn));
    uint8_t testDataIn2[3] = {0x5E, 0xB9, 0x3C};
    TEST_ASSERT_EQUAL(0x5EB9, sensirion::asUint16(testDataIn2));
}

void test_asUint32() {
    uint8_t testDataIn[6] = {0x66, 0x67, 0xA2, 0x5E, 0xB9, 0x3C};
    TEST_ASSERT_EQUAL(0x66675EB9, sensirion::asUint32(testDataIn));
}

void test_asFloat() {
    uint8_t testDataIn[6] = {0x66, 0x67, 0xA2, 0x5E, 0xB9, 0x3C};
    (void)sensirion::asFloat(testDataIn);
    TEST_IGNORE_MESSAGE("Implement this test...");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_crc);
    RUN_TEST(test_Checkcrc);
    RUN_TEST(test_insertCrc);
    RUN_TEST(test_asUint16);
    RUN_TEST(test_asUint32);
    RUN_TEST(test_asFloat);
    UNITY_END();
}