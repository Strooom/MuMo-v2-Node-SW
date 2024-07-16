// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include <qrcode.hpp>
#include <string.h>

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}


#pragma region staticFunctions

void test_getAlphanumeric() {
    TEST_ASSERT_EQUAL(0, qrCode::getAlphanumeric('0'));
    TEST_ASSERT_EQUAL(1, qrCode::getAlphanumeric('1'));
    TEST_ASSERT_EQUAL(8, qrCode::getAlphanumeric('8'));
    TEST_ASSERT_EQUAL(9, qrCode::getAlphanumeric('9'));
    TEST_ASSERT_EQUAL(10, qrCode::getAlphanumeric('A'));
    TEST_ASSERT_EQUAL(11, qrCode::getAlphanumeric('B'));
    TEST_ASSERT_EQUAL(34, qrCode::getAlphanumeric('Y'));
    TEST_ASSERT_EQUAL(35, qrCode::getAlphanumeric('Z'));
    TEST_ASSERT_EQUAL(36, qrCode::getAlphanumeric(' '));
    TEST_ASSERT_EQUAL(37, qrCode::getAlphanumeric('$'));
    TEST_ASSERT_EQUAL(38, qrCode::getAlphanumeric('%'));
    TEST_ASSERT_EQUAL(39, qrCode::getAlphanumeric('*'));
    TEST_ASSERT_EQUAL(40, qrCode::getAlphanumeric('+'));
    TEST_ASSERT_EQUAL(41, qrCode::getAlphanumeric('-'));
    TEST_ASSERT_EQUAL(42, qrCode::getAlphanumeric('.'));
    TEST_ASSERT_EQUAL(43, qrCode::getAlphanumeric('/'));
    TEST_ASSERT_EQUAL(44, qrCode::getAlphanumeric(':'));
    TEST_ASSERT_EQUAL(-1, qrCode::getAlphanumeric('?'));
    TEST_ASSERT_EQUAL(-1, qrCode::getAlphanumeric('a'));
}

void test_isAlphanumeric() {
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric("0", 1));
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric("9", 1));
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric("A", 1));
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric("Z", 1));
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric(" ", 1));
    TEST_ASSERT_FALSE(qrCode::isAlphanumeric("?", 1));
    TEST_ASSERT_FALSE(qrCode::isAlphanumeric("?", 1));
    TEST_ASSERT_FALSE(qrCode::isAlphanumeric("\n", 1));

    TEST_ASSERT_TRUE(qrCode::isAlphanumeric("HELLO WORLD", 11));
    TEST_ASSERT_TRUE(qrCode::isAlphanumeric("1234567890", 10));
    TEST_ASSERT_FALSE(qrCode::isAlphanumeric(" ? ", 3));
}

void test_isNumeric() {
    TEST_ASSERT_TRUE(qrCode::isNumeric("1234567890", 10));
    TEST_ASSERT_FALSE(qrCode::isNumeric("?", 1));
    TEST_ASSERT_FALSE(qrCode::isNumeric("\n", 1));
    TEST_ASSERT_FALSE(qrCode::isNumeric("000 000", 7));
}

void test_getModeBits() {
    TEST_ASSERT_EQUAL(10, qrCode::getModeBits(1, encodingFormat::numeric));
    TEST_ASSERT_EQUAL(9, qrCode::getModeBits(1, encodingFormat::alphanumeric));
    TEST_ASSERT_EQUAL(8, qrCode::getModeBits(1, encodingFormat::byte));
    TEST_ASSERT_EQUAL(10, qrCode::getModeBits(9, encodingFormat::numeric));
    TEST_ASSERT_EQUAL(9, qrCode::getModeBits(9, encodingFormat::alphanumeric));
    TEST_ASSERT_EQUAL(8, qrCode::getModeBits(9, encodingFormat::byte));

    TEST_ASSERT_EQUAL(12, qrCode::getModeBits(10, encodingFormat::numeric));
    TEST_ASSERT_EQUAL(11, qrCode::getModeBits(10, encodingFormat::alphanumeric));
    TEST_ASSERT_EQUAL(16, qrCode::getModeBits(10, encodingFormat::byte));
    TEST_ASSERT_EQUAL(12, qrCode::getModeBits(26, encodingFormat::numeric));
    TEST_ASSERT_EQUAL(11, qrCode::getModeBits(26, encodingFormat::alphanumeric));
    TEST_ASSERT_EQUAL(16, qrCode::getModeBits(26, encodingFormat::byte));

    TEST_ASSERT_EQUAL(14, qrCode::getModeBits(27, encodingFormat::numeric));
    TEST_ASSERT_EQUAL(13, qrCode::getModeBits(27, encodingFormat::alphanumeric));
    TEST_ASSERT_EQUAL(16, qrCode::getModeBits(27, encodingFormat::byte));
    TEST_ASSERT_EQUAL(14, qrCode::getModeBits(40, encodingFormat::numeric));
    TEST_ASSERT_EQUAL(13, qrCode::getModeBits(40, encodingFormat::alphanumeric));
    TEST_ASSERT_EQUAL(16, qrCode::getModeBits(40, encodingFormat::byte));
}


void test_max() {
    TEST_ASSERT_EQUAL(5, qrCode::max(0, 5));
    TEST_ASSERT_EQUAL(5, qrCode::max(5, 0));
    TEST_ASSERT_EQUAL(5, qrCode::max(5, 5));
    TEST_ASSERT_EQUAL(5, qrCode::max(5, -5));
    TEST_ASSERT_EQUAL(5, qrCode::max(-5, 5));
    TEST_ASSERT_EQUAL(-5, qrCode::max(-5, -5));
}


#pragma endregion


#pragma region testHelpers



#pragma endregion

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_max);
    RUN_TEST(test_getAlphanumeric);
    RUN_TEST(test_isAlphanumeric);
    RUN_TEST(test_isNumeric);
    RUN_TEST(test_getModeBits);
 
    UNITY_END();
}