#include <unity.h>
#include "buildinfo.hpp"

void setUp(void) {}
void tearDown(void) {}


void test_toString() {
    // For test coverage only
    TEST_ASSERT_EQUAL_STRING("unknown", toString(buildType::unknown));
    TEST_ASSERT_EQUAL_STRING("development", toString(buildType::development));
    TEST_ASSERT_EQUAL_STRING("production", toString(buildType::production));
    TEST_ASSERT_EQUAL_STRING("integrationTest", toString(buildType::integrationTest));

    TEST_ASSERT_EQUAL_STRING("unknown", toString(buildEnvironment::unknown));
    TEST_ASSERT_EQUAL_STRING("local", toString(buildEnvironment::local));
    TEST_ASSERT_EQUAL_STRING("ci", toString(buildEnvironment::ci));
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_toString);
    UNITY_END();
}