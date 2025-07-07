#include <unity.h>
#include <nvs.hpp>

void setUp(void) {}
void tearDown(void) {}

void test_isPresent() {
    nonVolatileStorage::mockEepromNmbr64KPages = 0;
    TEST_ASSERT_EQUAL(0, nonVolatileStorage::getNmbr64KBanks());
    nonVolatileStorage::mockEepromNmbr64KPages = 4;
    TEST_ASSERT_EQUAL(4, nonVolatileStorage::getNmbr64KBanks());
}

void test_totalSize() {
    nonVolatileStorage::mockEepromNmbr64KPages = 1;
    nonVolatileStorage::detectNmbr64KBanks();
    TEST_ASSERT_EQUAL_UINT32(64 * 1024, nonVolatileStorage::totalSize());
    nonVolatileStorage::mockEepromNmbr64KPages = 2;
    nonVolatileStorage::detectNmbr64KBanks();
    TEST_ASSERT_EQUAL_UINT32(128 * 1024, nonVolatileStorage::totalSize());
    nonVolatileStorage::mockEepromNmbr64KPages = 4;
    nonVolatileStorage::detectNmbr64KBanks();
    TEST_ASSERT_EQUAL_UINT32(256 * 1024, nonVolatileStorage::totalSize());
}

void test_getMeasurementsAreaSize() {
    nonVolatileStorage::mockEepromNmbr64KPages = 1;
    nonVolatileStorage::detectNmbr64KBanks();
    TEST_ASSERT_EQUAL_UINT32(60 * 1024, nonVolatileStorage::getMeasurementsAreaSize());
    nonVolatileStorage::mockEepromNmbr64KPages = 2;
    nonVolatileStorage::detectNmbr64KBanks();
    TEST_ASSERT_EQUAL_UINT32(124 * 1024, nonVolatileStorage::getMeasurementsAreaSize());
    nonVolatileStorage::mockEepromNmbr64KPages = 4;
    nonVolatileStorage::detectNmbr64KBanks();
    TEST_ASSERT_EQUAL_UINT32(252 * 1024, nonVolatileStorage::getMeasurementsAreaSize());
}


void test_getNumberOfBanks() {
    nonVolatileStorage::mockEepromNmbr64KPages = 1;
    nonVolatileStorage::detectNmbr64KBanks();
    TEST_ASSERT_EQUAL_UINT32(1, nonVolatileStorage::getNmbr64KBanks());
    nonVolatileStorage::mockEepromNmbr64KPages = 2;
    nonVolatileStorage::detectNmbr64KBanks();
    TEST_ASSERT_EQUAL_UINT32(2, nonVolatileStorage::getNmbr64KBanks());
    nonVolatileStorage::mockEepromNmbr64KPages = 4;
    nonVolatileStorage::detectNmbr64KBanks();
    TEST_ASSERT_EQUAL_UINT32(4, nonVolatileStorage::getNmbr64KBanks());
}


void test_bankNumber() {
    nonVolatileStorage::mockEepromNmbr64KPages = 8;
    nonVolatileStorage::detectNmbr64KBanks();
    for (uint32_t bankIndex = 0; bankIndex < nonVolatileStorage::getNmbr64KBanks(); bankIndex++) {
        TEST_ASSERT_EQUAL_UINT32(bankIndex, nonVolatileStorage::bankNumber(bankIndex * 64U * 1024U));
        TEST_ASSERT_EQUAL_UINT32(bankIndex, nonVolatileStorage::bankNumber(((bankIndex + 1) * 64U * 1024U) - 1));
    }
}

void test_bankI2cAddress() {
    nonVolatileStorage::mockEepromNmbr64KPages = 8;
    nonVolatileStorage::detectNmbr64KBanks();
    for (uint32_t bankIndex = 0; bankIndex < nonVolatileStorage::getNmbr64KBanks(); bankIndex++) {
        TEST_ASSERT_EQUAL_UINT32(nonVolatileStorage::baseI2cAddress + bankIndex, nonVolatileStorage::bankI2cAddress(bankIndex * 64U * 1024U));
        TEST_ASSERT_EQUAL_UINT32(nonVolatileStorage::baseI2cAddress + bankIndex, nonVolatileStorage::bankI2cAddress(((bankIndex + 1) * 64U * 1024U) - 1));
    }
}

