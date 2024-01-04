#include <unity.h>
#include <nvs.hpp>

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_isPresent() {
    TEST_ASSERT_TRUE(nonVolatileStorage::isPresent());        // on the desktop this calls a dummy which always returns true. Real test is on the target HW
}

void test_write_read_bytes() {
    uint32_t testAddress{0};
    const uint32_t testDataLength{16};
    uint8_t testBytesToWrite[testDataLength]{0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    uint8_t testBytesToRead[testDataLength]{};

    nonVolatileStorage::write(testAddress, testBytesToWrite, testDataLength);
    nonVolatileStorage::read(testAddress, testBytesToRead, testDataLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testBytesToWrite, testBytesToRead, testDataLength);
}

void test_erase() {
    const uint32_t testDataLength{nonVolatileStorage::size};
    uint8_t testBytesToRead[testDataLength]{};
    uint8_t expectedBytes[testDataLength];
    for (uint32_t i = 0; i < testDataLength; i++) {
        expectedBytes[i] = 0xFF;
    }

    nonVolatileStorage::erase();
    nonVolatileStorage::read(0, testBytesToRead, testDataLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBytes, testBytesToRead, testDataLength);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_isPresent);
    RUN_TEST(test_write_read_bytes);
    RUN_TEST(test_erase);
    UNITY_END();
}