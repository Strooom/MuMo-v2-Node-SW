#include <unity.h>
#include <batterytype.hpp>

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}



void test_toString() {
    TEST_ASSERT_EQUAL_STRING("LiFePO4 700mAh", toString(batteryType::liFePO4_700mAh));
    TEST_ASSERT_EQUAL_STRING("LiFePO4 1500mAh", toString(batteryType::liFePO4_1500mAh));
    TEST_ASSERT_EQUAL_STRING("Alkaline 1200mAh", toString(batteryType::alkaline_1200mAh));
    TEST_ASSERT_EQUAL_STRING("Saft LS14250", toString(batteryType::saft_ls_14250));
    TEST_ASSERT_EQUAL_STRING("Saft LS14500", toString(batteryType::saft_ls_14500));

    TEST_ASSERT_EQUAL_STRING("Unknown battery type", toString(batteryType::nmbrBatteryTypes));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_toString);
    UNITY_END();
}
