// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include <gf256.hpp>
#include <polynome.hpp>
#include <cstring>
#include <cstdio>

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_initialize() {
    polynome testPolynome1(10);
    TEST_ASSERT_EQUAL(10, testPolynome1.order);
    polynome testPolynome2(polynome::maxNmbrOfTerms);
    TEST_ASSERT_EQUAL(0, testPolynome2.order);
}

void test_isZero() {
    polynome testPolynome(10);
    TEST_ASSERT_EQUAL(true, testPolynome.isZero());
    testPolynome.generatePolynome(2);
    TEST_ASSERT_EQUAL(false, testPolynome.isZero());
}

void test_firstNonZeroTermIndex() {
    polynome testPolynome(10);
    testPolynome.generatePolynome(2);
    TEST_ASSERT_EQUAL(0, testPolynome.firstNonZeroTermIndex());
    testPolynome.coefficients[0] = 0;
    TEST_ASSERT_EQUAL(1, testPolynome.firstNonZeroTermIndex());
    testPolynome.coefficients[1] = 0;
    TEST_ASSERT_EQUAL(2, testPolynome.firstNonZeroTermIndex());
}

void test_firstNonZeroTerm() {
    polynome testPolynome(10);
    testPolynome.generatePolynome(2);
    TEST_ASSERT_EQUAL_UINT8(1, testPolynome.firstNonZeroTerm());
    testPolynome.coefficients[0] = 0;
    TEST_ASSERT_EQUAL_UINT8(3, testPolynome.firstNonZeroTerm());
    testPolynome.coefficients[1] = 0;
    TEST_ASSERT_EQUAL_UINT8(2, testPolynome.firstNonZeroTerm());
}

void test_setGenerator() {
    polynome testPolynome(1);
    testPolynome.setGeneratorPolynome(0);
    TEST_ASSERT_EQUAL(1, testPolynome.order);
    TEST_ASSERT_EQUAL_UINT8(1, testPolynome.coefficients[0]);
    TEST_ASSERT_EQUAL_UINT8(1, testPolynome.coefficients[1]);

    testPolynome.setGeneratorPolynome(1);
    TEST_ASSERT_EQUAL(1, testPolynome.order);
    TEST_ASSERT_EQUAL_UINT8(1, testPolynome.coefficients[0]);
    TEST_ASSERT_EQUAL_UINT8(2, testPolynome.coefficients[1]);

    testPolynome.setGeneratorPolynome(2);
    TEST_ASSERT_EQUAL(1, testPolynome.order);
    TEST_ASSERT_EQUAL_UINT8(1, testPolynome.coefficients[0]);
    TEST_ASSERT_EQUAL_UINT8(4, testPolynome.coefficients[1]);

    testPolynome.setGeneratorPolynome(15);
    TEST_ASSERT_EQUAL(1, testPolynome.order);
    TEST_ASSERT_EQUAL_UINT8(1, testPolynome.coefficients[0]);
    TEST_ASSERT_EQUAL_UINT8(38, testPolynome.coefficients[1]);
}

void test_multiplyScalar() {
    static constexpr uint32_t testMaxOrder{20};
    uint8_t coefStorage[polynome::maxNmbrOfTerms]{};
    polynome generator = polynome();
    generator.generatePolynome(2);
    generator.multiplyScalar(10);
    TEST_ASSERT_EQUAL(2, generator.order);
    static constexpr uint8_t expected[polynome::maxNmbrOfTerms]{10, 30, 20};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, generator.coefficients, 2);
}

void test_multiply() {
    polynome polynome1(1);
    polynome polynome2(1);
    polynome1.setGeneratorPolynome(0);
    polynome2.setGeneratorPolynome(1);
    polynome1.multiply(polynome2);
    TEST_ASSERT_EQUAL(2, polynome1.order);
    static constexpr uint8_t expected[3]{1, 3, 2};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, polynome1.coefficients, 3);

    polynome1.setGeneratorPolynome(0);
    polynome2.setGeneratorPolynome(1);
    polynome2.multiply(polynome1);
    TEST_ASSERT_EQUAL(2, polynome2.order);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, polynome2.coefficients, 3);
}

