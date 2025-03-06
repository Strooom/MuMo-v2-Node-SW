#include <unity.h>
#include <nvs.hpp>

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_isPresent() {
    TEST_ASSERT_EQUAL(0, nonVolatileStorage::isPresent());        // on the desktop this calls a dummy which always returns 0. Real test is on the target HW
}

void test_pageNumber() {
    TEST_ASSERT_EQUAL_UINT32(0, nonVolatileStorage::pageNumber(0));
    TEST_ASSERT_EQUAL_UINT32(0, nonVolatileStorage::pageNumber(nonVolatileStorage::pageSize - 1));
    TEST_ASSERT_EQUAL_UINT32(1, nonVolatileStorage::pageNumber(nonVolatileStorage::pageSize));
}

void test_bytesInCurrentPage() {
    static constexpr uint32_t quarterPageSize{nonVolatileStorage::pageSize / 4};
    static constexpr uint32_t halfPageSize{nonVolatileStorage::pageSize / 2};
    static constexpr uint32_t threeQuarterPageSize{3 * (nonVolatileStorage::pageSize / 4)};

    TEST_ASSERT_EQUAL_UINT32(quarterPageSize, nonVolatileStorage::bytesInCurrentPage(0, quarterPageSize));
    TEST_ASSERT_EQUAL_UINT32(quarterPageSize, nonVolatileStorage::bytesInCurrentPage(quarterPageSize, quarterPageSize));
    TEST_ASSERT_EQUAL_UINT32(quarterPageSize, nonVolatileStorage::bytesInCurrentPage(halfPageSize, quarterPageSize));
    TEST_ASSERT_EQUAL_UINT32(quarterPageSize, nonVolatileStorage::bytesInCurrentPage(threeQuarterPageSize, quarterPageSize));

    TEST_ASSERT_EQUAL_UINT32(halfPageSize, nonVolatileStorage::bytesInCurrentPage(0, halfPageSize));
    TEST_ASSERT_EQUAL_UINT32(halfPageSize, nonVolatileStorage::bytesInCurrentPage(quarterPageSize, halfPageSize));
    TEST_ASSERT_EQUAL_UINT32(halfPageSize, nonVolatileStorage::bytesInCurrentPage(halfPageSize, halfPageSize));
    TEST_ASSERT_EQUAL_UINT32(quarterPageSize, nonVolatileStorage::bytesInCurrentPage(threeQuarterPageSize, halfPageSize));

    TEST_ASSERT_EQUAL_UINT32(threeQuarterPageSize, nonVolatileStorage::bytesInCurrentPage(0, threeQuarterPageSize));
    TEST_ASSERT_EQUAL_UINT32(threeQuarterPageSize, nonVolatileStorage::bytesInCurrentPage(quarterPageSize, threeQuarterPageSize));
    TEST_ASSERT_EQUAL_UINT32(halfPageSize, nonVolatileStorage::bytesInCurrentPage(halfPageSize, threeQuarterPageSize));
    TEST_ASSERT_EQUAL_UINT32(quarterPageSize, nonVolatileStorage::bytesInCurrentPage(threeQuarterPageSize, threeQuarterPageSize));
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
    const uint32_t testDataLength{nonVolatileStorage::totalSize};
    uint8_t testBytesToRead[testDataLength]{};
    uint8_t expectedBytes[testDataLength];
    for (uint32_t i = 0; i < testDataLength; i++) {
        expectedBytes[i] = 0xFF;
    }

    nonVolatileStorage::erase();
    nonVolatileStorage::read(0, testBytesToRead, testDataLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBytes, testBytesToRead, testDataLength);
}

void test_fill() {
    const uint32_t testDataLength{nonVolatileStorage::totalSize};
    uint8_t testBytesToRead[testDataLength]{};
    uint8_t expectedBytes[testDataLength];
    for (uint32_t i = 0; i < testDataLength; i++) {
        expectedBytes[i] = 0xAA;
    }

    nonVolatileStorage::fill(0xAA);
    nonVolatileStorage::read(0, testBytesToRead, testDataLength);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBytes, testBytesToRead, testDataLength);
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_isPresent);
    RUN_TEST(test_pageNumber);
    RUN_TEST(test_bytesInCurrentPage);
    RUN_TEST(test_write_read_bytes);
    RUN_TEST(test_erase);
    RUN_TEST(test_fill);
    UNITY_END();
}