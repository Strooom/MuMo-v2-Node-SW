#include <unity.h>
#include <sht40.hpp>
#include <cstring>

void setUp(void) {}
void tearDown(void) {}

void test_calculateTemperature() {
    sht40::rawDataTemperature = 0x6666;
    TEST_ASSERT_EQUAL(25.0F, sht40::calculateTemperature());
}

void test_calculateRelativeHumidity() {
    sht40::rawDataRelativeHumidity = 0x72B0;
    TEST_ASSERT_EQUAL(50.0F, sht40::calculateRelativeHumidity());

    sht40::rawDataRelativeHumidity = 0x0;
    TEST_ASSERT_EQUAL(0.0F, sht40::calculateRelativeHumidity());

    sht40::rawDataRelativeHumidity = 0xFFFF;
    TEST_ASSERT_EQUAL(100.0F, sht40::calculateRelativeHumidity());
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_calculateTemperature);
    RUN_TEST(test_calculateRelativeHumidity);
    UNITY_END();
}
