// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include "lorachannelcollection.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_initialize() {
    loRaChannelCollection theChannels;
    TEST_ASSERT_EQUAL_UINT32(868'100'000U, theChannels.txRxChannels[0].frequency);
    TEST_ASSERT_EQUAL_UINT32(868'300'000U, theChannels.txRxChannels[1].frequency);
    TEST_ASSERT_EQUAL_UINT32(868'500'000U, theChannels.txRxChannels[2].frequency);
}

void test_getRandomChannelIndex() {
    loRaChannelCollection theChannels;
    constexpr uint32_t testCount{1000};
    uint32_t nmbrOfSelections[loRaChannelCollection::maxNmbrChannels]{};

    for (uint32_t testCounter = 0; testCounter < testCount; testCounter++) {
        theChannels.selectRandomChannelIndex();
        TEST_ASSERT_TRUE(theChannels.currentChannelIndex < loRaChannelCollection::maxNmbrChannels);
        TEST_ASSERT_TRUE(theChannels.currentChannelIndex < 3);
        nmbrOfSelections[theChannels.currentChannelIndex]++;
    }
    TEST_ASSERT_TRUE(nmbrOfSelections[0] > (testCount / 3) - 100);
    TEST_ASSERT_TRUE(nmbrOfSelections[1] > (testCount / 3) - 100);
    TEST_ASSERT_TRUE(nmbrOfSelections[2] > (testCount / 3) - 100);

    TEST_ASSERT_EQUAL(0, nmbrOfSelections[3]);
    TEST_ASSERT_EQUAL(0, nmbrOfSelections[4]);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_getRandomChannelIndex);
    UNITY_END();
}
