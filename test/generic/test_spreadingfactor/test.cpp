#include <unity.h>
#include <power.hpp>
#include <spreadingfactor.hpp>

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_dummy_for_coverage() {
    TEST_ASSERT_EQUAL_STRING("SF7", toString(spreadingFactor::SF7));
    TEST_ASSERT_EQUAL_STRING("SF8", toString(spreadingFactor::SF8));
    TEST_ASSERT_EQUAL_STRING("SF9", toString(spreadingFactor::SF9));
    TEST_ASSERT_EQUAL_STRING("SF10", toString(spreadingFactor::SF10));
    TEST_ASSERT_EQUAL_STRING("SF11", toString(spreadingFactor::SF11));
    TEST_ASSERT_EQUAL_STRING("SF12", toString(spreadingFactor::SF12));

    TEST_ASSERT_EQUAL_STRING("Unknown spreading factor", toString(static_cast<spreadingFactor>(0)));

}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_dummy_for_coverage);
    UNITY_END();
}
