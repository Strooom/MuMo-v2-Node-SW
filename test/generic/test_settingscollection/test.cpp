#include <unity.h>
#include <settingscollection.hpp>
#include <nvs.hpp>
#include <stdio.h>

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_isValidBlockIndex() {
    TEST_ASSERT_TRUE(settingsCollection::isValidIndex(settingsCollection::settingIndex::mapVersion));
    TEST_ASSERT_TRUE(settingsCollection::isValidIndex(settingsCollection::settingIndex::unusedLoRaWAN2));
    TEST_ASSERT_FALSE(settingsCollection::isValidIndex(settingsCollection::settingIndex::numberOfSettings));
}

void test_blockOverlap() {
    for (uint32_t index = 0; index < (static_cast<uint32_t>(settingsCollection::settingIndex::numberOfSettings) - 1); index++) {
        TEST_ASSERT_EQUAL_UINT32(settingsCollection::settings[index].startAddress + settingsCollection::settings[index].length, settingsCollection::settings[index + 1].startAddress);
    }
}

void test_allBlocksWithinOnePage() {
    for (uint32_t index = 0; index < (static_cast<uint32_t>(settingsCollection::settingIndex::numberOfSettings) - 1); index++) {
        uint32_t startPage = (settingsCollection::settings[index].startAddress) / 128;
        uint32_t endPage   = (settingsCollection::settings[index].startAddress + settingsCollection::settings[index].length - 1) / 128;
        TEST_ASSERT_EQUAL_UINT32(startPage, endPage);
    }
}

void test_write_read_setting() {
    uint8_t testByte{123};
    settingsCollection::save<uint8_t>(testByte, settingsCollection::settingIndex::mapVersion);
    TEST_ASSERT_EQUAL_UINT8(testByte, settingsCollection::read<uint8_t>(settingsCollection::settingIndex::mapVersion));

    settingsCollection::save(testByte, settingsCollection::settingIndex::mapVersion);
    TEST_ASSERT_EQUAL_UINT8(testByte, settingsCollection::read<uint8_t>(settingsCollection::settingIndex::mapVersion));

    uint32_t testDoubleWord{0x55AA00FF};
    settingsCollection::save<uint32_t>(testDoubleWord, settingsCollection::settingIndex::uplinkFrameCounter);
    TEST_ASSERT_EQUAL_UINT32(testDoubleWord, settingsCollection::read<uint32_t>(settingsCollection::settingIndex::uplinkFrameCounter));

    settingsCollection::save(testDoubleWord, settingsCollection::settingIndex::uplinkFrameCounter);
    TEST_ASSERT_EQUAL_UINT32(testDoubleWord, settingsCollection::read<uint32_t>(settingsCollection::settingIndex::uplinkFrameCounter));

    uint8_t testArrayIn[16]{0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    settingsCollection::saveByteArray(testArrayIn, settingsCollection::settingIndex::networkSessionKey);
    uint8_t testArrayOut[16]{};
    settingsCollection::readByteArray(testArrayOut, settingsCollection::settingIndex::networkSessionKey);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testArrayIn, testArrayOut, 16);
}

void test_is_initialized() {
    nonVolatileStorage::erase();
    TEST_ASSERT_FALSE(settingsCollection::isValid());
    settingsCollection::save<uint8_t>(0x01, settingsCollection::settingIndex::mapVersion);
    TEST_ASSERT_TRUE(settingsCollection::isValid());
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_isValidBlockIndex);
    RUN_TEST(test_blockOverlap);
    RUN_TEST(test_allBlocksWithinOnePage);
    RUN_TEST(test_write_read_setting);
    RUN_TEST(test_is_initialized);
    UNITY_END();
}
