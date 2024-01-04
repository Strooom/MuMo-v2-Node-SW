#include <unity.h>
#include <runresult.hpp>

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_dummy_for_coverage() {
    TEST_ASSERT_EQUAL_STRING("ready", toString(runResult::ready));
    TEST_ASSERT_EQUAL_STRING("busy", toString(runResult::busy));
    TEST_ASSERT_EQUAL_STRING("unknown", toString(static_cast<runResult>(99)));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_dummy_for_coverage);
    UNITY_END();
}
