#include <unity.h>
#include <txchannelcollection.hpp>

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_initalize() {
    TEST_ASSERT_EQUAL_UINT32(868'100'000U, loRaTxChannelCollection::channel[0].frequencyInHz);
    TEST_ASSERT_EQUAL_UINT32(868'300'000U, loRaTxChannelCollection::channel[1].frequencyInHz);
    TEST_ASSERT_EQUAL_UINT32(868'500'000U, loRaTxChannelCollection::channel[2].frequencyInHz);

    for (auto index = 3U; index < static_cast<uint32_t>(loRaTxChannelCollection::maxNmbrChannels); index++) {
        TEST_ASSERT_EQUAL_UINT32(0U, loRaTxChannelCollection::channel[index].frequencyInHz);
    }
    //TEST_ASSERT_EQUAL_UINT32(869'525'000U, loRaTxChannelCollection::rx2Channel.frequencyInHz);

    TEST_ASSERT_EQUAL_UINT32(0U, loRaTxChannelCollection::getCurrentChannelIndex());
}

void test_channel_selection() {
    TEST_ASSERT_EQUAL_UINT32(0U, loRaTxChannelCollection::getCurrentChannelIndex());
    loRaTxChannelCollection::selectNextActiveChannelIndex();
    TEST_ASSERT_EQUAL_UINT32(1U, loRaTxChannelCollection::getCurrentChannelIndex());
    loRaTxChannelCollection::selectNextActiveChannelIndex();
    TEST_ASSERT_EQUAL_UINT32(2U, loRaTxChannelCollection::getCurrentChannelIndex());
    loRaTxChannelCollection::selectNextActiveChannelIndex();
    TEST_ASSERT_EQUAL_UINT32(0U, loRaTxChannelCollection::getCurrentChannelIndex());
}

void test_random_channel_selection() {
uint32_t nmbrOfTimesSelected[loRaTxChannelCollection::maxNmbrChannels]{}; // initialize all elements to 0
    for (auto index = 0U; index < 1000U; index++) {
        loRaTxChannelCollection::selectRandomChannelIndex();
        nmbrOfTimesSelected[loRaTxChannelCollection::getCurrentChannelIndex()]++;
    }
        TEST_ASSERT(nmbrOfTimesSelected[0] > 200);
        TEST_ASSERT(nmbrOfTimesSelected[1] > 200);
        TEST_ASSERT(nmbrOfTimesSelected[2] > 200);
        TEST_ASSERT(nmbrOfTimesSelected[3] == 0);
        TEST_ASSERT(nmbrOfTimesSelected[4] == 0);
        TEST_ASSERT(nmbrOfTimesSelected[5] == 0);
        TEST_ASSERT(nmbrOfTimesSelected[6] == 0);
        TEST_ASSERT(nmbrOfTimesSelected[7] == 0);
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initalize);
    RUN_TEST(test_channel_selection);
    RUN_TEST(test_random_channel_selection);
    UNITY_END();
}
