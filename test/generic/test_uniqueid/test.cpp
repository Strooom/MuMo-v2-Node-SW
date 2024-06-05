// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include <uniqueid.hpp>
#include <ctime>

extern uint64_t mockUniqueId;

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_get() {
    TEST_ASSERT_EQUAL(mockUniqueId, uniqueId::get());
}

void test_dump() {
    uniqueId::dump();
    TEST_IGNORE_MESSAGE("ToDo : Test on Target");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_get);
    RUN_TEST(test_dump);
    UNITY_END();
}