void test_generator() {
    polynome result = polynome();

    result.generatePolynome(2);
    static constexpr uint8_t expected[polynome::maxNmbrOfTerms]{1, 3, 2};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, result.coefficients, 3);

    result.generatePolynome(16);
    static constexpr uint8_t expected2[polynome::maxNmbrOfTerms]{1, 59, 13, 104, 189, 68, 209, 30, 8, 163, 65, 41, 229, 98, 50, 36, 59};        // test-vector from https://dev.to/maxart2501/let-s-develop-a-qr-code-generator-part-iii-error-correction-1kbm
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected2, result.coefficients, 17);

    result.generatePolynome(7);
    static constexpr uint8_t expected3[polynome::maxNmbrOfTerms]{1, 127, 122, 154, 164, 11, 68, 117};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected3, result.coefficients, 8);
}

void test_remainder() {
    polynome message1(4);
    message1.coefficients[0] = 10;
    message1.coefficients[1] = 20;
    message1.coefficients[2] = 30;
    message1.coefficients[3] = 40;
    message1.coefficients[4] = 50;

    message1.remainder(2U);
    static constexpr uint8_t expected[polynome::maxNmbrOfTerms]{0, 0, 0, 0, 0, 46, 52};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, message1.coefficients, 7);

    polynome message2(15);

    message2.coefficients[0]  = 32;
    message2.coefficients[1]  = 91;
    message2.coefficients[2]  = 11;
    message2.coefficients[3]  = 120;
    message2.coefficients[4]  = 209;
    message2.coefficients[5]  = 114;
    message2.coefficients[6]  = 220;
    message2.coefficients[7]  = 77;
    message2.coefficients[8]  = 67;
    message2.coefficients[9]  = 64;
    message2.coefficients[10] = 236;
    message2.coefficients[11] = 17;
    message2.coefficients[12] = 236;
    message2.coefficients[13] = 17;
    message2.coefficients[14] = 236;
    message2.coefficients[15] = 17;

    message2.remainder(10U);
    static constexpr uint8_t expected2[polynome::maxNmbrOfTerms]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 196, 35, 39, 119, 235, 215, 231, 226, 93, 23};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected2, message2.coefficients, 26);

    polynome message3(27);
    message3.coefficients[0]  = 65;
    message3.coefficients[1]  = 118;
    message3.coefficients[2]  = 135;
    message3.coefficients[3]  = 71;
    message3.coefficients[4]  = 71;
    message3.coefficients[5]  = 7;
    message3.coefficients[6]  = 51;
    message3.coefficients[7]  = 162;
    message3.coefficients[8]  = 242;
    message3.coefficients[9]  = 247;
    message3.coefficients[10] = 119;
    message3.coefficients[11] = 119;
    message3.coefficients[12] = 114;
    message3.coefficients[13] = 231;
    message3.coefficients[14] = 23;
    message3.coefficients[15] = 38;
    message3.coefficients[16] = 54;
    message3.coefficients[17] = 246;
    message3.coefficients[18] = 70;
    message3.coefficients[19] = 82;
    message3.coefficients[20] = 230;
    message3.coefficients[21] = 54;
    message3.coefficients[22] = 246;
    message3.coefficients[23] = 210;
    message3.coefficients[24] = 240;
    message3.coefficients[25] = 236;
    message3.coefficients[26] = 17;
    message3.coefficients[27] = 236;


    message3.remainder(16U);
    static constexpr uint8_t expected3[polynome::maxNmbrOfTerms]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 61, 242, 187, 29, 7, 216, 249, 103, 87, 95, 69, 188, 134, 57, 20};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected3, message3.coefficients, 44);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_isZero);
    RUN_TEST(test_firstNonZeroTermIndex);
    RUN_TEST(test_firstNonZeroTerm);
    RUN_TEST(test_setGenerator);
    RUN_TEST(test_multiply);
    RUN_TEST(test_generator);
    RUN_TEST(test_multiplyScalar);
    RUN_TEST(test_remainder);
    UNITY_END();
}
