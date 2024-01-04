#include <unity.h>
#include "framecontrol.h"

void setUp(void) {}
void tearDown(void) {}

void test_initialize() {
    frameControl ctrl1;
    TEST_ASSERT_EQUAL_UINT8(0, ctrl1.asByte());                           // default constructor initializes to 0

    frameControl ctrl2(linkDirection::uplink);
    TEST_ASSERT_EQUAL_UINT8(0, ctrl2.asByte());
}

void test_fromByte() {
    frameControl ctrl1;
    ctrl1.fromByte(0x80);
    TEST_ASSERT_TRUE(ctrl1.ADR);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_fromByte);
    UNITY_END();
}