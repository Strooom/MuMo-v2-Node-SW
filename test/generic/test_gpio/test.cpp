#include <unity.h>
#include <gpio.hpp>

void setUp(void) {}
void tearDown(void) {}

void test_enableDisable() {
    gpio::initialize();
    gpio::enableGpio(gpio::group::usbPresent);
    gpio::disableGpio(gpio::group::debugPort);
    TEST_ASSERT_FALSE(gpio::isDebugProbePresent());
    TEST_IGNORE_MESSAGE("For coverage only");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_enableDisable);
    UNITY_END();
}