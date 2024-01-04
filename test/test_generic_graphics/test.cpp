// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include "graphics.hpp"
#include "display.hpp"
#include "font.hpp"

fontProperties testRoboto16Properties{15U, 2U, static_cast<uint8_t>('A'), static_cast<uint8_t>('F')};
const characterProperties testRoboto16Characters[] =
    {
        {14, 0},         // A
        {10, 28},        // B
        {12, 48},        // C
        {11, 72},        // D
        {9, 94},         // E
        {9, 112},        // F
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

        // @94 'E' (9 pixels wide)
        0xFF, 0xFE,        // ###############
        0xFF, 0xFE,        // ###############
        0xC1, 0x86,        // ##     ##    ##
        0xC1, 0x86,        // ##     ##    ##
        0xC1, 0x86,        // ##     ##    ##
        0xC1, 0x86,        // ##     ##    ##
        0xC1, 0x86,        // ##     ##    ##
        0xC1, 0x86,        // ##     ##    ##
        0xC0, 0x06,        // ##           ##

        // @112 'F' (9 pixels wide)
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

void setUp(void) {
    display::clearAllPixels();
    display::mirroring = displayMirroring::none;
    display::rotation  = displayRotation::rotation0;
}

void tearDown(void) {}        // after test

void test_sort() {
    uint32_t ten{10};
    uint32_t twenty{20};
    graphics::sort(ten, twenty);
    TEST_ASSERT_EQUAL(10, ten);
    TEST_ASSERT_EQUAL(20, twenty);
    graphics::sort(twenty, ten);
    TEST_ASSERT_EQUAL(20, ten);
    TEST_ASSERT_EQUAL(10, twenty);
}

void test_drawPixel() {
    uint32_t testX{10};
    uint32_t testY{10};

    graphics::drawPixel(testX, testY, graphics::color::black);
    TEST_ASSERT_TRUE(display::getPixel(testX, testY));
    TEST_ASSERT_FALSE(display::getPixel(testX - 1, testY));
    TEST_ASSERT_FALSE(display::getPixel(testX + 1, testY));
    TEST_ASSERT_FALSE(display::getPixel(testX, testY - 1));
    TEST_ASSERT_FALSE(display::getPixel(testX, testY + 1));

    graphics::drawPixel(testX, testY, graphics::color::white);
    TEST_ASSERT_FALSE(display::getPixel(testX, testY));
    TEST_ASSERT_FALSE(display::getPixel(testX - 1, testY));
    TEST_ASSERT_FALSE(display::getPixel(testX + 1, testY));
    TEST_ASSERT_FALSE(display::getPixel(testX, testY - 1));
    TEST_ASSERT_FALSE(display::getPixel(testX, testY + 1));
}

void test_drawHorizontalLine() {
    uint32_t startX{10};
    uint32_t endX{40};
    uint32_t testY{10};

    graphics::drawHorizontalLine(startX, endX, testY, graphics::color::black);
    for (auto i = startX; i < endX; i++) {
        TEST_ASSERT_TRUE(display::getPixel(i, testY));
        TEST_ASSERT_FALSE(display::getPixel(i, testY - 1));
        TEST_ASSERT_FALSE(display::getPixel(i, testY + 1));
    }
    TEST_ASSERT_FALSE(display::getPixel(startX - 1, testY));
    TEST_ASSERT_FALSE(display::getPixel(endX + 1, testY));
}

void test_drawVerticalLine() {
    uint32_t startY{10};
    uint32_t endY{40};
    uint32_t testX{20};

    graphics::drawVerticalLine(testX, startY, endY, graphics::color::black);
    for (auto i = startY; i < endY; i++) {
        TEST_ASSERT_TRUE(display::getPixel(testX, i));
        TEST_ASSERT_FALSE(display::getPixel(testX - 1, i));
        TEST_ASSERT_FALSE(display::getPixel(testX + 1, i));
    }
    TEST_ASSERT_FALSE(display::getPixel(testX, startY - 1));
    TEST_ASSERT_FALSE(display::getPixel(testX, endY + 1));
}

void test_drawRectangle() {
    graphics::drawRectangle(10, 10, 12, 12, graphics::color::black);
    TEST_ASSERT_TRUE(display::getPixel(11, 10));
    TEST_ASSERT_FALSE(display::getPixel(11, 11));
    TEST_ASSERT_TRUE(display::getPixel(11, 12));

    TEST_ASSERT_TRUE(display::getPixel(10, 11));
    TEST_ASSERT_FALSE(display::getPixel(11, 11));
    TEST_ASSERT_TRUE(display::getPixel(12, 11));
}

void test_drawFilledRectangle() {
    graphics::drawFilledRectangle(10, 10, 11, 11, graphics::color::black);
    TEST_ASSERT_TRUE(display::getPixel(10, 10));
    TEST_ASSERT_TRUE(display::getPixel(10, 11));
    TEST_ASSERT_TRUE(display::getPixel(11, 10));
    TEST_ASSERT_TRUE(display::getPixel(11, 11));

    TEST_ASSERT_FALSE(display::getPixel(9, 10));
    TEST_ASSERT_FALSE(display::getPixel(12, 10));
    TEST_ASSERT_FALSE(display::getPixel(9, 11));
    TEST_ASSERT_FALSE(display::getPixel(12, 11));

    TEST_ASSERT_FALSE(display::getPixel(10, 9));
    TEST_ASSERT_FALSE(display::getPixel(10, 12));
    TEST_ASSERT_FALSE(display::getPixel(11, 9));
    TEST_ASSERT_FALSE(display::getPixel(11, 12));
}

// void test_drawBitMap() {
//     const uint8_t testPixelData[8] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
//     bitmap testBitMap1{8, 8, testPixelData};
//     display::clearAllPixels();
//     display::mirroring = displayMirroring::none;
//     display::rotation  = displayRotation::rotation0;
//     graphics::drawBitMap(8, 8, testBitMap1);

//     for (auto i = 0; i < (25 * 8); i++) {
//         TEST_ASSERT_EQUAL(0, display::displayBuffer[i]);
//     }
//     for (auto i = 0; i < 8; i++) {
//         TEST_ASSERT_EQUAL(0, display::displayBuffer[(25 * 8) + (25 * i)]);
//         TEST_ASSERT_EQUAL(0xAA, display::displayBuffer[(25 * 8) + (25 * i) + 1]);
//         TEST_ASSERT_EQUAL(0, display::displayBuffer[(25 * 8) + (25 * i) + 2]);
//     }
// }

// void test_drawCharacter() {
//     display::clearAllPixels();
//     display::mirroring = displayMirroring::none;
//     display::rotation  = displayRotation::rotation0;
//     graphics::drawCharacter(8, 8, testRoboto16, '_');
//     TEST_ASSERT_EQUAL(0xFE, display::displayBuffer[(25 * 8) + 1]);
// }

// void test_drawText() {
//     display::clearAllPixels();
//     display::mirroring    = displayMirroring::none;
//     display::rotation     = displayRotation::rotation0;
//     const char testText[] = "_";
//     graphics::drawText(8, 8, testRoboto16, testText);
//     TEST_ASSERT_EQUAL(0xFE, display::displayBuffer[(25 * 8) + 1]);
// }

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_sort);
    RUN_TEST(test_drawPixel);
    RUN_TEST(test_drawHorizontalLine);
    RUN_TEST(test_drawVerticalLine);
    RUN_TEST(test_drawRectangle);
    RUN_TEST(test_drawFilledRectangle);
    // RUN_TEST(test_drawBitMap);
    // RUN_TEST(test_drawCharacter);
    // RUN_TEST(test_drawText);
    UNITY_END();
}