void test_bankOffset() {
    nonVolatileStorage::mockEepromNmbr64KPages = 8;
    nonVolatileStorage::detectNmbr64KBanks();
    for (uint32_t bankIndex = 0; bankIndex < nonVolatileStorage::getNmbr64KBanks(); bankIndex++) {
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
    nonVolatileStorage::mockEepromNmbr64KPages = 8;
    nonVolatileStorage::detectNmbr64KBanks();
    for (uint32_t address = 0; address < nonVolatileStorage::totalSize(); address++) {
        nonVolatileStorage::write(address, 0xAA);
        TEST_ASSERT_EQUAL_UINT8(0xAA, nonVolatileStorage::read(address));
        nonVolatileStorage::write(address, 0x55);
        TEST_ASSERT_EQUAL_UINT8(0x55, nonVolatileStorage::read(address));
    }
}

void test_write_read_rangeOfBytesInPage() {
    nonVolatileStorage::mockEepromNmbr64KPages = 8;
    nonVolatileStorage::detectNmbr64KBanks();
    const uint32_t numberOfPages{nonVolatileStorage::totalSize() / nonVolatileStorage::pageSize};

    uint8_t testBytesToWrite[nonVolatileStorage::pageSize];
    uint8_t testBytesToRead[nonVolatileStorage::pageSize]{};
    for (uint32_t i = 0; i < nonVolatileStorage::pageSize; i++) {
        testBytesToWrite[i] = static_cast<uint8_t>(i);
    }

    for (uint32_t page = 0; page < numberOfPages; page++) {
        uint32_t testAddress = page * nonVolatileStorage::pageSize;
        nonVolatileStorage::writeInPage(testAddress, testBytesToWrite, nonVolatileStorage::pageSize);
        nonVolatileStorage::readInPage(testAddress, testBytesToRead, nonVolatileStorage::pageSize);
        TEST_ASSERT_EQUAL_UINT8_ARRAY(testBytesToWrite, testBytesToRead, nonVolatileStorage::pageSize);
    }

    for (uint32_t i = 0; i < nonVolatileStorage::pageSize; i++) {
        testBytesToWrite[i] = testBytesToWrite[i] ^ 0xFF;
    }

    for (uint32_t page = 0; page < numberOfPages; page++) {
        uint32_t testAddress = page * nonVolatileStorage::pageSize;
        nonVolatileStorage::writeInPage(testAddress, testBytesToWrite, nonVolatileStorage::pageSize);
        nonVolatileStorage::readInPage(testAddress, testBytesToRead, nonVolatileStorage::pageSize);
        TEST_ASSERT_EQUAL_UINT8_ARRAY(testBytesToWrite, testBytesToRead, nonVolatileStorage::pageSize);
    }
}

void test_write_read_rangeOfBytesAcrossPages() {
    nonVolatileStorage::mockEepromNmbr64KPages = 8;
    nonVolatileStorage::detectNmbr64KBanks();
    uint8_t testBytesToWrite[nonVolatileStorage::totalSize()];
    uint8_t testBytesToRead[nonVolatileStorage::totalSize()]{};

    for (uint32_t i = 0; i < nonVolatileStorage::totalSize(); i++) {
        testBytesToWrite[i] = static_cast<uint8_t>(i % 256);
    }

    nonVolatileStorage::write(0, testBytesToWrite, nonVolatileStorage::totalSize());
    nonVolatileStorage::read(0, testBytesToRead, nonVolatileStorage::totalSize());
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testBytesToWrite, testBytesToRead, nonVolatileStorage::totalSize());

    for (uint32_t i = 0; i < nonVolatileStorage::totalSize(); i++) {
        testBytesToWrite[i] = testBytesToWrite[i] ^ 0xFF;
    }

    nonVolatileStorage::write(0, testBytesToWrite, nonVolatileStorage::totalSize());
    nonVolatileStorage::read(0, testBytesToRead, nonVolatileStorage::totalSize());
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testBytesToWrite, testBytesToRead, nonVolatileStorage::totalSize());
}

void test_erase() {
    nonVolatileStorage::mockEepromNmbr64KPages = 8;
    nonVolatileStorage::detectNmbr64KBanks();

    const uint32_t testDataLength{nonVolatileStorage::totalSize()};
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
    nonVolatileStorage::mockEepromNmbr64KPages = 8;
    nonVolatileStorage::detectNmbr64KBanks();

    const uint32_t testDataLength{nonVolatileStorage::totalSize()};
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
    RUN_TEST(test_totalSize);
    RUN_TEST(test_getMeasurementsAreaSize);
    RUN_TEST(test_getNumberOfBanks);
    RUN_TEST(test_bankNumber);
    RUN_TEST(test_bankI2cAddress);
    RUN_TEST(test_bankOffset);
    RUN_TEST(test_pageNumber);
    RUN_TEST(test_bytesInCurrentPage);
    RUN_TEST(test_write_read_bytes);
    RUN_TEST(test_write_read_rangeOfBytesInPage);
    RUN_TEST(test_write_read_rangeOfBytesAcrossPages);
    RUN_TEST(test_erase);
    RUN_TEST(test_fill);
    UNITY_END();
}