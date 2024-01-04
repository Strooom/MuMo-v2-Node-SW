#include <unity.h>
#include "cli.h"

void setUp(void) {}
void tearDown(void) {}

void test_initialize() {
    TEST_IGNORE_MESSAGE("Implement me!");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    UNITY_END();
}