#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <i2c.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>
#include <nvs.hpp>

#include <applicationevent.hpp>
#include <circularbuffer.hpp>
circularBuffer<applicationEvent, 16U> applicationEventBuffer;

static constexpr uint32_t nmbrOfEepromChipsOnHw{2};
static constexpr uint32_t nmbrOf64KBanksPerChip{1};

void setUp(void) {}
void tearDown(void) {}

void test_isPresent() {
    uint32_t expected{nmbrOfEepromChipsOnHw * nmbrOf64KBanksPerChip};
    uint32_t actual{nonVolatileStorage::detectNmbr64KBanks()};
    TEST_ASSERT_EQUAL(expected, actual);
}

void test_totalSize() {
    TEST_ASSERT_EQUAL_UINT32(nmbrOfEepromChipsOnHw * nmbrOf64KBanksPerChip * (64 * 1024), nonVolatileStorage::totalSize());
}

void test_bankNumber() {
    for (uint32_t bankIndex = 0; bankIndex < nonVolatileStorage::detectNmbr64KBanks(); bankIndex++) {
        TEST_ASSERT_EQUAL_UINT32(bankIndex, nonVolatileStorage::bankNumber(bankIndex * 64U * 1024U));
        TEST_ASSERT_EQUAL_UINT32(bankIndex, nonVolatileStorage::bankNumber(((bankIndex + 1) * 64U * 1024U) - 1));
    }
}

void test_bankI2cAddress() {
    for (uint32_t bankIndex = 0; bankIndex < nonVolatileStorage::detectNmbr64KBanks(); bankIndex++) {
        TEST_ASSERT_EQUAL_UINT32(nonVolatileStorage::baseI2cAddress + bankIndex, nonVolatileStorage::bankI2cAddress(bankIndex * 64U * 1024U));
        TEST_ASSERT_EQUAL_UINT32(nonVolatileStorage::baseI2cAddress + bankIndex, nonVolatileStorage::bankI2cAddress(((bankIndex + 1) * 64U * 1024U) - 1));
    }
}

void test_bankOffset() {
    for (uint32_t bankIndex = 0; bankIndex < nonVolatileStorage::detectNmbr64KBanks(); bankIndex++) {
        for (uint32_t offsetIndex = 0; offsetIndex < 64U * 1024U; offsetIndex++) {
            TEST_ASSERT_EQUAL_UINT16(offsetIndex, nonVolatileStorage::bankOffset((bankIndex * 64U * 1024U) + offsetIndex));
        }
    }
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
    static constexpr uint32_t testOffset{8192};
    static constexpr uint32_t testRange{64};
    for (uint32_t address = 0; address < testRange; address++) {
        nonVolatileStorage::write(testOffset + address, 0xAA);
        TEST_ASSERT_EQUAL_UINT8(0xAA, nonVolatileStorage::read(testOffset + address));
        nonVolatileStorage::write(testOffset + address, 0x55);
        TEST_ASSERT_EQUAL_UINT8(0x55, nonVolatileStorage::read(testOffset + address));
    }
}

void test_write_read_rangeOfBytesInPage() {
    static constexpr uint32_t testOffset{8192};
    static constexpr uint32_t testRange{64};

    uint8_t testBytesToWrite[testRange];
    uint8_t testBytesToRead[testRange]{};

    for (uint32_t i = 0; i < testRange; i++) {
        testBytesToWrite[i] = static_cast<uint8_t>(i);
    }

    nonVolatileStorage::writeInPage(testOffset, testBytesToWrite, testRange);
    nonVolatileStorage::readInPage(testOffset, testBytesToRead, testRange);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testBytesToWrite, testBytesToRead, testRange);

    for (uint32_t i = 0; i < testRange; i++) {
        testBytesToWrite[i] = testBytesToWrite[i] ^ 0xFF;
    }

    nonVolatileStorage::writeInPage(testOffset, testBytesToWrite, testRange);
    nonVolatileStorage::readInPage(testOffset, testBytesToRead, testRange);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testBytesToWrite, testBytesToRead, testRange);
}

void test_write_read_rangeOfBytesAcrossPages() {
    static constexpr uint32_t testOffset{8192};
    static constexpr uint32_t testRange{256};

    uint8_t testBytesToWrite[testRange];
    uint8_t testBytesToRead[testRange]{};

    for (uint32_t i = 0; i < testRange; i++) {
        testBytesToWrite[i] = static_cast<uint8_t>(i);
    }

    nonVolatileStorage::write(testOffset, testBytesToWrite, testRange);
    nonVolatileStorage::read(testOffset, testBytesToRead, testRange);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testBytesToWrite, testBytesToRead, testRange);

    for (uint32_t i = 0; i < nonVolatileStorage::pageSize; i++) {
        testBytesToWrite[i] = testBytesToWrite[i] ^ 0xFF;
    }

    nonVolatileStorage::write(testOffset, testBytesToWrite, testRange);
    nonVolatileStorage::read(testOffset, testBytesToRead, testRange);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testBytesToWrite, testBytesToRead, testRange);
}


void test_erase() {
    nonVolatileStorage::erase();
    for (uint32_t address = 0; address < nonVolatileStorage::totalSize(); address++) {
        TEST_ASSERT_EQUAL_UINT8(nonVolatileStorage::blankEepromValue, nonVolatileStorage::read(address));
    }
}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(3000);
    SystemClock_Config();
    gpio::initialize();
    i2c::wakeUp();

    UNITY_BEGIN();
    RUN_TEST(test_isPresent);
    RUN_TEST(test_totalSize);
    RUN_TEST(test_bankNumber);
    RUN_TEST(test_bankI2cAddress);
    RUN_TEST(test_bankOffset);
    RUN_TEST(test_pageNumber);
    RUN_TEST(test_bytesInCurrentPage);
    RUN_TEST(test_write_read_bytes);
    RUN_TEST(test_write_read_rangeOfBytesInPage);
    RUN_TEST(test_write_read_rangeOfBytesAcrossPages);
    RUN_TEST(test_erase);
    UNITY_END();
}
