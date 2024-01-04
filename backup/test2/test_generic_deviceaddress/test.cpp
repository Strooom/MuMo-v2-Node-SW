#include <unity.h>
#include "deviceaddress.h"

void setUp(void) {}
void tearDown(void) {}

void test_initialize() {
    deviceAddress address1;
    uint8_t address1Bytes[4]{0};
    TEST_ASSERT_EQUAL_UINT32(0, address1.asUint32);                           // default constructor initializes to 0
    TEST_ASSERT_EQUAL_UINT8_ARRAY(address1Bytes, address1.asUint8, 4);        //

    deviceAddress address2(0x12345678);
    uint8_t address2Bytes[4] = {0x78, 0x56, 0x34, 0x12};
    TEST_ASSERT_EQUAL_UINT32(0x12345678, address2.asUint32);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(address2Bytes, address2.asUint8, 4);

    uint8_t address3Bytes[4] = {0x78, 0x56, 0x34, 0x12};
    deviceAddress address3(address3Bytes);
    TEST_ASSERT_EQUAL_UINT32(0x12345678, address3.asUint32);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(address3Bytes, address3.asUint8, 4);
}

void test_set() {
    deviceAddress address1;
    uint8_t address1Bytes[4]{0};
    TEST_ASSERT_EQUAL_UINT32(0, address1.asUint32);                           // default constructor initializes to 0
    TEST_ASSERT_EQUAL_UINT8_ARRAY(address1Bytes, address1.asUint8, 4);        //

    uint8_t address2Bytes[4] = {0x78, 0x56, 0x34, 0x12};
    address1.set(address2Bytes);
    TEST_ASSERT_EQUAL_UINT32(0x12345678, address1.asUint32);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(address2Bytes, address1.asUint8, 4);

    address1.set(0x87654321);
    TEST_ASSERT_EQUAL_UINT32(0x87654321, address1.asUint32);

    address1 = 0x12345678;
    TEST_ASSERT_EQUAL_UINT32(0x12345678, address1.asUint32);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_set);
    UNITY_END();
}