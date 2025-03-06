// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include <uniqueid.hpp>
#include <ctime>


void setUp(void) {
}
void tearDown(void) {
}

void test_get() {
    TEST_ASSERT_EQUAL(uniqueId::mockUniqueId, uniqueId::get());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_get);
    UNITY_END();
}
