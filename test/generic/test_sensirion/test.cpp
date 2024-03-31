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

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_crc);
    RUN_TEST(test_Checkcrc);
    RUN_TEST(test_insertCrc);
    UNITY_END();
}