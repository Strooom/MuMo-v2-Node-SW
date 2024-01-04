#include <unity.h>
#include <settingscollection.hpp>
#include <nvs.hpp>
#include <stdio.h>

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_isValidBlockIndex() {
    TEST_ASSERT_TRUE(settingsCollection::isValidIndex(settingsCollection::settingIndex::nvsMapVersion));
    TEST_ASSERT_TRUE(settingsCollection::isValidIndex(settingsCollection::settingIndex::unusedLoRaWAN));
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
        uint32_t endPage = (settingsCollection::settings[index].startAddress + settingsCollection::settings[index].length - 1) / 128;
        char text[128];
        snprintf(text, 128, "index %d", index);
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(startPage, endPage, text );
    }
}


void test_write_read_setting() {
    uint8_t testByte{123};
    settingsCollection::save<uint8_t>(settingsCollection::settingIndex::nvsMapVersion, testByte);
    TEST_ASSERT_EQUAL_UINT8(testByte, settingsCollection::read<uint8_t>(settingsCollection::settingIndex::nvsMapVersion));

    settingsCollection::save(settingsCollection::settingIndex::nvsMapVersion, testByte);
    TEST_ASSERT_EQUAL_UINT8(testByte, settingsCollection::read<uint8_t>(settingsCollection::settingIndex::nvsMapVersion));

    uint32_t testDoubleWord{0x55AA00FF};
    settingsCollection::save<uint32_t>(settingsCollection::settingIndex::uplinkFrameCounter, testDoubleWord);
    TEST_ASSERT_EQUAL_UINT32(testDoubleWord, settingsCollection::read<uint32_t>(settingsCollection::settingIndex::uplinkFrameCounter));

    settingsCollection::save(settingsCollection::settingIndex::uplinkFrameCounter, testDoubleWord);
    TEST_ASSERT_EQUAL_UINT32(testDoubleWord, settingsCollection::read<uint32_t>(settingsCollection::settingIndex::uplinkFrameCounter));

    uint8_t testArrayIn[16]{0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    settingsCollection::save(settingsCollection::settingIndex::networkSessionKey, testArrayIn);
    uint8_t testArrayOut[16]{};
    settingsCollection::read(settingsCollection::settingIndex::networkSessionKey, testArrayOut);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testArrayIn, testArrayOut, 16);
}

void test_is_initialized() {
    nonVolatileStorage::erase();
    TEST_ASSERT_FALSE(settingsCollection::isInitialized());
    settingsCollection::save<uint8_t>(settingsCollection::settingIndex::nvsMapVersion, 0x01);
    TEST_ASSERT_TRUE(settingsCollection::isInitialized());
}

void test_initialize_once() {
    nonVolatileStorage::erase();
    TEST_ASSERT_FALSE(settingsCollection::isInitialized());
    settingsCollection::initializeOnce();
    TEST_ASSERT_EQUAL_UINT8(0x01, settingsCollection::read<uint8_t>(settingsCollection::settingIndex::nvsMapVersion));

    uint8_t testArrayExpected[16]{};
    uint8_t testArrayOut[16]{};
    settingsCollection::read(settingsCollection::settingIndex::networkSessionKey, testArrayOut);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testArrayExpected, testArrayOut, 16);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_isValidBlockIndex);
    RUN_TEST(test_blockOverlap);
    RUN_TEST(test_allBlocksWithinOnePage);
    RUN_TEST(test_write_read_setting);
    RUN_TEST(test_is_initialized);
    RUN_TEST(test_initialize_once);
    UNITY_END();
}
