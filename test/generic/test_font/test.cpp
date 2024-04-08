// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include "display.hpp"
#include "graphics.hpp"
#include "font.hpp"

fontProperties testRoboto16Properties{15U, 2U,static_cast<uint8_t>('A'), static_cast<uint8_t>('F')};
const characterProperties testRoboto16Characters[] =
    {
        {14, 0},         // A
        {10, 28},        // B
        {12, 48},        // C
        {11, 72},        // D
        {0, 94},         // E is not present in pixel data
        {9, 94},         // F
};
const uint8_t testRoboto16PixelData[] =
    {
        // @0 'A' (14 pixels wide)
        0x80, 0x00,        // #
        0xE0, 0x00,        // ###
        0xF8, 0x00,        // #####
        0x1F, 0x00,        //    #####
        0x0F, 0xE0,        //     #######
        0x0C, 0x78,        //     ##   ####
        0x0C, 0x1E,        //     ##     ####
        0x0C, 0x1E,        //     ##     ####
        0x0C, 0x78,        //     ##   ####
        0x0F, 0xE0,        //     #######
        0x1F, 0x00,        //    #####
        0xF8, 0x00,        // #####
        0xE0, 0x00,        // ###
        0x80, 0x00,        // #

        // @28 'B' (10 pixels wide)
        0xFF, 0xFE,        // ###############
        0xFF, 0xFE,        // ###############
        0xC1, 0x86,        // ##     ##    ##
        0xC1, 0x86,        // ##     ##    ##
        0xC1, 0x86,        // ##     ##    ##
        0xC1, 0x86,        // ##     ##    ##
        0xC1, 0x86,        // ##     ##    ##
        0xE3, 0xFC,        // ###   ########
        0x7F, 0x7C,        //  ####### #####
        0x3C, 0x20,        //   ####    #

        // @48 'C' (12 pixels wide)
        0x0F, 0xE0,        //     #######
        0x3F, 0xF8,        //   ###########
        0x78, 0x3C,        //  ####     ####
        0x60, 0x0C,        //  ##         ##
        0xC0, 0x06,        // ##           ##
        0xC0, 0x06,        // ##           ##
        0xC0, 0x06,        // ##           ##
        0xC0, 0x06,        // ##           ##
        0xE0, 0x0E,        // ###         ###
        0x70, 0x1C,        //  ###       ###
        0x38, 0x38,        //   ###     ###
        0x08, 0x20,        //     #     #

        // @72 'D' (11 pixels wide)
        0xFF, 0xFE,        // ###############
        0xFF, 0xFE,        // ###############
        0xC0, 0x06,        // ##           ##
        0xC0, 0x06,        // ##           ##
        0xC0, 0x06,        // ##           ##
        0xC0, 0x06,        // ##           ##
        0xE0, 0x0E,        // ###         ###
        0x70, 0x0C,        //  ###        ##
        0x3C, 0x78,        //   ####   ####
        0x1F, 0xF0,        //    #########
        0x03, 0x80,        //       ###

        // @94  'F' (9 pixels wide)
        0xFF, 0xFE,        // ###############
        0xFF, 0xFE,        // ###############
        0x03, 0x06,        //       ##     ##
        0x03, 0x06,        //       ##     ##
        0x03, 0x06,        //       ##     ##
        0x03, 0x06,        //       ##     ##
        0x03, 0x06,        //       ##     ##
        0x03, 0x06,        //       ##     ##
        0x00, 0x06,        //              ##
};
font testRoboto16{testRoboto16Properties, testRoboto16Characters, testRoboto16PixelData};

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_initialize() {
    TEST_ASSERT_EQUAL(15, testRoboto16.properties.heightInPixels);
    TEST_ASSERT_EQUAL(2, testRoboto16.properties.bytesPerRow);
    TEST_ASSERT_EQUAL(static_cast<uint8_t>('A'), testRoboto16.properties.asciiStart);
    TEST_ASSERT_EQUAL(static_cast<uint8_t>('F'), testRoboto16.properties.asciiEnd);
}

void test_charIsInBounds() {
    TEST_ASSERT_TRUE(testRoboto16.charIsInBounds('A'));
    TEST_ASSERT_TRUE(testRoboto16.charIsInBounds('F'));

    TEST_ASSERT_FALSE(testRoboto16.charIsInBounds(' '));
    TEST_ASSERT_FALSE(testRoboto16.charIsInBounds('G'));
    TEST_ASSERT_FALSE(testRoboto16.charIsInBounds('E'));
}

