// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include "display.hpp"

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_isInBounds() {
    TEST_ASSERT_TRUE(display::isInBounds(0, 0));
    TEST_ASSERT_TRUE(display::isInBounds(0, display::heightInPixels - 1));
    TEST_ASSERT_TRUE(display::isInBounds(display::widthInPixels - 1, 0));
    TEST_ASSERT_TRUE(display::isInBounds(display::widthInPixels - 1, display::heightInPixels - 1));

    TEST_ASSERT_FALSE(display::isInBounds(0, display::heightInPixels));
    TEST_ASSERT_FALSE(display::isInBounds(display::widthInPixels, 0));
    TEST_ASSERT_FALSE(display::isInBounds(display::widthInPixels, display::heightInPixels));
}

void test_swapCoordinates() {
    uint32_t c1{1};
    uint32_t c2{2};
    display::swapCoordinates(c1, c2);
    TEST_ASSERT_EQUAL(2, c1);
    TEST_ASSERT_EQUAL(1, c2);
}

void test_mirrorCoordinate() {
    uint32_t c{10};
    display::mirrorCoordinate(c, display::widthInPixels);
    TEST_ASSERT_EQUAL(189, c);
}

void test_mirrorCoordinates() {
    {
        uint32_t xTest{10};
        uint32_t yTest{20};
        display::rotation  = displayRotation::rotation0;
        display::mirroring = displayMirroring::none;
        display::rotateAndMirrorCoordinates(xTest, yTest);
        TEST_ASSERT_EQUAL(10, xTest);
        TEST_ASSERT_EQUAL(20, yTest);
    }
    {
        uint32_t xTest{10};
        uint32_t yTest{20};
        display::mirroring = displayMirroring::horizontal;
        display::rotateAndMirrorCoordinates(xTest, yTest);
        TEST_ASSERT_EQUAL((display::widthInPixels - 1) - 10, xTest);
        TEST_ASSERT_EQUAL(20, yTest);
    }
    {
        uint32_t xTest{10};
        uint32_t yTest{20};
        display::mirroring = displayMirroring::vertical;
        display::rotateAndMirrorCoordinates(xTest, yTest);
        TEST_ASSERT_EQUAL((display::widthInPixels - 1) - 20, yTest);
        TEST_ASSERT_EQUAL(10, xTest);
    }
    {
        uint32_t xTest{10};
        uint32_t yTest{20};
        display::mirroring = displayMirroring::both;
        display::rotateAndMirrorCoordinates(xTest, yTest);
        TEST_ASSERT_EQUAL((display::widthInPixels - 1) - 10, xTest);
        TEST_ASSERT_EQUAL((display::widthInPixels - 1) - 20, yTest);
    }
}

void test_rotateCoordinates() {
    {
        uint32_t xTest{10};
        uint32_t yTest{20};
        display::mirroring = displayMirroring::none;
        display::rotation  = displayRotation::rotation0;
        display::rotateAndMirrorCoordinates(xTest, yTest);
        TEST_ASSERT_EQUAL(10, xTest);
        TEST_ASSERT_EQUAL(20, yTest);
    }
    {
        uint32_t xTest{10};
        uint32_t yTest{20};
        display::rotation = displayRotation::rotation90;
        display::rotateAndMirrorCoordinates(xTest, yTest);
        TEST_ASSERT_EQUAL(20, xTest);
        TEST_ASSERT_EQUAL((display::widthInPixels - 1) - 10, yTest);
    }
    {
        uint32_t xTest{10};
        uint32_t yTest{20};
        display::rotation = displayRotation::rotation180;
        display::rotateAndMirrorCoordinates(xTest, yTest);
        TEST_ASSERT_EQUAL((display::widthInPixels - 1) - 10, xTest);
        TEST_ASSERT_EQUAL((display::widthInPixels - 1) - 20, yTest);
    }
    {
        uint32_t xTest{10};
        uint32_t yTest{20};
        display::rotation = displayRotation::rotation270;
        display::rotateAndMirrorCoordinates(xTest, yTest);
        TEST_ASSERT_EQUAL((display::widthInPixels - 1) - 20, xTest);
        TEST_ASSERT_EQUAL(10, yTest);
    }
}

void test_getByteOffset() {
    TEST_ASSERT_EQUAL(0, display::getByteOffset(0, 0));
    TEST_ASSERT_EQUAL((display::bufferSize - 1), display::getByteOffset((display::widthInPixels - 1), (display::heightInPixels - 1)));
}

void test_getBitOffset() {
    TEST_ASSERT_EQUAL(7, display::getBitOffset(0));
    TEST_ASSERT_EQUAL(0, display::getBitOffset(display::widthInPixels - 1));
}

