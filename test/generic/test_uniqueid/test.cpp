// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include <uniqueid.hpp>
#include <ctime>


void setUp(void) {}
void tearDown(void) {}

void test_getAsUint64() {
    TEST_ASSERT_EQUAL(uniqueId::mockUniqueId, uniqueId::asUint64());
}

void test_getAsString() {
    char expected[17];
    TEST_ASSERT_EQUAL_STRING("1122334455667788", uniqueId::asHexString());
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_getAsUint64);
    RUN_TEST(test_getAsString);
    UNITY_END();
}
