// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <unity.h>
#include <polynome.hpp>
#include <reedsolomon.hpp>
#include <cstring>
#include <cstdio>

void setUp(void) {}
void tearDown(void) {}

void test_initialize_polynome() {
    {
        polynome testPolynome = polynome();
        TEST_ASSERT_EQUAL(0, testPolynome.getOrder());
        TEST_ASSERT_EACH_EQUAL_UINT8(0, testPolynome.coefficients, polynome::maxNmbrOfTerms);
    }
    {
        uint32_t testPolynomeOrder{10};
        polynome testPolynome = polynome(testPolynomeOrder);
        TEST_ASSERT_EQUAL(testPolynomeOrder, testPolynome.getOrder());
        TEST_ASSERT_EACH_EQUAL_UINT8(0, testPolynome.coefficients, testPolynomeOrder);
    }
    {
        static constexpr uint32_t testPolynomeOrder{4};
        uint8_t testPolynomeCoefficients[testPolynomeOrder + 1]{10, 20, 30, 40, 50};
        polynome testPolynome(testPolynomeCoefficients, testPolynomeOrder);
        TEST_ASSERT_EQUAL(testPolynomeOrder, testPolynome.getOrder());
        TEST_ASSERT_EQUAL_UINT8_ARRAY(testPolynomeCoefficients, testPolynome.coefficients, testPolynomeOrder + 1);
    }
    {
        uint32_t testPolynomeOrder{polynome::maxNmbrOfTerms - 1};        // maximum size... edge case
        polynome testPolynome = polynome(testPolynomeOrder);
        TEST_ASSERT_EQUAL(polynome::maxNmbrOfTerms - 1, testPolynome.getOrder());
        TEST_ASSERT_EACH_EQUAL_UINT8(0, testPolynome.coefficients, polynome::maxNmbrOfTerms - 1);
    }
    {
        uint32_t testPolynomeOrder{polynome::maxNmbrOfTerms};        // too large... edge case
        polynome testPolynome = polynome(testPolynomeOrder);
        TEST_ASSERT_EQUAL(0, testPolynome.getOrder());
    }
}

void test_isZero_polynome() {
    {
        polynome testPolynome(2);
        TEST_ASSERT_TRUE(testPolynome.isZero());
    }
    {
        static constexpr uint32_t testPolynomeOrder{2};
        uint8_t testPolynomeCoefficients[testPolynomeOrder + 1]{10, 20, 30};
        polynome testPolynome(testPolynomeCoefficients, testPolynomeOrder);
        TEST_ASSERT_FALSE(testPolynome.isZero());
    }
}

void test_firstNonZeroTermIndex() {
    {
        static constexpr uint32_t testPolynomeOrder{2};
        uint8_t testPolynomeCoefficients[testPolynomeOrder + 1]{10, 20, 30};
        polynome testPolynome(testPolynomeCoefficients, testPolynomeOrder);
        TEST_ASSERT_EQUAL(0, testPolynome.firstNonZeroTermIndex());
    }
    {
        static constexpr uint32_t testPolynomeOrder{2};
        uint8_t testPolynomeCoefficients[testPolynomeOrder + 1]{0, 20, 30};
        polynome testPolynome(testPolynomeCoefficients, testPolynomeOrder);
        TEST_ASSERT_EQUAL(1, testPolynome.firstNonZeroTermIndex());
    }
    {
        static constexpr uint32_t testPolynomeOrder{2};
        uint8_t testPolynomeCoefficients[testPolynomeOrder + 1]{0, 0, 30};
        polynome testPolynome(testPolynomeCoefficients, testPolynomeOrder);
        TEST_ASSERT_EQUAL(2, testPolynome.firstNonZeroTermIndex());
    }
}

void test_firstNonZeroTerm() {
    {
        static constexpr uint32_t testPolynomeOrder{2};
        uint8_t testPolynomeCoefficients[testPolynomeOrder + 1]{10, 20, 30};
        polynome testPolynome(testPolynomeCoefficients, testPolynomeOrder);
        TEST_ASSERT_EQUAL(10, testPolynome.firstNonZeroTerm());
    }
    {
        static constexpr uint32_t testPolynomeOrder{2};
        uint8_t testPolynomeCoefficients[testPolynomeOrder + 1]{0, 20, 30};
        polynome testPolynome(testPolynomeCoefficients, testPolynomeOrder);
        TEST_ASSERT_EQUAL(20, testPolynome.firstNonZeroTerm());
    }
    {
        static constexpr uint32_t testPolynomeOrder{2};
        uint8_t testPolynomeCoefficients[testPolynomeOrder + 1]{0, 0, 30};
        polynome testPolynome(testPolynomeCoefficients, testPolynomeOrder);
        TEST_ASSERT_EQUAL(30, testPolynome.firstNonZeroTerm());
    }
}