void test_getCharacterWidthInPixels() {
    TEST_ASSERT_EQUAL(14, testRoboto16.getCharacterWidthInPixels('A'));
    TEST_ASSERT_EQUAL(10, testRoboto16.getCharacterWidthInPixels('B'));
    TEST_ASSERT_EQUAL(12, testRoboto16.getCharacterWidthInPixels('C'));
    TEST_ASSERT_EQUAL(11, testRoboto16.getCharacterWidthInPixels('D'));
    TEST_ASSERT_EQUAL(0, testRoboto16.getCharacterWidthInPixels('E'));
    TEST_ASSERT_EQUAL(9, testRoboto16.getCharacterWidthInPixels('F'));
    TEST_ASSERT_EQUAL(0, testRoboto16.getCharacterWidthInPixels(' '));
    TEST_ASSERT_EQUAL(0, testRoboto16.getCharacterWidthInPixels('G'));
}

void test_getOffsetInBytes() {
    TEST_ASSERT_EQUAL(0, testRoboto16.getOffsetInBytes('A'));
    TEST_ASSERT_EQUAL(28, testRoboto16.getOffsetInBytes('B'));
    TEST_ASSERT_EQUAL(48, testRoboto16.getOffsetInBytes('C'));
    TEST_ASSERT_EQUAL(72, testRoboto16.getOffsetInBytes('D'));
    TEST_ASSERT_EQUAL(94, testRoboto16.getOffsetInBytes('E'));
    TEST_ASSERT_EQUAL(94, testRoboto16.getOffsetInBytes('F'));
    TEST_ASSERT_EQUAL(0, testRoboto16.getOffsetInBytes(' '));
    TEST_ASSERT_EQUAL(0, testRoboto16.getOffsetInBytes('G'));
}

void test_getOffsetInBytes2() {
    TEST_ASSERT_EQUAL(0, testRoboto16.getOffsetInBytes(0, 0));
    TEST_ASSERT_EQUAL(0, testRoboto16.getOffsetInBytes(0, 7));
    TEST_ASSERT_EQUAL(1, testRoboto16.getOffsetInBytes(0, 8));
    TEST_ASSERT_EQUAL(1, testRoboto16.getOffsetInBytes(0, testRoboto16.properties.heightInPixels - 1));

    TEST_ASSERT_EQUAL(2, testRoboto16.getOffsetInBytes(1, 0));
    TEST_ASSERT_EQUAL(2, testRoboto16.getOffsetInBytes(1, 7));
    TEST_ASSERT_EQUAL(3, testRoboto16.getOffsetInBytes(1, 8));
    TEST_ASSERT_EQUAL(3, testRoboto16.getOffsetInBytes(1, testRoboto16.properties.heightInPixels - 1));

    TEST_ASSERT_EQUAL(26, testRoboto16.getOffsetInBytes(13, 0));
    TEST_ASSERT_EQUAL(26, testRoboto16.getOffsetInBytes(13, 7));
    TEST_ASSERT_EQUAL(27, testRoboto16.getOffsetInBytes(13, 8));
    TEST_ASSERT_EQUAL(27, testRoboto16.getOffsetInBytes(13, testRoboto16.properties.heightInPixels - 1));
}

void test_getBitIndex() {
    TEST_ASSERT_EQUAL(7, font::getBitIndex(0));
    TEST_ASSERT_EQUAL(0, font::getBitIndex(7));
    TEST_ASSERT_EQUAL(7, font::getBitIndex(8));
    TEST_ASSERT_EQUAL(0, font::getBitIndex(15));
}

void test_getPixel() {
    // uint32_t x{13};
    // uint32_t y{0};
    uint8_t testChar = 'A';
    uint32_t offset = testRoboto16.getOffsetInBytes(testChar);
    // TEST_ASSERT_EQUAL(0, offset);
    // uint32_t offset2 = testRoboto16.getOffsetInBytes(x, y);
    // TEST_ASSERT_EQUAL(0, offset2);
    // TEST_ASSERT_EQUAL(0b10000000, 1 << font::getBitIndex(y));

    TEST_ASSERT_TRUE(testRoboto16.getPixel(0, 0, offset));
    TEST_ASSERT_FALSE(testRoboto16.getPixel(0, 1, offset));
    TEST_ASSERT_TRUE(testRoboto16.getPixel(13, 0, offset));
    TEST_ASSERT_FALSE(testRoboto16.getPixel(13, 1, offset));
}


// void test_getPixelOutOfBounds() {
//     uint8_t testChar = 128;
//     TEST_ASSERT_FALSE(couriernew12.getPixel(0, 0, testChar));
//     testChar = 32;
//     TEST_ASSERT_FALSE(couriernew12.getPixel(7, 0, testChar));
// }

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_charIsInBounds);
    RUN_TEST(test_getCharacterWidthInPixels);
    RUN_TEST(test_getOffsetInBytes);
    RUN_TEST(test_getOffsetInBytes2);
    RUN_TEST(test_getBitIndex);
    RUN_TEST(test_getPixel);
    UNITY_END();
}