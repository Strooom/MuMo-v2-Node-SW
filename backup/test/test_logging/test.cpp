// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include "logging.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_initialize() {
    TEST_ASSERT_EQUAL_CHAR(0x00, logging::buffer[0]);
    TEST_ASSERT_FALSE(logging::isActive(logging::source::applicationEvents));
    TEST_ASSERT_FALSE(logging::isActive(logging::destination::debugProbe));
    logging::enable(logging::destination::debugProbe);
    logging::enable(logging::source::applicationEvents);
    logging::reset();
    TEST_ASSERT_FALSE(logging::isActive(logging::source::applicationEvents));
    TEST_ASSERT_FALSE(logging::isActive(logging::destination::debugProbe));
}

void test_enable_disable() {
    logging::reset();
    TEST_ASSERT_FALSE(logging::isActive(logging::source::applicationEvents));
    logging::enable(logging::source::applicationEvents);
    TEST_ASSERT_TRUE(logging::isActive(logging::source::applicationEvents));
    logging::disable(logging::source::applicationEvents);
    TEST_ASSERT_FALSE(logging::isActive(logging::source::applicationEvents));

    TEST_ASSERT_FALSE(logging::isActive(logging::destination::debugProbe));
    logging::enable(logging::destination::debugProbe);
    TEST_ASSERT_TRUE(logging::isActive(logging::destination::debugProbe));
    logging::disable(logging::destination::debugProbe);
    TEST_ASSERT_FALSE(logging::isActive(logging::destination::debugProbe));
}

void test_output() {
    logging::reset();
    TEST_ASSERT_EQUAL(0, logging::snprintf("test"));          // before any destination is enabled
    logging::enable(logging::destination::debugProbe);        //
    TEST_ASSERT_EQUAL(4, logging::snprintf("test"));          // after a destination is enabled
}

void test_output_source() {
    logging::reset();
    logging::enable(logging::destination::debugProbe);
    TEST_ASSERT_EQUAL(0, logging::snprintf(logging::source::error, "test"));        // before any source is enabled
    logging::enable(logging::source::error);                                        //
    TEST_ASSERT_EQUAL(4, logging::snprintf(logging::source::error, "test"));        // after a source is enabled
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_enable_disable);
    RUN_TEST(test_output);
    RUN_TEST(test_output_source);
    UNITY_END();
}
