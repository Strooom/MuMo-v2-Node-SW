// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include <logging.hpp>

void setUp(void) {}
void tearDown(void) {}

void test_initialize() {
    TEST_ASSERT_EQUAL_CHAR(0x00, logging::buffer[0]);
    TEST_ASSERT_FALSE(logging::isActive(logging::source::applicationEvents));
    TEST_ASSERT_FALSE(logging::isActive(logging::destination::swo));
    logging::enable(logging::destination::swo);
    logging::enable(logging::source::applicationEvents);
    logging::reset();
    TEST_ASSERT_FALSE(logging::isActive(logging::source::applicationEvents));
    TEST_ASSERT_FALSE(logging::isActive(logging::destination::swo));
}

void test_enable_disable() {
    logging::reset();
    TEST_ASSERT_FALSE(logging::isActive(logging::source::applicationEvents));
    logging::enable(logging::source::applicationEvents);
    TEST_ASSERT_TRUE(logging::isActive(logging::source::applicationEvents));
    logging::disable(logging::source::applicationEvents);
    TEST_ASSERT_FALSE(logging::isActive(logging::source::applicationEvents));

    TEST_ASSERT_FALSE(logging::isActive(logging::destination::swo));
    logging::enable(logging::destination::swo);
    TEST_ASSERT_TRUE(logging::isActive(logging::destination::swo));
    logging::disable(logging::destination::swo);
    TEST_ASSERT_FALSE(logging::isActive(logging::destination::swo));
}

void test_output() {
    logging::reset();
    TEST_ASSERT_EQUAL(0, logging::snprintf("test"));          // before any destination is enabled
    logging::enable(logging::destination::swo);        //
    TEST_ASSERT_EQUAL(4, logging::snprintf("test"));          // after a destination is enabled
}

void test_output_source() {
    logging::reset();
    logging::enable(logging::destination::swo);
    TEST_ASSERT_EQUAL(0, logging::snprintf(logging::source::error, "test"));        // before any source is enabled
    logging::enable(logging::source::error);                                        //
    TEST_ASSERT_EQUAL(4, logging::snprintf(logging::source::error, "test"));        // after a source is enabled
}

void test_toString_destination() {
    TEST_ASSERT_EQUAL_STRING("swo", toString(logging::destination::swo));
    TEST_ASSERT_EQUAL_STRING("uart1", toString(logging::destination::uart1));
    TEST_ASSERT_EQUAL_STRING("uart2", toString(logging::destination::uart2));
    TEST_ASSERT_EQUAL_STRING("unknown", toString(static_cast<logging::destination>(99U)));
}

void test_toString_source() {
    TEST_ASSERT_EQUAL_STRING("applicationEvents", toString(logging::source::applicationEvents));
    TEST_ASSERT_EQUAL_STRING("sensorEvents", toString(logging::source::sensorEvents));
    TEST_ASSERT_EQUAL_STRING("sensorData", toString(logging::source::sensorData));
    TEST_ASSERT_EQUAL_STRING("displayEvents", toString(logging::source::displayEvents));
    TEST_ASSERT_EQUAL_STRING("displayData", toString(logging::source::displayData));
    TEST_ASSERT_EQUAL_STRING("eepromData", toString(logging::source::eepromData));
    TEST_ASSERT_EQUAL_STRING("eepromEvents", toString(logging::source::eepromEvents));
    TEST_ASSERT_EQUAL_STRING("lorawanEvents", toString(logging::source::lorawanEvents));
    TEST_ASSERT_EQUAL_STRING("lorawanData", toString(logging::source::lorawanData));
    TEST_ASSERT_EQUAL_STRING("lorawanMac", toString(logging::source::lorawanMac));
    TEST_ASSERT_EQUAL_STRING("sx126xControl", toString(logging::source::sx126xControl));
    TEST_ASSERT_EQUAL_STRING("sx126xBufferData", toString(logging::source::sx126xBufferData));
    TEST_ASSERT_EQUAL_STRING("settings", toString(logging::source::settings));
    TEST_ASSERT_EQUAL_STRING("error", toString(logging::source::error));
    TEST_ASSERT_EQUAL_STRING("criticalError", toString(logging::source::criticalError));
    TEST_ASSERT_EQUAL_STRING("unknown", toString(static_cast<logging::source>(99U)));
}

void test_dump() {
    logging::dump();
    TEST_IGNORE_MESSAGE("For Coverage Only");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_enable_disable);
    RUN_TEST(test_output);
    RUN_TEST(test_output_source);
    RUN_TEST(test_toString_destination);
    RUN_TEST(test_toString_source);
    RUN_TEST(test_dump);
    UNITY_END();
}
