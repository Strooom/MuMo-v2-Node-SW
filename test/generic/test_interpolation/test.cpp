#include <unity.h>
#include <chargefromvoltage.hpp>

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_checkVoltageVsCharge() {
    TEST_ASSERT_TRUE(chargeFromVoltage::checkVoltageVsCharge(0));
    TEST_ASSERT_FALSE(chargeFromVoltage::checkVoltageVsCharge(3));        // Battery type 3 has intentionally an error in its voltage vs charge table so we can test that case
}

void test_interpolation() {
    TEST_ASSERT_EQUAL(0.0, chargeFromVoltage::calculateChargeLevel(2.0F, batteryType::liFePO4_700mAh));           // voltage below minimum
    TEST_ASSERT_EQUAL(0.0, chargeFromVoltage::calculateChargeLevel(2.80F, batteryType::liFePO4_700mAh));          // voltage at minimum
    TEST_ASSERT_EQUAL(1.0, chargeFromVoltage::calculateChargeLevel(3.60F, batteryType::liFePO4_700mAh));        // voltage at maximum
    TEST_ASSERT_EQUAL(1.0, chargeFromVoltage::calculateChargeLevel(4.0F, batteryType::liFePO4_700mAh));         // voltage above maximum

    TEST_ASSERT_EQUAL(0.5, chargeFromVoltage::calculateChargeLevel(3.2F, batteryType::liFePO4_700mAh));         // voltage at 50%
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_checkVoltageVsCharge);
    RUN_TEST(test_interpolation);
    UNITY_END();
}