void test_multiplyScalar() {
    static constexpr uint32_t testPolynomeOrder{2};
    uint8_t testPolynomeCoefficients[testPolynomeOrder + 1]{7, 8, 9};        // using examples from gf256_log_antilog.pdf
    polynome testPolynome(testPolynomeCoefficients, testPolynomeOrder);
    testPolynome.multiplyScalar(10);
    uint8_t expectedPolynomeCoefficients[testPolynomeOrder + 1]{54, 80, 90};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedPolynomeCoefficients, testPolynome.coefficients, testPolynomeOrder + 1);
}

void test_multiplyPolynomes() {
    static constexpr uint32_t testPolynomeOrder{2};
    uint8_t polynomeLeftCoefficients[testPolynomeOrder + 1]{1, 2, 3};
    uint8_t polynomeRightCoefficients[testPolynomeOrder + 1]{4, 5, 6};
    polynome polynomeLeft(polynomeLeftCoefficients, testPolynomeOrder);
    polynome polynomeRight(polynomeRightCoefficients, testPolynomeOrder);
    polynomeLeft.multiply(polynomeRight);

    uint32_t expectedPolynomeOrder{testPolynomeOrder * 2};

    TEST_ASSERT_EQUAL(expectedPolynomeOrder, polynomeLeft.order);
    static constexpr uint8_t expected[5]{4, 13, 0, 3, 10};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, polynomeLeft.coefficients, expectedPolynomeOrder + 1);
}

void test_setGeneratorFactor() {
    polynome testPolynome(1);
    testPolynome.setGeneratorFactor(0);
    TEST_ASSERT_EQUAL(1, testPolynome.order);
    TEST_ASSERT_EQUAL_UINT8(1, testPolynome.coefficients[0]);
    TEST_ASSERT_EQUAL_UINT8(1, testPolynome.coefficients[1]);

    testPolynome.setGeneratorFactor(16);
    TEST_ASSERT_EQUAL(1, testPolynome.order);
    TEST_ASSERT_EQUAL_UINT8(1, testPolynome.coefficients[0]);
    TEST_ASSERT_EQUAL_UINT8(76, testPolynome.coefficients[1]);

    testPolynome.setGeneratorFactor(32);
    TEST_ASSERT_EQUAL(1, testPolynome.order);
    TEST_ASSERT_EQUAL_UINT8(1, testPolynome.coefficients[0]);
    TEST_ASSERT_EQUAL_UINT8(157, testPolynome.coefficients[1]);
}

void test_generateGeneratorPolynome() {
    {
        polynome testPolynome = polynome();
        static constexpr uint32_t expectedOrder{2};
        testPolynome.generateGeneratorPolynome(expectedOrder);
        static constexpr uint8_t expectedCoefficients[expectedOrder + 1]{1, 3, 2};
        TEST_ASSERT_EQUAL_UINT32(expectedOrder, testPolynome.getOrder());
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedCoefficients, testPolynome.coefficients, expectedOrder + 1);
    }
    {
        polynome testPolynome = polynome();
        static constexpr uint32_t expectedOrder{7};
        testPolynome.generateGeneratorPolynome(expectedOrder);
        static constexpr uint8_t expectedCoefficients[expectedOrder + 1]{1, 127, 122, 154, 164, 11, 68, 117};
        TEST_ASSERT_EQUAL_UINT32(expectedOrder, testPolynome.getOrder());
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedCoefficients, testPolynome.coefficients, expectedOrder + 1);
    }
    {
        polynome testPolynome = polynome();
        static constexpr uint32_t expectedOrder{16};
        testPolynome.generateGeneratorPolynome(expectedOrder);
        static constexpr uint8_t expectedCoefficients[expectedOrder + 1]{1, 59, 13, 104, 189, 68, 209, 30, 8, 163, 65, 41, 229, 98, 50, 36, 59};
        TEST_ASSERT_EQUAL_UINT32(expectedOrder, testPolynome.getOrder());
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedCoefficients, testPolynome.coefficients, expectedOrder + 1);
    }
}

