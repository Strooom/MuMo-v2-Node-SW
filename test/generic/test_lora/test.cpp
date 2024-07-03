#include <unity.h>

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_initialize() {
    TEST_IGNORE_MESSAGE("Implement me!");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    UNITY_END();
}