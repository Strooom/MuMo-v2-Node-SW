#include <unity.h>
#include "nvs.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_isValid() {
    nonVolatileStorage nvs;
    TEST_ASSERT_TRUE(nvs.blockIndexIsValid(0));
    TEST_ASSERT_TRUE(nvs.blockIndexIsValid(static_cast<uint32_t>(nvsMap::blockIndex::numberOfBlocks) - 1));
    TEST_ASSERT_FALSE(nvs.blockIndexIsValid(static_cast<uint32_t>(nvsMap::blockIndex::numberOfBlocks)));
}

void test_overlap() {
    nvsMap theMap;
    for (uint32_t index = 0; index < (static_cast<uint32_t>(nvsMap::blockIndex::numberOfBlocks) - 1); index++) {
        TEST_ASSERT_EQUAL_UINT32(theMap.blocks[index].startAddress + theMap.blocks[index].length, theMap.blocks[index + 1].startAddress);
    }
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_isValid);
    RUN_TEST(test_overlap);
    UNITY_END();
}