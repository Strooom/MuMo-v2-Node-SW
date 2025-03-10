#include <unity.h>
#include <txchannelcollection.hpp>

void setUp(void) {}
void tearDown(void) {}

void test_initalize() {
    TEST_ASSERT_EQUAL_UINT32(868'100'000U, loRaTxChannelCollection::channel[0].frequencyInHz);
    TEST_ASSERT_EQUAL_UINT32(868'300'000U, loRaTxChannelCollection::channel[1].frequencyInHz);
    TEST_ASSERT_EQUAL_UINT32(868'500'000U, loRaTxChannelCollection::channel[2].frequencyInHz);

    for (auto index = 3U; index < loRaTxChannelCollection::maxNmbrChannels; index++) {
        TEST_ASSERT_EQUAL_UINT32(0U, loRaTxChannelCollection::channel[index].frequencyInHz);
    }
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
    uint32_t nmbrOfTimesSelected[loRaTxChannelCollection::maxNmbrChannels]{};
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

void test_nmbrActiveChannels() {
    TEST_ASSERT_EQUAL_UINT32(3U, loRaTxChannelCollection::nmbrActiveChannels());
    loRaTxChannelCollection::channel[7].frequencyInHz = 868'400'000U;
    TEST_ASSERT_EQUAL_UINT32(4U, loRaTxChannelCollection::nmbrActiveChannels());
}

void test_reset() {
    loRaTxChannelCollection::channel[0].frequencyInHz = 868'200'000U;
    loRaTxChannelCollection::reset();
    TEST_ASSERT_EQUAL_UINT32(868'100'000U, loRaTxChannelCollection::channel[0].frequencyInHz);
    TEST_ASSERT_EQUAL_UINT32(868'300'000U, loRaTxChannelCollection::channel[1].frequencyInHz);
    TEST_ASSERT_EQUAL_UINT32(868'500'000U, loRaTxChannelCollection::channel[2].frequencyInHz);
    for (auto index = 3U; index < loRaTxChannelCollection::maxNmbrChannels; index++) {
        TEST_ASSERT_EQUAL_UINT32(0U, loRaTxChannelCollection::channel[index].frequencyInHz);
    }
    TEST_ASSERT_EQUAL_UINT32(3U, loRaTxChannelCollection::nmbrActiveChannels());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initalize);
    RUN_TEST(test_channel_selection);
    RUN_TEST(test_random_channel_selection);
    RUN_TEST(test_nmbrActiveChannels);
    RUN_TEST(test_reset);
    UNITY_END();
}