void test_setGetClearPixel() {
    display::mirroring = displayMirroring::none;
    display::rotation  = displayRotation::rotation0;
    display::clearAllPixels();

    for (auto x = 0; x < display::widthInPixels; x++) {
        for (auto y = 0; y < display::heightInPixels; y++) {
            TEST_ASSERT_FALSE(display::getPixel(x, y));
        }
    }

    // set the pixel in the four corners, and check the related bytes in the buffer and the pixel state
    display::setPixel(0, 0);
    TEST_ASSERT_EQUAL(0b01111111, display::newFrameBuffer[0]);
    TEST_ASSERT_TRUE(display::getPixel(0, 0));

    display::setPixel((display::widthInPixels - 1), 0);
    TEST_ASSERT_EQUAL(0b11111110, display::newFrameBuffer[24]);
    TEST_ASSERT_TRUE(display::getPixel((display::widthInPixels - 1), 0));

    display::setPixel(0, (display::heightInPixels - 1));
    TEST_ASSERT_EQUAL(0b01111111, display::newFrameBuffer[5000 - 25]);
    TEST_ASSERT_TRUE(display::getPixel(0, (display::heightInPixels - 1)));

    display::setPixel((display::widthInPixels - 1), (display::heightInPixels - 1));
    TEST_ASSERT_EQUAL(0b11111110, display::newFrameBuffer[5000 - 1]);
    TEST_ASSERT_TRUE(display::getPixel((display::widthInPixels - 1), (display::heightInPixels - 1)));

    // clear the pixel in the four corners, and check the related bytes in the buffer and the pixel state
    display::clearPixel(0, 0);
    TEST_ASSERT_EQUAL(0b11111111, display::newFrameBuffer[0]);
    TEST_ASSERT_FALSE(display::getPixel(0, 0));

    display::clearPixel((display::widthInPixels - 1), 0);
    TEST_ASSERT_EQUAL(0b11111111, display::newFrameBuffer[24]);
    TEST_ASSERT_FALSE(display::getPixel((display::widthInPixels - 1), 0));

    display::clearPixel(0, (display::heightInPixels - 1));
    TEST_ASSERT_EQUAL(0b11111111, display::newFrameBuffer[5000 - 25]);
    TEST_ASSERT_FALSE(display::getPixel(0, (display::heightInPixels - 1)));

    display::clearPixel((display::widthInPixels - 1), (display::heightInPixels - 1));
    TEST_ASSERT_EQUAL(0b11111111, display::newFrameBuffer[5000 - 1]);
    TEST_ASSERT_FALSE(display::getPixel((display::widthInPixels - 1), (display::heightInPixels - 1)));
}

void test_changePixelOutOfBounds() {
    display::clearAllPixels();
    display::mirroring = displayMirroring::none;
    display::rotation  = displayRotation::rotation0;

    display::setPixel(200, 0);        // set some pixels outside the display area
    display::setPixel(0, 200);
    display::setPixel(200, 200);

    TEST_ASSERT_FALSE(display::getPixel(200, 0));
    TEST_ASSERT_FALSE(display::getPixel(0, 200));
    TEST_ASSERT_FALSE(display::getPixel(200, 200));


    for (uint32_t i = 0; i < display::bufferSize; i++) {
        TEST_ASSERT_EQUAL(0xFF, display::newFrameBuffer[i]);        // all of the displaybuffer should still be cleared
    }

    for (uint32_t i = 0; i < display::bufferSize; i++) {
        display::newFrameBuffer[i] = 0x00;        // set all pixels
    }
    display::clearPixel(200, 0);        // set some pixels outside the display area
    display::clearPixel(0, 200);
    display::clearPixel(200, 200);

    for (uint32_t i = 0; i < display::bufferSize; i++) {
        TEST_ASSERT_EQUAL(0x00, display::newFrameBuffer[i]);        // all of the displaybuffer should still be set
    }
}

void test_read_busy() {
    TEST_ASSERT_TRUE(display::isReady());
    TEST_ASSERT_FALSE(display::isBusy());
}

void test_dummy() {
    display::isPresent();
    display::update();
    display::goSleep();
    TEST_IGNORE_MESSAGE("For testCoverage only");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_isInBounds);
    RUN_TEST(test_swapCoordinates);
    RUN_TEST(test_mirrorCoordinate);
    RUN_TEST(test_mirrorCoordinates);
    RUN_TEST(test_rotateCoordinates);
    RUN_TEST(test_getByteOffset);
    RUN_TEST(test_getBitOffset);
    RUN_TEST(test_setGetClearPixel);
    RUN_TEST(test_changePixelOutOfBounds);
    RUN_TEST(test_read_busy);
    RUN_TEST(test_dummy);
    UNITY_END();
}