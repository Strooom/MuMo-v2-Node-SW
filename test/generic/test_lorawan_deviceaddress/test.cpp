#include <unity.h>
#include <deviceaddress.hpp>

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_initialize() {
    deviceAddress address1;
    uint8_t address1Bytes[4]{0};
    TEST_ASSERT_EQUAL_UINT32(0, address1.asUint32);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(address1Bytes, address1.asUint8, 4);

    deviceAddress address2(0x12345678);
    uint8_t address2Bytes[4] = {0x78, 0x56, 0x34, 0x12};
    TEST_ASSERT_EQUAL_UINT32(0x12345678, address2.asUint32);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(address2Bytes, address2.asUint8, 4);
}

void test_operatorAssign() {
    deviceAddress testAddress1;
    deviceAddress testAddress2;
    testAddress1 = 0x12345678;
    TEST_ASSERT_EQUAL_UINT32(0x12345678, testAddress1.asUint32);
    testAddress2 = testAddress1;
    TEST_ASSERT_EQUAL_UINT32(0x12345678, testAddress2.asUint32);
}

void test_isEqualOperator() {
    deviceAddress address1(0x12345678);
    deviceAddress address2(0x12345678);
    deviceAddress address3(0x87654321);
    TEST_ASSERT_TRUE(address1 == address2);
    TEST_ASSERT_FALSE(address1 == address3);
}

void test_isNotEqualOperator() {
    deviceAddress address1(0x12345678);
    deviceAddress address2(0x12345678);
    deviceAddress address3(0x87654321);
    TEST_ASSERT_FALSE(address1 != address2);
    TEST_ASSERT_TRUE(address1 != address3);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_operatorAssign);
    RUN_TEST(test_isEqualOperator);
    RUN_TEST(test_isNotEqualOperator);
    UNITY_END();
}