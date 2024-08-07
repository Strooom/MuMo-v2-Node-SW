#include <unity.h>
#include <gpio.hpp>

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_enableDisable() {
    gpio::enableGpio(gpio::group::usbPresent);
    gpio::disableGpio(gpio::group::debugPort);
    gpio::disableAllGpio();
    TEST_IGNORE_MESSAGE("For coverage only");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_enableDisable);
    UNITY_END();
}