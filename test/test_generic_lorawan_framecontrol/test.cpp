#include <unity.h>
#include <framecontrol.hpp>

void setUp(void) {}
void tearDown(void) {}

void test_initialize() {
    frameControl ctrl1;
    TEST_ASSERT_EQUAL_UINT8(0, ctrl1.asByte());                           // default constructor initializes to 0

    frameControl ctrl2(linkDirection::uplink);
    TEST_ASSERT_EQUAL_UINT8(0, ctrl2.asByte());
}

void test_setFromByte() {
    frameControl ctrl1;
    ctrl1.set(0x80);
    TEST_ASSERT_TRUE(ctrl1.ADR);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_setFromByte);
    UNITY_END();
}