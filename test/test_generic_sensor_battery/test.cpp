#include <unity.h>
#include <battery.hpp>

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_initilization() {
    battery::initalize();
}

void test_measurements() {
    // TEST_ASSERT_TRUE(battery::isPresent());
    // battery::initalize();
    // battery::sample();

    // TEST_ASSERT_EQUAL_FLOAT(3.2F, battery::getVoltage());
    // TEST_ASSERT_EQUAL_FLOAT(128.0F, battery::getChargeLevel());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initilization);
    RUN_TEST(test_measurements);
    UNITY_END();
}