void test_remainder() {
    // using https://www.thonky.com/qr-code-tutorial/show-division-steps to generate test-vectors
    {
        static constexpr uint32_t messagePolynomeOrder{4};
        uint8_t messagePolynomeCoefficients[messagePolynomeOrder + 1]{10, 20, 30, 40, 50};
        polynome messagePolynome(messagePolynomeCoefficients, messagePolynomeOrder);
        static constexpr uint32_t generatorPolynomeOrder{2};
        messagePolynome.remainder(generatorPolynomeOrder);
        static constexpr uint8_t expected[generatorPolynomeOrder]{46, 52};
        TEST_ASSERT_EQUAL_UINT32(generatorPolynomeOrder - 1, messagePolynome.getOrder());
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, messagePolynome.coefficients, generatorPolynomeOrder);
    }
    {
        static constexpr uint32_t messagePolynomeOrder{15};
        uint8_t messagePolynomeCoefficients[messagePolynomeOrder + 1]{32, 91, 11, 120, 209, 114, 220, 77, 67, 64, 236, 17, 236, 17, 236, 17};
        polynome messagePolynome(messagePolynomeCoefficients, messagePolynomeOrder);
        static constexpr uint32_t generatorPolynomeOrder{10};
        messagePolynome.remainder(generatorPolynomeOrder);
        static constexpr uint8_t expected[generatorPolynomeOrder]{196, 35, 39, 119, 235, 215, 231, 226, 93, 23};
        TEST_ASSERT_EQUAL_UINT32(generatorPolynomeOrder - 1, messagePolynome.getOrder());
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, messagePolynome.coefficients, generatorPolynomeOrder);
    }
    {
        static constexpr uint32_t messagePolynomeOrder{27};
        uint8_t messagePolynomeCoefficients[messagePolynomeOrder + 1]{65, 118, 135, 71, 71, 7, 51, 162, 242, 247, 119, 119, 114, 231, 23, 38, 54, 246, 70, 82, 230, 54, 246, 210, 240, 236, 17, 236};
        polynome messagePolynome(messagePolynomeCoefficients, messagePolynomeOrder);
        static constexpr uint32_t generatorPolynomeOrder{16};
        messagePolynome.remainder(generatorPolynomeOrder);
        static constexpr uint8_t expected[generatorPolynomeOrder]{52, 61, 242, 187, 29, 7, 216, 249, 103, 87, 95, 69, 188, 134, 57, 20};
        TEST_ASSERT_EQUAL_UINT32(generatorPolynomeOrder - 1, messagePolynome.getOrder());
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, messagePolynome.coefficients, generatorPolynomeOrder);
    }

    // Test-vector from https://www.nayuki.io/page/creating-a-qr-code-step-by-step using "0123456789" / low as input
    {
        static constexpr uint32_t messagePolynomeOrder{18};
        uint8_t messagePolynomeCoefficients[messagePolynomeOrder + 1]{16, 40, 12, 86, 106, 105, 0, 236, 17, 236, 17, 236, 17, 236, 17, 236, 17, 236, 17};
        polynome messagePolynome(messagePolynomeCoefficients, messagePolynomeOrder);
        static constexpr uint32_t generatorPolynomeOrder{7};
        messagePolynome.remainder(generatorPolynomeOrder);
        static constexpr uint8_t expected[generatorPolynomeOrder]{39, 170, 38, 8, 235, 255, 214};
        TEST_ASSERT_EQUAL_UINT32(generatorPolynomeOrder - 1, messagePolynome.getOrder());
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, messagePolynome.coefficients, generatorPolynomeOrder);
    }
}

void test_reedSolomon() {
    // using https://www.thonky.com/qr-code-tutorial/show-division-steps to generate test-vectors
    {
        static constexpr uint32_t messagePolynomeOrder{4};
        uint8_t messagePolynomeCoefficients[messagePolynomeOrder + 1]{10, 20, 30, 40, 50};
        polynome messagePolynome(messagePolynomeCoefficients, messagePolynomeOrder);
        static constexpr uint32_t generatorPolynomeOrder{2};
        uint8_t ecc[generatorPolynomeOrder];
        static constexpr uint8_t expected[generatorPolynomeOrder]{46, 52};

        reedSolomon::getErrorCorrectionBytes(ecc, generatorPolynomeOrder, messagePolynomeCoefficients, messagePolynomeOrder + 1);

        TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, ecc, generatorPolynomeOrder);
    }
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize_polynome);
    RUN_TEST(test_isZero_polynome);
    RUN_TEST(test_firstNonZeroTermIndex);
    RUN_TEST(test_firstNonZeroTerm);
    RUN_TEST(test_multiplyScalar);
    RUN_TEST(test_multiplyPolynomes);
    RUN_TEST(test_setGeneratorFactor);
    RUN_TEST(test_generateGeneratorPolynome);
    RUN_TEST(test_remainder);
    RUN_TEST(test_reedSolomon);
    UNITY_END();
}
