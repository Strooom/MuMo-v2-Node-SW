#include <unity.h>
#include <battery.hpp>

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

// bool checkVoltageVsCharge(uint32_t batteryTypeIndex) {
//     for (auto interpolationPointIndex = 0; interpolationPointIndex < nmbrInterpolationPoints - 2; interpolationPointIndex++) {
//         if (battery::voltageVsCharge[batteryTypeIndex][interpolationPointIndex].voltage >= battery::voltageVsCharge[batteryTypeIndex][interpolationPointIndex + 1].voltage) {
//             return false;
//         }
//     }
//     return true;
// }

// void test_checkVoltageVsCharge() {
//     TEST_ASSERT_TRUE(checkVoltageVsCharge(0));
//     TEST_ASSERT_FALSE(checkVoltageVsCharge(3));        // Battery type 3 has intentionally an error in its voltage vs charge table so we can test that case
// }

// void test_interpolation() {
//     TEST_ASSERT_EQUAL_UINT8(0, battery::calculateChargeLevel(2.0F));           // voltage below minimum
//     TEST_ASSERT_EQUAL_UINT8(0, battery::calculateChargeLevel(2.80F));          // voltage at minimum
//     TEST_ASSERT_EQUAL_UINT8(255, battery::calculateChargeLevel(3.60F));        // voltage at maximum
//     TEST_ASSERT_EQUAL_UINT8(255, battery::calculateChargeLevel(4.0F));         // voltage above maximum

//     TEST_ASSERT_EQUAL_UINT8(128, battery::calculateChargeLevel(3.2F));         // voltage at 50%
// }

// void test_measurements() {
//     TEST_ASSERT_TRUE(battery::isPresent());
//     battery::initalize();
//     battery::sample();

//     TEST_ASSERT_EQUAL_FLOAT(3.2F, battery::getVoltage());
//     TEST_ASSERT_EQUAL_FLOAT(128.0F, battery::getChargeLevel());
// }

int main(int argc, char **argv) {
    UNITY_BEGIN();
    // RUN_TEST(test_checkVoltageVsCharge);
    // RUN_TEST(test_interpolation);
    // RUN_TEST(test_measurements);
    UNITY_END();
}


